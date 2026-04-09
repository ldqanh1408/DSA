# Marketplace — Mô Tả Nghiệp Vụ Chi Tiết Từng Actor & Schema Ownership

> Phiên bản: v1.0 | Căn cứ: ERD v4 + API Spec v4.1 + Spring Boot Library Config

---

## 1. TỔNG QUAN HỆ THỐNG

Hệ thống là một **Multi-Vendor Marketplace** — nơi nhiều Seller có thể mở gian hàng và bán sản phẩm cho Buyer trên cùng một nền tảng, thanh toán qua Stripe Connect, với Admin là người điều hành nền tảng.

**Kết luận MVP:** Database và API đã **đầy đủ và hoàn thiện** cho phạm vi MVP. Không có gap nghiệp vụ nào bị bỏ sót. Tất cả luồng từ đăng ký → mua hàng → thanh toán → hoàn tiền đã được cover. Hệ thống sẵn sàng để bắt đầu implement.

---

## 2. ACTOR: BUYER (Người Mua)

### 2.1. Định nghĩa
Là người dùng đã đăng ký tài khoản và có role `BUYER`. Đây là role mặc định được gán tự động khi đăng ký. Một user có thể đồng thời là cả BUYER lẫn SELLER.

### 2.2. Luồng nghiệp vụ chi tiết

#### A. Đăng ký & Quản lý tài khoản
- Đăng ký bằng `username + email + phone + password + full_name`. Hệ thống tự động tạo `LOYALTY_ACCOUNT` (1-1) và gán role `BUYER`.
- Đăng nhập bằng `username | email | phone` và nhận cặp `access_token (15 phút) + refresh_token`. Token dùng RS256.
- Xem/cập nhật thông tin cá nhân: `full_name`, `avatar_url` (upload lên MinIO trước), `phone` (cần xác minh OTP).
- Quản lý sổ địa chỉ: thêm/sửa/xóa địa chỉ giao hàng, đặt 1 địa chỉ làm mặc định (`is_default = true`) để hỗ trợ Fast Checkout.

#### B. Khám phá & Tìm kiếm sản phẩm
- Truy cập trang chủ và duyệt danh mục (category tree).
- Tìm kiếm full-text qua Elasticsearch: tìm theo tên, mô tả, thuộc tính động, lọc theo `price_min/max`, `category`, `seller`, `is_flash`.
- Xem chi tiết sản phẩm: ảnh, mô tả, variants (size/màu sắc/...), giá, tồn kho.

#### C. Giỏ hàng
- Thêm variant vào giỏ hàng (MongoDB, đọc/ghi rất nhanh). Mỗi `MG_CART_ITEM` lưu `price_snapshot` tại thời điểm thêm — giá này **không tự thay đổi** khi Seller cập nhật giá sau này.
- Nếu item thuộc Flash Sale, `fs_item_id` được gắn vào cart item.
- Cập nhật số lượng, xóa item khỏi giỏ.
- Giỏ hàng tồn tại liên tục theo `user_id` (1 user — 1 giỏ).

#### D. Đặt hàng (Checkout)
- Buyer chọn items trong giỏ, chọn địa chỉ giao hàng (hoặc dùng địa chỉ mặc định), chọn có dùng điểm thưởng hay không.
- Hệ thống tạo `PARENT_ORDER` (đơn tổng) và tự động tách thành nhiều `ORDERS` (đơn con) theo từng Seller. Ví dụ: giỏ hàng có sản phẩm của 3 Seller → 1 PARENT_ORDER + 3 ORDERS.
- `shipping_address` được snapshot vào từng ORDER (JSONB) — đảm bảo địa chỉ không thay đổi dù Buyer cập nhật sổ địa chỉ sau này.

#### E. Thanh toán
- Buyer thanh toán toàn bộ `PARENT_ORDER` qua Stripe (hoặc VNPay).
- Nếu dùng điểm thưởng: `PARENT_ORDER.loyalty_discount` được trừ vào `final_amt`.
- Stripe WebHook xác nhận thanh toán thành công → `TRANSACTIONS.status = SUCCESS` → phát sự kiện `payment.success` → Order Service cập nhật tất cả ORDERS trong parent sang `PAID`.

#### F. Theo dõi đơn hàng
- Xem danh sách tất cả đơn hàng của mình, lọc theo `status`.
- Xem chi tiết 1 đơn: `ORDER_ITEMS`, giá snapshot, địa chỉ giao, `tracking_number`.
- Nhận thông báo real-time qua SSE khi trạng thái đơn thay đổi.

#### G. Hoàn tiền (Refund)
Buyer được phép yêu cầu hoàn tiền trong các trường hợp: hàng lỗi, không đúng mô tả, không nhận được hàng, v.v.

- **Hoàn toàn phần (Partial Refund):** Chọn từng item trong 1 sub-order, nhập số lượng cần hoàn, lý do, upload ảnh bằng chứng.
- **Hoàn toàn bộ đơn tổng (Full Refund):** Chọn items từ nhiều sub-order/seller trong cùng 1 PARENT_ORDER. Hệ thống tự tạo cùng `group_ref` UUID, tách thành nhiều REFUND records (1 per seller), xử lý song song.
- Theo dõi trạng thái từng yêu cầu hoàn tiền (PENDING → SUCCESS | REJECTED | FAILED).

#### H. Điểm thưởng (Loyalty)
- Xem số dư điểm (`available_points`), lịch sử giao dịch điểm.
- Điểm được tích lũy tự động khi đơn hàng chuyển sang `DELIVERED` (sự kiện do Order Service phát).
- Điểm có ngày hết hạn (`expires_at` trong `POINT_TRANSACTIONS`).
- Ước tính điểm sẽ nhận hoặc có thể dùng trước khi thanh toán.

#### I. Flash Sale
- Xem danh sách session đang `ACTIVE` / `UPCOMING`.
- Đăng ký nhắc nhở trước khi session bắt đầu (`FS_REMINDERS`).
- Mua flash sale (endpoint chịu tải cao — WebFlux + Redis Lua Script): hệ thống kiểm tra `flash_stock` và `limit_per_user` nguyên tử trên Redis. Trả về `409 SOLD_OUT` hoặc `400 LIMIT_EXCEEDED` ngay lập tức nếu không thỏa điều kiện.

---

## 3. ACTOR: SELLER (Người Bán)

### 3.1. Định nghĩa
Là người dùng có role `SELLER`. Role này được thêm vào bảng `ROLES` — không xóa role BUYER đang có. Seller phải hoàn tất onboarding Stripe mới có thể nhận tiền.

### 3.2. Luồng nghiệp vụ chi tiết

#### A. Đăng ký làm Seller
- Seller tự upgrade role từ màn hình profile (thêm record `SELLER` vào `ROLES`).
- Bắt đầu onboarding Stripe Connect (Express Account): hệ thống tạo `acct_xxx` và trả về `onboarding_url` (Account Link URL của Stripe). Seller điền thông tin định danh (KYC) và gắn tài khoản ngân hàng trực tiếp trên Stripe.
- Thông tin onboarding được lưu vào `SELLER_STRIPE_ACCOUNTS`: `account_status`, `charges_enabled`, `payouts_enabled`, `details_submitted`.
- Stripe Webhook `account.updated` tự động sync trạng thái về.

#### B. Quản lý sản phẩm
- **Tạo sản phẩm:** Tạo product với tên, mô tả, danh mục, ảnh (upload MinIO), attributes động (size/màu/...), và ít nhất 1 variant.
  - Mỗi variant có `sku_code` (unique), `tier_name`, `price`.
  - Mỗi variant tự động có 1 bản ghi `MG_INVENTORIES` với `stock_total = 0`.
- Sản phẩm mới tạo có `status = PENDING`, chờ Admin duyệt.
- **Sửa sản phẩm:** Cập nhật thông tin, ảnh, thuộc tính. Nếu cần duyệt lại thì status reset về PENDING.
- **Xóa sản phẩm:** Soft delete (`deleted_at`). Sản phẩm không còn hiển thị, nhưng dữ liệu lịch sử đơn hàng vẫn toàn vẹn.
- **Đăng ký Flash Sale:** Submit sản phẩm (SKU cụ thể) vào session flash sale với `flash_price` và `flash_stock`. Chờ Admin duyệt (`FS_ITEMS.status = PENDING → APPROVED`).

#### C. Quản lý tồn kho
- Điều chỉnh tồn kho theo từng SKU (`delta` dương/âm, kèm lý do). Hệ thống validate không cho `stock_available` xuống âm.
- Xem lịch sử nhập/xuất/điều chỉnh theo SKU (audit log).
- Tồn kho bị trừ (lock) khi Buyer đặt hàng → chỉ release chính thức khi đơn `DELIVERED` hoặc `CANCELLED`.

#### D. Quản lý đơn hàng
- Xem danh sách các `ORDERS` mà mình là `seller_id`, lọc theo status.
- Cập nhật trạng thái đơn: `PAID → SHIPPING` (gắn `tracking_number`) → `DELIVERED`.
- Xem chi tiết từng đơn, bao gồm `ORDER_ITEMS`, địa chỉ giao hàng snapshot.

#### E. Nhận tiền (Payout)
- **Đơn 1 Seller:** Stripe dùng Destination Charges — tự động split tại thời điểm charge: `final_amt - application_fee_amount` vào tài khoản Seller, `application_fee_amount` về Platform.
- **Đơn nhiều Seller:** Platform thu toàn bộ tiền trước, sau đó Payment Service gọi Transfer API để chuyển tiền cho từng Seller tương ứng. `stripe_transfer_id` được lưu vào `TRANSACTIONS`.
- Seller rút tiền về tài khoản ngân hàng trực tiếp qua dashboard Stripe (ngoài phạm vi hệ thống).

#### F. Xử lý hoàn tiền
- Nhận thông báo khi có yêu cầu hoàn tiền liên quan đến đơn của mình.
- Khi Admin duyệt refund, Stripe tự động reverse transfer từ tài khoản Seller về Platform trước khi refund về Buyer.
- Trust score bị trừ 5 điểm mỗi khi Admin phải can thiệp duyệt refund thủ công.

---

## 4. ACTOR: ADMIN (Quản Trị Viên)

### 4.1. Định nghĩa
Là người dùng có role `ADMIN`. Có quyền cao nhất trong hệ thống, không giới hạn bởi ownership. Admin không thể tự đăng ký role này — phải được cấp trực tiếp trong DB.

### 4.2. Luồng nghiệp vụ chi tiết

#### A. Quản lý người dùng
- Xem danh sách tất cả user, tìm kiếm/lọc.
- **Khóa tài khoản (`LOCK`):** User bị khóa không thể đăng nhập (trả về `403`). Dùng khi vi phạm chính sách.
- **Mở khóa (`UNLOCK`):** Khôi phục quyền truy cập.
- **Điều chỉnh Trust Score:** Tăng/giảm `USERS.trust_score` với lý do, lưu log vào `TRUST_SCORE_LOGS`. Trust score ảnh hưởng đến ưu tiên hiển thị sản phẩm của Seller và khả năng tham gia Flash Sale.
- Xem lịch sử thay đổi trust score của từng user.

#### B. Duyệt sản phẩm
- Xem danh sách sản phẩm `status = PENDING`.
- **Duyệt sản phẩm:** `status → APPROVED` + phát sự kiện `product.approved` → Search Service index sản phẩm vào Elasticsearch.
- **Từ chối sản phẩm:** `status → REJECTED` + ghi `reject_reason` + thông báo cho Seller.

#### C. Quản lý danh mục
- CRUD danh mục (tree structure, có `parent_id` và `level`).
- Không thể xóa danh mục đang có product con hoặc sub-category (`409`).

#### D. Quản lý Flash Sale
- Tạo session flash sale với `name`, `start_time`, `end_time`.
- Duyệt/từ chối từng `FS_ITEMS` mà Seller submit vào session.
- Hệ thống Worker Service (Quartz Scheduler + ShedLock) tự động chuyển `FS_SESSIONS.status`: `UPCOMING → ACTIVE → ENDED` đúng giờ.

#### E. Xử lý hoàn tiền (Adjudication)
Admin đóng vai trò trọng tài cuối cùng trong tranh chấp:
- Xem tất cả REFUND requests, lọc theo `status`, `type`, `seller_id`, `group_ref`, date range.
- **Duyệt hoàn tiền:** Gọi Stripe Refund API (`refunds.create`), có thể override `adjust_amount`. Ghi `admin_note` và `reviewed_by`. Tự động trừ trust score Seller 5 điểm. Phát `refund.admin_approved` event.
- **Từ chối hoàn tiền:** Ghi `reject_reason`, phát `refund.rejected` event, thông báo cho Buyer.

#### F. Giám sát hệ thống
- **Failed Events:** Xem danh sách các event/task bị lỗi (`FAILED_EVENTS`), retry thủ công. Dùng khi Kafka consumer gặp exception hoặc external API call thất bại sau N lần retry.
- Xem `OUTBOX_EVENTS` trạng thái để kiểm tra tắc nghẽn event pipeline.

---

## 5. ACTOR: SYSTEM (Hệ Thống / Cronjob)

Đây không phải human actor, nhưng cần mô tả vì ảnh hưởng trực tiếp đến dữ liệu.

- **Worker Service (Quartz + ShedLock):** Tự động activate/expire flash sale sessions, expire điểm thưởng (`POINT_TRANSACTIONS`), cleanup outbox events đã xử lý, v.v. (xem chi tiết trong Data Retention Policy).
- **Stripe Webhook Processor:** Nhận event từ Stripe, cập nhật trạng thái TRANSACTIONS, REFUNDS, SELLER_STRIPE_ACCOUNTS.
- **Kafka Consumers:** Order Service lắng nghe `flash_sale.item_sold`, Payment Service lắng nghe `refund.requested`, Loyalty Service lắng nghe `order.delivered`, v.v.
- **Outbox Pattern:** Mọi sự kiện được ghi vào `OUTBOX_EVENTS` trong cùng transaction DB trước khi publish lên Kafka — đảm bảo at-least-once delivery.

---

## 6. SCHEMA OWNERSHIP — THUỘC SERVICE NÀO

### 6.1. PostgreSQL Schemas

| Schema / Bảng | Owned By | Ghi chú |
|---|---|---|
| `USERS` | **Identity Service** | Source of truth cho user |
| `ROLES` | **Identity Service** | |
| `ADDRESSES` | **Identity Service** | |
| `TRUST_SCORE_LOGS` | **Identity Service** | Admin ghi qua Identity API |
| `SELLER_STRIPE_ACCOUNTS` | **Payment Service** | Stripe onboarding data |
| `TRANSACTIONS` | **Payment Service** | Payment intent records |
| `REFUNDS` | **Payment Service** (ghi) / Order Service (đọc) | Payment Service là writer duy nhất |
| `REFUND_ITEMS` | **Payment Service** | |
| `PARENT_ORDERS` | **Order Service** | |
| `ORDERS` | **Order Service** | |
| `ORDER_ITEMS` | **Order Service** | |
| `LOYALTY_ACCOUNTS` | **Order Service** (đọc amount) / **Loyalty Service** (ghi) | Loyalty Service là writer |
| `POINT_TRANSACTIONS` | **Loyalty Service** | |
| `FS_SESSIONS` | **Flash Sale Service** | |
| `FS_ITEMS` | **Flash Sale Service** | |
| `FS_REMINDERS` | **Flash Sale Service** | |
| `OUTBOX_EVENTS` | **Mỗi service tự có bảng riêng** | Không share chung — mỗi DB có 1 bảng OUTBOX_EVENTS của chính nó |
| `FAILED_EVENTS` | **Worker Service** | Tập trung các event dead-letter từ tất cả service |
| `SHEDLOCK` | **Worker Service** | Distributed lock table |

> ⚠️ **Lưu ý quan trọng:** Trong microservices, mỗi service có **database riêng**. Các bảng liệt kê trên không nằm chung 1 PostgreSQL instance — chúng được phân tách theo service. Kết nối cross-service qua Kafka events hoặc gRPC calls, **không bao giờ** JOIN cross-DB trực tiếp.

### 6.2. MongoDB Collections

| Collection | Owned By | Ghi chú |
|---|---|---|
| `MG_CATEGORIES` | **Product Service** | Admin CRUD qua Product Service API |
| `MG_PRODUCTS` | **Product Service** | |
| `MG_PRODUCT_VARIANTS` | **Product Service** | |
| `MG_INVENTORIES` | **Product Service** | Stock management |
| `MG_CARTS` | **Cart Service** | Đã chuyển sang MongoDB |
| `MG_CART_ITEMS` | **Cart Service** | |
| `MG_NOTIFICATIONS` | **Notification Service** | TTL index 90 ngày |

### 6.3. Elasticsearch Index

| Index | Owned By | Ghi chú |
|---|---|---|
| `ES_PRODUCTS_INDEX` | **Search Service** | Consumer của `product.approved` event từ Product Service. Search Service là reader/writer duy nhất của index này. |

### 6.4. Redis Keys (Không có bảng, nhưng cần rõ ownership)

| Key Pattern | Owned By | Mục đích |
|---|---|---|
| `flash:stock:{sessionId}:{skuCode}` | **Flash Sale Service** | Atomic stock counter |
| `flash:user_bought:{sessionId}:{userId}` | **Flash Sale Service** | Per-user limit tracking |
| `rate_limit:{userId}:{endpoint}` | **API Gateway** | Rate limiting |
| `notif:channel:{userId}` | **Notification Service** | Pub/Sub channel |

---

## 7. SERVICE MAP — PORT & CÔNG NGHỆ

| Service | Port | DB | Pattern | Ghi chú |
|---|---|---|---|---|
| API Gateway | :8080 | Redis | WebFlux | JWT validation, Rate Limiting |
| Identity Service | :8081 | PostgreSQL | Servlet + CQRS/Axon | Auth, User, Address |
| Product Service | :8082 | MongoDB | Servlet + CQRS/Axon | gRPC Server |
| Cart Service | :8083 | MongoDB | Servlet (Virtual Threads) | gRPC Client → Product |
| Order Service | :8087 | PostgreSQL | Servlet + CQRS/Axon (Saga) | Kafka Consumer |
| Payment Service | :8085 | PostgreSQL | Servlet + CQRS/Axon | gRPC Server, Stripe |
| Loyalty Service | :8084 | PostgreSQL | Servlet | Kafka Consumer |
| Flash Sale Service | :8086 | PostgreSQL + Redis | **WebFlux** | Lua Script, 50k req/s |
| Search Service | :8089 | Elasticsearch | Servlet | Kafka Consumer |
| Notification Service | :8088 | MongoDB | **WebFlux** | SSE, Redis Pub/Sub |
| Worker Service | — | PostgreSQL | Servlet (no web) | Quartz, ShedLock |
| Discovery Service | :8761 | — | — | Eureka Server |
