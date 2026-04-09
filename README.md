# Data Retention, Cleanup Cronjobs & Policy

> Phiên bản: v1.0 | Áp dụng cho: Marketplace Microservices (Java 25 / Spring Boot 4.0.4)
> Tất cả cronjob chạy trong **Worker Service** — Quartz Scheduler + ShedLock (PostgreSQL provider)

---

## 1. NGUYÊN TẮC CHUNG

| Nguyên tắc | Nội dung |
|---|---|
| **Soft Delete First** | Mọi dữ liệu cần xóa đều được đánh dấu trước (soft delete), sau một khoảng thời gian grace period mới hard delete. |
| **Distributed Lock** | Mọi cronjob dùng ShedLock — đảm bảo chỉ 1 node chạy 1 job tại 1 thời điểm trong môi trường multi-instance. |
| **Audit Trail** | Các bảng tài chính (`TRANSACTIONS`, `REFUNDS`, `POINT_TRANSACTIONS`) **không bao giờ bị hard delete**. Chỉ archive nếu cần. |
| **Idempotent** | Mọi cleanup job phải idempotent — chạy 2 lần kết quả như chạy 1 lần. |
| **Off-peak Execution** | Tất cả job nặng chạy ngoài giờ cao điểm: **02:00 – 05:00 UTC+7**. |
| **Batch Size** | Mỗi lần xử lý tối đa 500–1000 bản ghi để tránh lock table. Dùng `LIMIT` + loop nếu cần. |

---

## 2. DANH SÁCH CRONJOB

---

### JOB-01 · Flash Sale Session Lifecycle Manager

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Tự động chuyển trạng thái `FS_SESSIONS`: UPCOMING → ACTIVE khi đến giờ, ACTIVE → ENDED khi hết giờ. |
| **Cron** | `0 * * * * *` (mỗi 1 phút) |
| **ShedLock name** | `flash-sale-session-lifecycle` |
| **Lock duration** | 55 giây |
| **Bảng tác động** | `FS_SESSIONS`, `FS_ITEMS` |

**Logic:**
```sql
-- Activate sessions
UPDATE FS_SESSIONS
SET status = 'ACTIVE', updated_at = NOW()
WHERE status = 'UPCOMING' AND start_time <= NOW();

-- End sessions
UPDATE FS_SESSIONS
SET status = 'ENDED', updated_at = NOW()
WHERE status = 'ACTIVE' AND end_time <= NOW();

-- Sync items của session vừa ENDED (không cần approve thêm)
UPDATE FS_ITEMS SET status = 'CANCELLED'
WHERE session_id IN (SELECT id FROM FS_SESSIONS WHERE status = 'ENDED')
  AND status = 'PENDING';
```

**Side effects:** Phát Kafka event `flash_sale.session_started` và `flash_sale.session_ended` → Notification Service push thông báo cho user đã đăng ký reminder.

---

### JOB-02 · Flash Sale Reminder Dispatcher

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Gửi thông báo nhắc nhở cho Buyer đã đăng ký trước khi Flash Sale session bắt đầu 15 phút. |
| **Cron** | `0 * * * * *` (mỗi 1 phút) |
| **ShedLock name** | `flash-sale-reminder-dispatcher` |
| **Lock duration** | 55 giây |
| **Bảng tác động** | `FS_REMINDERS`, `FS_SESSIONS` |

**Logic:**
```sql
-- Tìm session sắp bắt đầu trong 15 phút tới
SELECT r.user_id, r.session_id
FROM FS_REMINDERS r
JOIN FS_SESSIONS s ON r.session_id = s.id
WHERE s.status = 'UPCOMING'
  AND s.start_time BETWEEN NOW() AND NOW() + INTERVAL '15 minutes';
-- Publish notification event cho từng user_id tìm được
-- Đánh dấu đã gửi (hoặc delete record FS_REMINDERS)
```

**Retention:** Sau khi session `ENDED`, toàn bộ `FS_REMINDERS` của session đó bị xóa (xem JOB-08).

---

### JOB-03 · Loyalty Points Expiry

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Tìm các điểm thưởng đã hết hạn, trừ ra khỏi `available_points` và tạo transaction ghi nhận. |
| **Cron** | `0 0 2 * * *` (02:00 hàng ngày) |
| **ShedLock name** | `loyalty-points-expiry` |
| **Lock duration** | 10 phút |
| **Bảng tác động** | `POINT_TRANSACTIONS`, `LOYALTY_ACCOUNTS` |

**Policy:** Điểm thưởng từ 1 đơn hàng hết hạn sau **365 ngày** kể từ `expires_at` (set lúc tạo POINT_TRANSACTION).

**Logic:**
```sql
-- Tìm điểm EARNED/CONFIRMED chưa hết hạn chưa bị xử lý
SELECT id, user_id, delta FROM POINT_TRANSACTIONS
WHERE type = 'EARNED'
  AND status = 'CONFIRMED'
  AND expires_at <= NOW()
  AND delta > 0
LIMIT 500;

-- Với mỗi bản ghi:
-- 1. Tạo POINT_TRANSACTIONS mới: type='EXPIRED', delta = -original_delta
-- 2. UPDATE LOYALTY_ACCOUNTS: available_points -= original_delta, expired_points += original_delta
-- 3. Dùng Optimistic Locking (version field) để tránh race condition
```

**Retention cho POINT_TRANSACTIONS:** Lưu vĩnh viễn (audit trail tài chính). Không xóa.

---

### JOB-04 · Outbox Event Publisher

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Đọc các event `PENDING` trong `OUTBOX_EVENTS`, publish lên Kafka, cập nhật status. |
| **Cron** | `0/10 * * * * *` (mỗi 10 giây) |
| **ShedLock name** | `outbox-event-publisher-{serviceName}` (mỗi service có lock riêng) |
| **Lock duration** | 9 giây |
| **Bảng tác động** | `OUTBOX_EVENTS` (mỗi service DB) |

**Logic:**
```sql
SELECT id, topic, payload FROM OUTBOX_EVENTS
WHERE status = 'PENDING'
ORDER BY created_at ASC
LIMIT 100;

-- Publish từng event lên Kafka
-- Nếu thành công: status = 'PROCESSED', processed_at = NOW()
-- Nếu thất bại: retry_count++; nếu retry_count >= 5 → status = 'FAILED', insert vào FAILED_EVENTS
```

**Cleanup OUTBOX_EVENTS:**
- `status = 'PROCESSED'` → xóa sau **7 ngày** (JOB-05).
- `status = 'FAILED'` → đã chuyển sang `FAILED_EVENTS`, xóa sau **3 ngày**.

---

### JOB-05 · Outbox Events Cleanup

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Xóa các OUTBOX_EVENTS đã xử lý xong để tránh bảng phình to. |
| **Cron** | `0 0 3 * * *` (03:00 hàng ngày) |
| **ShedLock name** | `outbox-cleanup` |
| **Lock duration** | 5 phút |
| **Bảng tác động** | `OUTBOX_EVENTS` |

```sql
DELETE FROM OUTBOX_EVENTS
WHERE status = 'PROCESSED'
  AND processed_at < NOW() - INTERVAL '7 days';

DELETE FROM OUTBOX_EVENTS
WHERE status = 'FAILED'
  AND created_at < NOW() - INTERVAL '3 days';
```

---

### JOB-06 · Failed Events Cleanup

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Xóa các FAILED_EVENTS đã được resolve hoặc đã quá lâu ở trạng thái DEAD. |
| **Cron** | `0 0 3 30 * ?` (03:00 ngày 30 hàng tháng) |
| **ShedLock name** | `failed-events-cleanup` |
| **Lock duration** | 10 phút |
| **Bảng tác động** | `FAILED_EVENTS` |

**Policy:**

| Status | Retention | Hành động |
|---|---|---|
| `RESOLVED` | 30 ngày sau khi resolve | Hard delete |
| `DEAD` | 90 ngày | Hard delete (sau khi đã manual review) |
| `MANUAL_INTERVENTION` | Không tự xóa | Admin phải đổi status trước |
| `PENDING` | Không tự xóa | Vẫn đang trong hàng chờ retry |

```sql
DELETE FROM FAILED_EVENTS
WHERE status = 'RESOLVED'
  AND updated_at < NOW() - INTERVAL '30 days';

DELETE FROM FAILED_EVENTS
WHERE status = 'DEAD'
  AND updated_at < NOW() - INTERVAL '90 days';
```

---

### JOB-07 · Stale Cart Cleanup

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Xóa giỏ hàng không hoạt động quá lâu. |
| **Cron** | `0 0 4 * * *` (04:00 hàng ngày) |
| **ShedLock name** | `stale-cart-cleanup` |
| **Lock duration** | 10 phút |
| **Bảng tác động** | `MG_CARTS`, `MG_CART_ITEMS` (MongoDB) |
| **Thực thi bởi** | Worker Service gọi Product/Cart Service nội bộ qua HTTP, hoặc Cart Service tự có TTL index |

**Policy:**

| Điều kiện | Retention |
|---|---|
| Cart của user không đăng nhập (guest) | Không hỗ trợ (hệ thống yêu cầu JWT) |
| Cart của user đã đăng nhập, `updated_at` cũ hơn 90 ngày | Xóa toàn bộ cart items |
| Cart item có `fs_item_id` (flash sale) mà session đã `ENDED` | Xóa item ngay (không cần chờ 90 ngày) |

```javascript
// MongoDB TTL hoặc Aggregate
db.mg_cart_items.deleteMany({
  fs_item_id: { $ne: null },
  // session đã ENDED — cần lookup qua FS_ITEMS
});

db.mg_carts.find({ updated_at: { $lt: new Date(Date.now() - 90*24*60*60*1000) } })
  .forEach(cart => {
    db.mg_cart_items.deleteMany({ cart_id: cart._id });
    db.mg_carts.deleteOne({ _id: cart._id });
  });
```

---

### JOB-08 · Flash Sale Data Cleanup

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Dọn dẹp dữ liệu Flash Sale sau khi session kết thúc. |
| **Cron** | `0 0 2 * * *` (02:00 hàng ngày) |
| **ShedLock name** | `flash-sale-cleanup` |
| **Lock duration** | 5 phút |
| **Bảng tác động** | `FS_REMINDERS`, `FS_SESSIONS`, `FS_ITEMS` |

**Policy:**

| Bảng | Điều kiện | Retention | Hành động |
|---|---|---|---|
| `FS_REMINDERS` | Session đã `ENDED` | 0 ngày (xóa ngay sau khi session kết thúc) | Hard delete |
| `FS_ITEMS` | Session `ENDED` + status `CANCELLED/REJECTED` | 30 ngày | Hard delete |
| `FS_ITEMS` | Session `ENDED` + status `APPROVED` | 180 ngày (giữ để báo cáo doanh thu) | Hard delete |
| `FS_SESSIONS` | Status `ENDED` | 365 ngày | Hard delete |

```sql
-- Xóa reminders của session đã ended
DELETE FROM FS_REMINDERS
WHERE session_id IN (SELECT id FROM FS_SESSIONS WHERE status = 'ENDED');

-- Xóa items của session cũ
DELETE FROM FS_ITEMS
WHERE status IN ('CANCELLED', 'REJECTED')
  AND updated_at < NOW() - INTERVAL '30 days';

DELETE FROM FS_ITEMS
WHERE status = 'APPROVED'
  AND session_id IN (
    SELECT id FROM FS_SESSIONS
    WHERE status = 'ENDED' AND end_time < NOW() - INTERVAL '180 days'
  );

-- Xóa session cũ (chỉ khi không còn FS_ITEMS nào)
DELETE FROM FS_SESSIONS
WHERE status = 'ENDED'
  AND end_time < NOW() - INTERVAL '365 days'
  AND id NOT IN (SELECT DISTINCT session_id FROM FS_ITEMS);
```

---

### JOB-09 · Notification Cleanup (MongoDB TTL)

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Tự động xóa thông báo cũ. **Không dùng cronjob** — dùng MongoDB TTL Index. |
| **Loại** | MongoDB TTL Index (không phải Quartz job) |
| **Bảng tác động** | `MG_NOTIFICATIONS` |

```javascript
// Index này được tạo 1 lần khi khởi tạo Notification Service
db.mg_notifications.createIndex(
  { "created_at": 1 },
  { expireAfterSeconds: 90 * 24 * 60 * 60 }  // 90 ngày
);
```

**Policy:** Thông báo tự xóa sau **90 ngày** kể từ `created_at`. MongoDB TTL background thread chạy mỗi 60 giây.

---

### JOB-10 · Soft-Deleted Products Hard Cleanup

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Xóa vĩnh viễn sản phẩm đã soft delete. |
| **Cron** | `0 0 3 * * 0` (03:00 Chủ nhật hàng tuần) |
| **ShedLock name** | `soft-deleted-products-cleanup` |
| **Lock duration** | 30 phút |
| **Bảng tác động** | `MG_PRODUCTS`, `MG_PRODUCT_VARIANTS`, `MG_INVENTORIES`, `ES_PRODUCTS_INDEX` |

**Policy:** Sản phẩm có `deleted_at` cũ hơn **30 ngày** mới được hard delete.

**Điều kiện bắt buộc trước khi xóa:**
- Không còn `ORDERS` nào ở trạng thái `PENDING | PAID | SHIPPING` có chứa SKU của sản phẩm.
- `MG_INVENTORIES.stock_locked = 0` (không có đơn hàng đang giữ tồn kho).

```javascript
const cutoff = new Date(Date.now() - 30 * 24 * 60 * 60 * 1000);
const products = db.mg_products.find({
  deleted_at: { $ne: null, $lt: cutoff }
});

products.forEach(p => {
  // Kiểm tra không còn active orders → nếu có thì skip
  const skus = db.mg_product_variants.find({ product_id: p._id }).map(v => v.sku_code);
  const lockedInventory = db.mg_inventories.findOne({
    sku_code: { $in: skus }, stock_locked: { $gt: 0 }
  });
  if (lockedInventory) return; // Skip sản phẩm này

  db.mg_product_variants.deleteMany({ product_id: p._id });
  db.mg_inventories.deleteMany({ product_id: p._id });
  db.mg_products.deleteOne({ _id: p._id });
  // Gọi Search Service API để xóa khỏi Elasticsearch index
});
```

---

### JOB-11 · Trust Score Log Cleanup

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Archive trust score logs quá cũ. |
| **Cron** | `0 0 4 1 * *` (04:00 ngày 1 hàng tháng) |
| **ShedLock name** | `trust-score-log-cleanup` |
| **Lock duration** | 5 phút |
| **Bảng tác động** | `TRUST_SCORE_LOGS` |

**Policy:** Log cũ hơn **2 năm** có thể xóa. Trước khi xóa, log aggregate (tổng delta theo tháng) được lưu vào file CSV/S3 nếu cần audit.

```sql
DELETE FROM TRUST_SCORE_LOGS
WHERE created_at < NOW() - INTERVAL '2 years';
```

---

### JOB-12 · ShedLock Stale Entry Cleanup

| Thuộc tính | Giá trị |
|---|---|
| **Mô tả** | Xóa các ShedLock entry bị stale (node crash mà không release lock). |
| **Cron** | `0 0 5 * * *` (05:00 hàng ngày) |
| **ShedLock name** | Không dùng ShedLock cho job này (tránh deadlock) |
| **Bảng tác động** | `SHEDLOCK` |

```sql
-- Xóa lock đã hết thời hạn hơn 1 giờ (node crash, không tự release)
DELETE FROM SHEDLOCK
WHERE lock_until < NOW() - INTERVAL '1 hour';
```

---

## 3. POLICY TỔNG HỢP THEO BẢNG

### PostgreSQL

| Bảng | Retention | Hard Delete? | Ghi chú |
|---|---|---|---|
| `USERS` | Vĩnh viễn | Không | Chỉ LOCKED, không xóa |
| `ROLES` | Vĩnh viễn | Không | |
| `ADDRESSES` | Cho đến khi user tự xóa | Có (theo yêu cầu) | |
| `TRUST_SCORE_LOGS` | 2 năm | Có (JOB-11) | |
| `LOYALTY_ACCOUNTS` | Vĩnh viễn | Không | Tài chính |
| `POINT_TRANSACTIONS` | Vĩnh viễn | Không | Audit trail |
| `PARENT_ORDERS` | Vĩnh viễn | Không | Tài chính |
| `ORDERS` | Vĩnh viễn | Không | Tài chính |
| `ORDER_ITEMS` | Vĩnh viễn | Không | Tài chính |
| `TRANSACTIONS` | Vĩnh viễn | Không | Tài chính, pháp lý |
| `REFUNDS` | Vĩnh viễn | Không | Tài chính, pháp lý |
| `REFUND_ITEMS` | Vĩnh viễn | Không | |
| `SELLER_STRIPE_ACCOUNTS` | Vĩnh viễn | Không | |
| `FS_SESSIONS` | 365 ngày sau ENDED | Có (JOB-08) | |
| `FS_ITEMS` | 30–180 ngày sau session ENDED | Có (JOB-08) | |
| `FS_REMINDERS` | 0 ngày sau session ENDED | Có (JOB-08) | |
| `OUTBOX_EVENTS` | 7 ngày (PROCESSED), 3 ngày (FAILED) | Có (JOB-05) | |
| `FAILED_EVENTS` | 30 ngày (RESOLVED), 90 ngày (DEAD) | Có (JOB-06) | |
| `SHEDLOCK` | Stale > 1 giờ | Có (JOB-12) | |

### MongoDB

| Collection | Retention | Cơ chế |
|---|---|---|
| `MG_PRODUCTS` | 30 ngày sau soft delete | JOB-10 |
| `MG_PRODUCT_VARIANTS` | Theo product | JOB-10 |
| `MG_INVENTORIES` | Theo product | JOB-10 |
| `MG_CARTS` | 90 ngày inactive | JOB-07 |
| `MG_CART_ITEMS` | 90 ngày inactive / ngay khi FS session ENDED | JOB-07 |
| `MG_NOTIFICATIONS` | 90 ngày | MongoDB TTL Index |

### Elasticsearch

| Index | Retention | Cơ chế |
|---|---|---|
| `ES_PRODUCTS_INDEX` | Sync với MongoDB `MG_PRODUCTS` | Xóa khi sản phẩm hard delete (JOB-10) |

---

## 4. CHECKLIST TRIỂN KHAI

- [ ] Tạo bảng `SHEDLOCK` trong PostgreSQL DB của Worker Service khi khởi tạo
- [ ] Cấu hình `spring.quartz.job-store-type=jdbc` để Quartz persist jobs
- [ ] Tạo MongoDB TTL index cho `MG_NOTIFICATIONS.created_at` khi Notification Service start
- [ ] Đặt alert khi `FAILED_EVENTS` có bản ghi `PENDING` tồn tại > 24 giờ
- [ ] Đặt alert khi `OUTBOX_EVENTS` có bản ghi `PENDING` tồn tại > 30 phút
- [ ] Test ShedLock bằng cách kill node giữa chừng, verify không có double execution
- [ ] Verify tất cả DELETE statement dùng `LIMIT` để tránh lock table toàn bộ
