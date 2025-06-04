#include "../include/Console.h"
#include <algorithm>
#include <iomanip>

// Plane* Console::list_planes[MAX_PLANE] = {};
// Flight* Console::list = nullptr;
// Passenger* Console::input = nullptr;
// AVL_TREE Console::manager = AVL_TREE();

// namespace fs = std::filesystem;

unsigned int Console::count_flights() {
    int count = 0;
    for(Flight *i = Console::list; i != nullptr; i = i->next, ++count);
    return count;
}    
void Console::load_flights_before_view() {
    for(Flight *p = list; p != nullptr; p = p->next) {
        if(p->cur_status == status::available || p->cur_status == status::sold_out) {
            if(is_completed(&p->date_dep, &p->time_dep)) {
                p->cur_status = status::completed;
                
            }
            std::ofstream out("data/Flights/" + std::string(p->flight_id) + ".txt");
            out << *p;
        }
    }
}
void Console::load_flights_from_folder() {
    // 1) Đếm file .txt
    int count = 0;
    for (auto &entry : fs::directory_iterator("data/Flights")) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() == ".txt")
            ++count;
    }
    if (count == 0) {
        list = nullptr;
        return;
    }

    // 2) Thu thập tên file vào mảng động
    std::string *files = new std::string[count];
    int idx = 0;
    for (auto &entry : fs::directory_iterator("data/Flights")) {
        if (!entry.is_regular_file()) continue;
        auto p = entry.path();
        if (p.extension() == ".txt") {
            files[idx++] = p.filename().string();
            // Menu::gotoxy(0, 0);
            // std::cout << p.filename().string() << std::endl;
            // Sleep(2000);
        }
    }

    // 3) Bubble‑sort GIẢM dần
    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count - 1 - i; ++j) {
            if (files[j] < files[j + 1]) {
                std::swap(files[j], files[j + 1]);
            }
        }
    }

    // 4) Duyệt mảng đã GIẢM, mỗi lần chèn đầu → linked‑list sẽ TĂNG dần
    Flight *head = nullptr;
    for (int i = 0; i < count; ++i) {
        std::string path = "data/Flights/" + files[i];
        std::ifstream in(path);
        // if (!in) continue;

        Flight *f = new Flight();
        // f->plane_id = new char;
        // f->total_seats = new unsigned int;
        // if (!(in >> *f)) {  // cần ưcó operator>> đọc vào đúng format
        //     delete f;
        //     continue;
        // }
        in >> *f;
        in.close();
        // std::cout << *f << std::endl;
        // Sleep(2000);
        if(f->cur_status == status::cancelled || f->cur_status == status::completed) {
            for(int i = 0; i < *f->total_seats; ++i) {
                if(f->tickets[i].CMND != nullptr) {
                    Node *p = manager.search(manager.root, f->tickets[i].CMND);
                    p->data.number_of_tickets--;
                    if(p->data.number_of_tickets == 0) {
                        fs::remove("data/Passenger/" + std::string(f->tickets[i].CMND) + ".txt");
                        manager.root = manager.erase(manager.root, p->data);
                    }
                    std::ofstream out("data/Passenger/" + std::string(f->tickets[i].CMND) + ".txt");
                    out << p->data;
                }
            }
            fs::remove("data/Flights/" + std::string(f->flight_id) + ".txt");
            continue;
        }
        
        std::ifstream plane_in("data/Planes/" + std::string(f->plane_id) + ".txt"); // Renamed to plane_in
        int idx; plane_in >> idx;
        f->plane_id = list_planes[idx]->plane_id;
        f->total_seats = &list_planes[idx]->number_of_seats;
        if(is_completed(&f->date_dep, &f->time_dep)) {
            list_planes[idx]->number_flights_performed++;
            f->cur_status = status::completed;
            std::ofstream out("data/Flights/" + std::string(f->flight_id) + ".txt");
            out << *f;
            std::ofstream plane_out("data/Planes/" + std::string(f->plane_id) + ".txt");
            plane_out << idx << std::endl;
            plane_out << *list_planes[idx];
        }
        plane_in.close();


        for(int i = 0; i < *f->total_seats; ++i) {
            if(f->tickets[i].CMND != nullptr) {
                Node *p = manager.search(manager.root, f->tickets[i].CMND);
                f->tickets[i].CMND = p->data.CMND;
            }
        }

        // chèn vào đầu
        f->next = head;
        head = f;
    }
    
    delete[] files;
    list = head;

}
bool Console::is_completed(date_departure *x, time_departure *y) {
    tm t = {};
    t.tm_year = x->year - 1900;
    t.tm_mon = x->month - 1;
    t.tm_mday = x->day;
    t.tm_hour = y->hour;
    t.tm_min = y->minute;
    t.tm_sec = 0;

    time_t scheduled_time = mktime(&t);
    time_t now = time(nullptr);
    return now >= scheduled_time; // true nếu thời gian hiện tại đã vượt quá lịch
}
void Console::load_passengers_from_folder() {

    for (auto &entry : fs::directory_iterator("data/Passenger")) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() == ".txt") {
            std::string path = entry.path().filename().string();
            std::ifstream in("data/Passenger/"+ path);
            Passenger data; in >> data;

            std::cout << data << std::endl;
            // Menu::gotoxy(0,0);
            // std::cout << path << std::endl;
            // Sleep(2000);
            manager.root = manager.insert(manager.root, data);
        }
    }
}
void Console::load_planes_from_folder() {
    const std::string folder = "data/Planes";
    int max_idx = -1;
    for (auto& entry : std::filesystem::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;

        std::ifstream in(entry.path());
        if (!in) continue;

        int idx;
        in >> idx;
        in.ignore();

        // int index = std::stoi(line); // chuyển string -> int

        // Plane* p = new Plane();
        list_planes[idx] = new Plane();
        in >> *list_planes[idx]; // phần còn lại là thông tin máy bay
        max_idx = std::max(max_idx, idx);
        // std::cout << "Đã nạp máy bay tại vị trí [" << index << "] từ file " << entry.path().filename() << "\n";
    }
    for(int i = MAX_PLANE - 1; i > max_idx; --i) {
        list_planes[i] = nullptr;
    }
}
void Console::processing() {
    load_passengers_from_folder();
    load_planes_from_folder();
    load_flights_from_folder();


}

void Console::start_program() {
    processing();
    int i = 0;
    char ch;
    Menu::display_login_frame();
    while(true) {
        Menu::gotoxy(25 + 29, 7 + i);
        // ch = _getch();
        if(i == 0) {
            Menu::gotoxy(25 + 29, 7);
            std::cout << ">>";
            Menu::gotoxy(25 + 29, 10);
            std::cout << "  ";
        } else {    
            Menu::gotoxy(25 + 29, 7);
            std::cout << "  ";
            Menu::gotoxy(25 + 29, 10);
            std::cout << ">>";
        }
        // std::cout << ">>";
        Menu::gotoxy(0, 0);
        ch = _getch();
        #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    ch = _getch(); // Lấy mã thực tế của phím
                }
        #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        ch = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif

        if(ch == UP && i > 0) --i;
        else if(ch == DOWN && i < 1) ++i; 
        else if(ch == ENTER) {
            if(i == 0) {
                enter_user_information();
                Menu::display_login_frame();
                continue;
            } else {
                enter_manager_menu();
                Menu::display_login_frame();
            }
        }
    }
}
void Console::enter_manager_menu() {
    int i = 0;
    char ch;
    Menu::display_manager_menu();
    while(true) {
        Menu::gotoxy(25 + 29, 7 + i);
        // ch = _getch();
        if(i == 0) {
            Menu::gotoxy(29, 6);
            std::cout << ">>";
            Menu::gotoxy(29, 9);
            std::cout << "  ";
            Menu::gotoxy(29, 12);
            std::cout << "  ";
        } else if (i == 1){    
            Menu::gotoxy(29, 6);
            std::cout << "  ";
            Menu::gotoxy(29, 9);
            std::cout << ">>";
            Menu::gotoxy(29, 12);
            std::cout << "  ";
        } else if(i == 2) {
            Menu::gotoxy(29, 6);
            std::cout << "  ";
            Menu::gotoxy(29, 9);
            std::cout << "  ";
            Menu::gotoxy(29, 12);
            std::cout << ">>";
        }
        // std::cout << ">>";
        Menu::gotoxy(0, 0);
        ch = _getch();
        #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    ch = _getch(); // Lấy mã thực tế của phím
                }
        #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        ch = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif

        if(ch == UP && i > 0) --i;
        else if(ch == DOWN && i < 2) ++i; 
        else if(ch == ENTER) {
            if(i == 0) {
                enter_plane_list();
                Menu::display_manager_menu();
            } else if(i == 1) {
                enter_available_flights(2); // 2 là manager
                Menu::display_manager_menu();
            } else if(i == 2) {
                enter_plane_statistics();
                Menu::display_manager_menu();
            }
        } else if(ch == ESC) return;
    }
}


void Console::enter_plane_statistics() {
    merge_sort();
    char ch;
    int number_of_planes = get_plane_count();
    //merge sort
    int cur_page = 0, max_page;
    if (number_of_planes == 0) max_page = 1;
    else if (number_of_planes % PLANES_PER_PAGE == 0){
        max_page = max_page = number_of_planes / PLANES_PER_PAGE;
    }
    else {
        max_page = (number_of_planes / PLANES_PER_PAGE) + 1;
    }
    int cur_row = 0;
    Menu::display_plane_statistics();
    while(true) {
        
        for(int i = 0; i < PLANES_PER_PAGE; ++i) {
            if(cur_row == i) {
                Menu::gotoxy(24, 7 + i);
                std::cout << ">>";
            }
            else {
                Menu::gotoxy(24, 7 + i);
                std::cout << "  ";
            }
            int cur_i = i + cur_page*PLANES_PER_PAGE;
            if(cur_i < number_of_planes) {
                Menu::gotoxy(27, 7 + i);
                std::cout << list_planes[cur_i]->plane_id;
                Menu::gotoxy(27 + 23, 7 + i);
                std::cout << list_planes[cur_i]->plane_type;
                Menu::gotoxy(27 + 23 + 43, 7 + i);
                std::cout << list_planes[cur_i]->number_flights_performed;
            } else {
                Menu::gotoxy(27, 7 + i);
                std::cout << "         "; // xóa plane_id
                Menu::gotoxy(27 + 23, 7 + i);
                std::cout << "                     "; // xóa plane_type
                Menu::gotoxy(27 + 23 + 43, 7 + i);
                std::cout << "         "; // xóa number_flights_performed
            }

        }
        Menu::display_list_instructions(cur_page + 1, max_page);
        Menu::gotoxy(0, 0);
        std::cout << "number_of_planes:" << number_of_planes;
        ch = _getch();
        #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    ch = _getch(); // Lấy mã thực tế của phím
                }
        #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        ch = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif
        
        if(ch == UP && cur_row > 0){
            cur_row--;
        }
        else if(ch == DOWN && cur_row + cur_page*PLANES_PER_PAGE < number_of_planes - 1){
            int remaining = std::min(PLANES_PER_PAGE, number_of_planes - cur_page * PLANES_PER_PAGE);
            if (cur_row < remaining - 1){
                ++cur_row;
            }
        }
        else if(ch == RIGHT && cur_page < max_page - 1){
            cur_row = 0;
            ++cur_page;
            // sang trang mới vẽ lại khung
            Menu::display_plane_statistics();
        }
        else if(ch == LEFT && cur_page > 0){
            cur_row = 0;
            --cur_page;
            // sang trang mới vẽ lại khung
            Menu::display_plane_statistics();
        } 
        else if(ch == ESC) return;
    }
    
}
void Console::enter_flight_manager_menu() {
    char ch;
    int i = 0;
    Menu::display_flight_manager_menu();
    while (true)
    {
        
        for(int j = 0; j < 3; ++j) {
            Menu::gotoxy(15 + 14, 6 + j*3);
            if(i == j) std::cout << ">>"; 
            else std::cout << "  ";                
        }
        Menu::gotoxy(0,0);
        ch = _getch();
        #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    ch = _getch(); // Lấy mã thực tế của phím
                }
        #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        ch = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif

        if(ch == TAB) {
            break;
        } else if(ch == UP && i > 0) --i;
        else if(ch == DOWN && i < 2) ++i;
        else if(ch == ENTER) {
            if(i == 1) enter_flight_id(1);
            else if(i == 2) enter_flight_id(2);
            else if(i == 0) enter_manage_flights();
            Menu::display_flight_manager_menu();
        } else if(ch == ESC) return;
    }   
}
void Console::enter_manage_flights() {
    char ch = '\0';
    int i = 0;
    Menu::display_manage_flights();
    while(true) {
        for(int j = 0; j < 3; ++j) {
            Menu::gotoxy(25 + 19, 6 + 3*j);
            if(j == i) std::cout << ">>";
            else std::cout << "  "; 
            
        }
        Menu::gotoxy(0, 0);
        ch = _getch();
        #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    ch = _getch(); // Lấy mã thực tế của phím
                }
        #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        ch = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif

        if(ch == UP && i > 0) --i;
        else if(ch == DOWN && i < 2) ++i;
        else if(ch == ESC) return;
        else if(ch == ENTER) {
            if(i == 0) enter_flight_information();
            else if(i == 1) enter_flight_update();
            else if(i == 2) enter_flight_cancel();
            Menu::display_manage_flights();
        }
    }
}

void Console::enter_flight_id(int choice) {
    char ch;
    Flight *choosing = nullptr;
    char flight_id[LEN_FLIGHT_ID] = "\0"; int i = 0;
    Menu::display_enter_flight_ID();
    while(true) {
        Menu::gotoxy(25 + 35, 6);
        // enter(flight_id, i, LEN_FLIGHT_ID, ch, [](char &c) {
        //     return true;
        // });

        // Sửa lại cho đúng định dạng flight_id
        enter(flight_id, i, LEN_FLIGHT_ID, ch,
            [&](char &c) {
                if (c >= 'a' && c <= 'z') c -= 32;
                return (c >= 'A' && c <= 'Z' || c >= '0' && c <= '9');
            });
        // ch = _getch();
        if(ch == ESC) return;
        else if(ch == ENTER) {
            //thieu dieu kien check flight id
            choosing = list;
            while(choosing) {
                if(strcmp(flight_id, choosing->flight_id) == 0) {
                    break;
                }
                choosing = choosing->next;
            }
            if(choosing == nullptr) {
                //in ra thoong bao
                Menu::display_flight_not_found();
                // gán flight_id về rỗng để nhập lại
                strcpy(flight_id, "/0");
                i = 0;
                choosing = nullptr;
                Menu::display_enter_flight_ID();
                continue;
            } 
            else {
                if(choice == 1) enter_passenger_list(choosing);
                else if(choice == 2)  enter_available_tickets(choosing, 2); // 2 là manager
                strcpy(flight_id, "/0");
                i = 0;
                choosing = nullptr;
                Menu::display_enter_flight_ID();
            }
        } 
    }
}   



void Console::enter_passenger_list(Flight *flight) {
    int n = 0;

    // Lấy danh sách chỉ số vé có người đặt (mảng được cấp phát động)
    int *seat_indices = flight->list_passengers(n);  
    if(n == 0) {
        Menu::display_flight_not_booked_by_user();
        return;
        //thông báo không có hành khách
    }
    int cur_row = 0, cur_page = 0;
    int max_page;
    if (n == 0) max_page = 1;
    else if (n % PASSENGERS_PER_PAGE == 0){
        max_page = n / PASSENGERS_PER_PAGE;
    }
    else {
        max_page = (n / PASSENGERS_PER_PAGE) + 1;
    }

    Menu::display_passenger_list();
    while (true) {
        int start_idx = cur_page * PASSENGERS_PER_PAGE;
        int end_idx = std::min(start_idx + PASSENGERS_PER_PAGE, n);
        
        
        // Hiển thị thông tin chuyến bay
        Menu::gotoxy(80, 3);
        std::cout << flight->flight_id;
        Menu::gotoxy(38, 5);
        std::cout << flight->plane_id;
        Menu::gotoxy(81, 5);
        std::cout << flight->destination;
        Menu::gotoxy(44, 7);
        std::cout << flight->date_dep; 
        Menu::gotoxy(74, 7);
        std::cout << flight->time_dep;
        Menu::gotoxy(36, 9);
        switch (flight->cur_status) {
            case status::cancelled:
            std::cout << "Cancelled";
            break;
            case status::available:
            std::cout << "Available";
            break;
            case status::sold_out:
            std::cout << "Sold Out";
            break;
            case status::completed:
            std::cout << "Completed";
            break;
        }
        
        // Hiển thị danh sách các hành khách trên trang hiện tại
        for (int i = start_idx; i < end_idx; i++) {
            int row = 13 + (i - start_idx);
            
            // In con trỏ đánh dấu dòng được chọn
            if ((i - start_idx) == cur_row) {
                Menu::gotoxy(22, row);
                std::cout << ">>";
            } else {
                Menu::gotoxy(22, row);
                std::cout << "  ";
            }
            
            // Sử dụng chỉ số từ mảng seat_indices để biết vé cụ thể trong flight->tickets
            int ticketIndex = seat_indices[i];
            Node *node = manager.search(manager.root, flight->tickets[ticketIndex].CMND);
            Passenger *passenger = (node ? &node->data : nullptr);
            Menu::gotoxy(24, row);
            // std::cout << std::string(90, ' ');
            // In Last Name
            Menu::gotoxy(24 + 3, row);
            std::cout << passenger->last_name;
            // In First Name
            Menu::gotoxy(45, row);
            std::cout << passenger->first_name;
            // In CMND
            Menu::gotoxy(56 + 8, row);
            std::cout << passenger->CMND;
            // In Gender
            Menu::gotoxy(72 + 11, row);
            std::cout << (*passenger->gender == 1 ? "Male" : "Female");
            // Bạn có thể in thêm Seat No. nếu cần, ví dụ:
            Menu::gotoxy(72 + 22, row);
            std::cout << ticketIndex + 1;
            
        }
        Menu::display_list_instructions(cur_page + 1, max_page);
        // thêm chỉ dẫn khi nhấn enter để huỷ vé đó
        std::cout << "\t\t\t\t[Enter] Cancel ticket";
        Menu::gotoxy(0, 0);
        char key = _getch();
        #ifdef _WIN32
        if (key == -32 || key == 224) { // Phím mũi tên trên Windows có mã tiền tố
            key = _getch(); // Lấy mã thực tế của phím
        }
        #else
                if (key == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        key = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif

        // Xử lý các phím điều hướng
        if (key == ESC) break;
        else if (key == LEFT && cur_page > 0){
            cur_page--;
            cur_row = 0;
            // sang trang mới vẽ lại khung
            Menu::display_passenger_list();
        }
        else if (key == RIGHT && cur_page < max_page - 1){
            cur_page++;
            cur_row = 0;
            // sang trang mới vẽ lại khung
            Menu::display_passenger_list();
        }
        else if (key == UP && cur_row > 0){
            cur_row--;
        }
        else if (key == DOWN){
            int remaining = std::min(PASSENGERS_PER_PAGE, n - cur_page * PASSENGERS_PER_PAGE);
            if (cur_row < remaining - 1){
                ++cur_row;
            }
        }
        // Trong hàm enter_passenger_list (Console.cpp)
        else if (key == ENTER) {
            int selected = start_idx + cur_row;
            int ticketIndex = seat_indices[selected]; // Sửa ở đây
            Node *test = manager.search(manager.root, flight->tickets[ticketIndex].CMND);
            if (test != nullptr) { // Kiểm tra test khác nullptr
                test->data.number_of_tickets--;
                std::ofstream out("data/Passenger/" + std::string(test->data.CMND) + ".txt");
                out << test->data;
                out.close();
                // Menu::gotoxy(0,0);
                // std::cout << test->data.number_of_tickets << std::endl;
                if (test->data.number_of_tickets == 0) {
                    std::filesystem::remove("data/Passenger/" + std::string(test->data.CMND) + ".txt");
                    manager.root = manager.erase(manager.root, test->data); // Cập nhật root
                }
                flight->tickets[ticketIndex].CMND = nullptr; // Sửa chỉ số ở đây
            }
            if(n == *flight->total_seats) {
                flight->cur_status = status::available;
            }
            std::ofstream out("data/Flights/" + std::string(flight->flight_id) + ".txt");
            out << *flight;

            n = 0;
            seat_indices = flight->list_passengers(n);  
            if(n == 0) {
                //thông báo không có hành khách
                Menu::display_flight_not_booked_by_user();
                return;
            }
            cur_row = 0, cur_page = 0;
            max_page = (n + PASSENGERS_PER_PAGE - 1) / PASSENGERS_PER_PAGE - 1;
            Menu::display_passenger_list();
            // break;
        } else if(key == TAB) {
            enter_delete_passenger(flight);
            if(n == *flight->total_seats) {
                flight->cur_status = status::available;
            }
            std::ofstream out("data/Flights/" + std::string(flight->flight_id) + ".txt");
            out << *flight;

            n = 0;
            seat_indices = flight->list_passengers(n);  
            if(n == 0) {
                //thông báo không có hành khách
                Menu::display_flight_not_booked_by_user();
                return;
            }
            cur_row = 0, cur_page = 0;
            max_page = (n + PASSENGERS_PER_PAGE - 1) / PASSENGERS_PER_PAGE - 1;

            Menu::display_passenger_list();
        }

    } 
    // Giải phóng bộ nhớ đã cấp phát cho mảng seat_indices
    delete[] seat_indices;
}

void Console::enter_delete_passenger(Flight *flight) {
    char ch;
    Menu::display_delete_passenger();
    char CMND[LEN_CMND];
    int idx = 0;
    while(true) {

        Menu::gotoxy(20 + 45, 6);
        enter(CMND, idx, LEN_CMND, ch, [&](char &c) {return 1;});

        if(ch == TAB) {
            break;
        } else if(ch == ENTER) {
            Node *test = manager.search(manager.root, CMND);
            if (test != nullptr) { // Kiểm tra test khác nullptr
                test->data.number_of_tickets--;
                std::ofstream out("data/Passenger/" + std::string(test->data.CMND) + ".txt");
                out << test->data;
                out.close();
                if (test->data.number_of_tickets == 0) {
                    std::filesystem::remove("data/Passenger/" + std::string(test->data.CMND) + ".txt");
                    manager.root = manager.erase(manager.root, test->data); // Cập nhật root
                }
                for(int i = 0; i < *flight->total_seats; ++i) {
                    if(flight->tickets[i].CMND != nullptr && strcmp(CMND, flight->tickets[i].CMND) == 0) {

                        flight->tickets[i].CMND = nullptr; 
                        break;
                    }
                }
            }
            break;
        }
    }   
}



void Console::enter_available_tickets(Flight *flight, int choice) {
    
    int n = 0;
    // Lấy danh sách chỉ số vé có người đặt (mảng được cấp phát động)
    int *seat_indices = flight->list_available_seats(n);  
    
    int current_page = 0, current_column = 0;
    int max_pages; // Số trang
    if (n == 0) max_pages = 1;
    else if (n % SEATS_PER_PAGE == 0){
        max_pages = n / SEATS_PER_PAGE;
    }
    else {
        max_pages = (n / SEATS_PER_PAGE) + 1;
    }

    Menu::display_available_tickets();
    while (true) {
        int start_idx = current_page * SEATS_PER_PAGE;

        int end_idx = std::min(start_idx + SEATS_PER_PAGE, static_cast<int>(n));

        // 📌 Hiển thị thông tin chuyến bay
        Menu::gotoxy(75, 3);  // Mã chuyến bay
        std::cout << flight->flight_id;

        Menu::gotoxy(38, 5);  // Số hiệu chuyến bay
        std::cout << flight->plane_id;

        Menu::gotoxy(81, 5);  // Điểm đến (Destination)
        std::cout << flight->destination;

        Menu::gotoxy(44, 7);  // Ngày khởi hành
        std::cout << flight->date_dep; 

        Menu::gotoxy(74, 7);  // Giờ khởi hành
        std::cout << flight->time_dep;

        Menu::gotoxy(36, 9);  // Trạng thái chuyến bay
        switch (flight->cur_status) {
            case status::cancelled:
                std::cout << "Cancelled";
                Menu::gotoxy(53, 15);
                Menu::display_flight_cancelled();
                return;
            case status::available:
                std::cout << "Available";
                break;
            case status::sold_out:
                std::cout << "Sold Out";
                Menu::gotoxy(53, 15);
                Menu::display_flight_sold_out();
                return;
            case status::completed:
                std::cout << "Completed";
                Menu::gotoxy(53, 15);
                Menu::display_flight_completed();
                return;
        }

        for (int i = start_idx; i < end_idx; i++) {
            int row = 13 + (i - start_idx);
            // Nếu đây là vé được chọn (current_column) thì in con trỏ ">>"
            if ((i - start_idx) == current_column) {
                Menu::gotoxy(26, row); // Vị trí in con trỏ, ví dụ cột 30
                std::cout << ">>";
            } else {
                // Nếu không phải vé được chọn, in khoảng trắng để giữ định dạng
                Menu::gotoxy(26, row);
                std::cout << "  ";
            }
            int ticketIndex = seat_indices[i];
            Menu::gotoxy(30, row);  // Cột hiển thị số ghế
            std::cout << ticketIndex + 1;
            Menu::gotoxy(69, row);  // Cột hiển thị trạng thái ghế
            std::cout << (flight->tickets[ticketIndex].CMND != nullptr ? "SOLD OUT" : "AVAILABLE");
        }

        Menu::display_list_instructions(current_page + 1, max_pages);
        Menu::gotoxy(0, 0);
        char key = _getch(); // Nhận phím nhập vào
        #ifdef _WIN32
                if (key == -32 || key == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    key = _getch(); // Lấy mã thực tế của phím
                }
        #else
                if (key == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        key = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif

        if (key == ESC) break;  // ESC để thoát
        else if (key == LEFT && current_page > 0){
            // sang trang mới vẽ lại khung
            Menu::display_available_tickets();
            current_page--;  // Phím mũi tên trái để chuyển trang về trước
            current_column = 0;
        }
        else if (key == RIGHT && current_page < max_pages - 1){
            // sang trang mới vẽ lại khung
            Menu::display_available_tickets();
            current_page++;  // Phím mũi tên phải để chuyển trang tiếp theo
            current_column = 0;
        }
        else if (key == UP && current_column > 0)
            current_column--;  // Phím mũi tên lên để chọn vé ở vị trí trước đó
        else if (key == DOWN && current_column < (end_idx - start_idx - 1))
            current_column++;  // Phím mũi tên xuống để chọn vé ở vị trí sau đó

        else if (key == ENTER && input != nullptr) { // Phím ENTER, chỉ user mới đặt được vé
            if (choice == 2){ // choice == 2 là manager chỉ xem khong đặt được vé
                continue;
            }
            int selected = start_idx + current_column;
            int seat_index = seat_indices[selected]; // Lấy chỉ số ghế thực
            if (flight->tickets[seat_index].CMND != nullptr) {
                continue; // Ghế đã được đặt
            }

            // Đặt vé

            flight->tickets[seat_index].CMND = input->CMND;
            input->number_of_tickets++;
            
            // Cập nhật AVL tree
            Node *test = manager.search(manager.root, input->CMND);
            if (test == nullptr) {
                manager.root = manager.insert(manager.root, *input);
            }
            //in ra mua thành công
            Menu::display_booking_success();
            // input->number_of_tickets++;
            std::ofstream out("data/Passenger/" + std::string(input->CMND) + ".txt");
            out << *input;


            if(n == 1) {
                flight->cur_status = status::sold_out;
            } 
            out = std::ofstream("data/Flights/" + std::string(flight->flight_id) + ".txt");
            out << *flight;

            break;
        }
    }
}
void Console::enter_user_information() {
    //khởi tạo input = neu passenger
    Console::input = new Passenger;
    char tmp_gender[2] = "\0", ch;
    int idx[4] = {}, column = 0;
    
    Menu::display_enter_user_id_card();
    while (true) {

        Menu::gotoxy(55, 6);
        std::cout << input->CMND;

        switch (column) {
            case 0:
                Menu::gotoxy(55 + idx[column], 6);
                // CMND chỉ nhận số thôi
                enter(input->CMND, idx[column], LEN_CMND, ch,
                    [&](char &c) { 
                        return (c >= 48 && c <= 57);
                    });
                break;
        }

        // Xử lý phím mũi tên
        #ifdef _WIN32
            if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                _getch(); // Bỏ qua ký tự của phím mũi tên
                continue;
            }
        #else
            if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                    _getch(); // Bỏ qua ký tự của phím mũi tên
                    continue;
                }
            }
        #endif


        // Nếu nhấn ESC thì thoát luôn
        if (ch == ESC){
            return;
        }
            
        // Nếu nhấn ENTER, tiến hành kiểm tra và xử lý 
        if (ch == ENTER) {
            if (strlen(input->CMND) > 0) {
                // tìm thấy user cũ: cập nhật lại input
                Node *tmp = nullptr;
                // strlen > 0 và gán search(...) vào tmp, rồi mới so sánh != nullptr
                tmp = manager.search(manager.root, input->CMND);
                if(tmp != nullptr){
                    Menu::display_user_exist(); // thông báo user đã tồn tại và đặt vé luôn
                    input = &tmp->data;
                    enter_available_flights(1); // 1 là user
                    break;
                }
                else {
                    // user chưa tồn tại
                    Menu::display_user_not_found(); // thông báo user chưa tồn tại, và tạo tài khoản mới
                    column = 1;
                    Menu::display_enter_user_information();
                    while (true) {

                        Menu::gotoxy(55, 6);
                        std::cout << input->CMND;
                        Menu::gotoxy(60, 9);
                        std::cout << input->last_name;
                        Menu::gotoxy(61, 12);
                        std::cout << input->first_name;
                        Menu::gotoxy(78, 15);
                        std::cout << (tmp_gender);
                
                        switch (column) {
                            case 0:
                                Menu::gotoxy(55 + idx[column], 6);
                                // CMND chỉ nhận số thôi
                                enter(input->CMND, idx[column], LEN_CMND, ch,
                                    [&](char &c) { 
                                        return (c >= 48 && c <= 57);
                                    });
                                break;
                            case 1:
                                Menu::gotoxy(60 + idx[column], 9);
                                enter(input->last_name, idx[column], LEN_LAST_NAME, ch,
                                    [&](char &c) { 
                                        if(c >= 'a' && c <= 'z') c -= 32;
                                        return (c >= 'A' && c <= 'Z') || c == ' ';
                                    });
                                break;
                            case 2: {
                                Menu::gotoxy(61 + idx[column], 12);
                                enter(input->first_name, idx[column], LEN_FIRST_NAME, ch,
                                    [&](char &c) { 
                                        if(c >= 'a' && c <= 'z') c -= 32;
                                        return (c >= 'A' && c <= 'Z');
                                    });
                                break;
                            }
                            case 3:
                                Menu::gotoxy(78 + (input->gender != nullptr ? 1 : 0), 15);
                                enter(tmp_gender, idx[column], 2, ch, [](char &c) { return c == '0' || c == '1'; });
                                if (strlen(tmp_gender)) input->gender = new bool(tmp_gender[0] == '1');
                                else input->gender = nullptr;
                                break;
                        }
                        
                        if (ch == UP && column > 0) {
                            --column;
                        } else if (ch == DOWN && column < 3) {
                            ++column;
                        } else if (ch == ENTER) {
                            if (column == 0){
                                if (strlen(input->CMND) > 0) {
                                    // tìm thấy user cũ: cập nhật lại input
                                    Node *tmp = nullptr;
                                    // strlen > 0 và gán search(...) vào tmp, rồi mới so sánh != nullptr
                                    tmp = manager.search(manager.root, input->CMND);
                                    if(tmp != nullptr){
                                        Menu::display_user_exist(); // thông báo user đã tồn tại và đặt vé luôn
                                        input = &tmp->data;
                                        enter_available_flights(1); // 1 là user
                                    }
                                    else {
                                        column++;
                                        continue;
                                    }
                                    
                                }
                            }
                            else if (column < 3){
                                column++;
                                continue;
                            }
                            else {
                                // hiện tại ở dòng cuối cùng
                                // Nếu thông tin hiện tại hợp lệ (user mới hoặc đã load user cũ)
                                if (input->valid_user()) {
                                    Menu::display_user_create_success(); // thông báo tạo tài khoản thành công và cho đặt vé
                                    enter_available_flights(1); // 1 là user
                                    return;
                                }
                            }
                            
                            
                        } else if(ch == ESC) return;
                    
                    }
                } 
            }
            else {
                continue;
            }

                
        }
        
    
    }
}
void Console::enter_manage_plane() {
    char ch;
    int i = 0;
    Menu::display_manage_plane();
    while(true) {
        for(int j = 0; j < 3; ++j) { 
            if(j == i) {
                Menu::gotoxy(27 + 27 - 3, 6 + j*3);
                std::cout << ">>";
            } else {
                Menu::gotoxy(27 + 27 - 3, 6 + j*3);
                std::cout << "  ";
            }

        }
        Menu::gotoxy(0, 0);
        ch = _getch();
        #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    ch = _getch(); // Lấy mã thực tế của phím
                }
        #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        ch = _getch(); // Lấy mã thực tế của phím
                    }
                }
        #endif

        if(ch == TAB) {
            return;
        } else if(ch == UP && i > 0) --i;
        else if(ch == DOWN && i < 2) ++i;
        else if(ch == ENTER) {
            if(i == 0) {
                enter_plane_information();
            } else if(i == 1) {
                enter_plane_delete();
            } else if(i == 2) {
                enter_plane_update();
            }
            Menu::display_manage_plane();
            continue;
        } else if(ch == ESC) {
            return;
        }
    }
}
void Console::enter_plane_list() {
    char ch = '\0';
    int number_of_planes = get_plane_count();
    int cur_page = 0;
    int max_page;
    if (number_of_planes == 0) max_page = 1;
    else if (number_of_planes % PLANES_PER_PAGE == 0){
        max_page = number_of_planes / PLANES_PER_PAGE;
    }
    else {
        max_page = (number_of_planes / PLANES_PER_PAGE) + 1;
    }
    int cur_row = 0;

    Menu::display_plane_list();
    while (true) {
        // Vẽ lại khung

        // In danh sách + con trỏ
        for (int i = 0; i < PLANES_PER_PAGE; ++i) {
            int idx = i + cur_page * PLANES_PER_PAGE;

            // Xóa con trỏ cũ
            Menu::gotoxy(24, 7 + i);
            std::cout << "  ";

            if (idx < number_of_planes) {
                // Nếu đây là dòng được chọn thì in >>
                if (i == cur_row) {
                    Menu::gotoxy(24, 7 + i);
                    std::cout << ">>";
                }
                // In Plane ID
                Menu::gotoxy(27, 7 + i);
                std::cout << list_planes[idx]->plane_id;
                // In Plane Type
                Menu::gotoxy(27 + 23, 7 + i);
                std::cout << list_planes[idx]->plane_type;
                // In số chỗ
                Menu::gotoxy(27 + 23 + 43, 7 + i);
                std::cout << list_planes[idx]->number_of_seats;
            }
        }

        // In hướng dẫn
        Menu::display_list_instructions_tab(cur_page + 1, max_page);
        Menu::gotoxy(0, 0);
        std::cout << cur_page * PLANES_PER_PAGE << " " << number_of_planes;
        ch = _getch();

        #ifdef _WIN32
            if (ch == -32 || ch == 224) ch = _getch();
        #else
            if (ch == ESC && _getch() == '[') ch = _getch();
        #endif

        // Điều hướng
        if (ch == UP && cur_row > 0) {
            --cur_row;
        } else if (ch == DOWN
                   && cur_row < PLANES_PER_PAGE - 1
                   && cur_row + cur_page * PLANES_PER_PAGE < number_of_planes - 1) {
            ++cur_row;
        } else if (ch == RIGHT && cur_page < max_page - 1) {
            ++cur_page;
            cur_row = 0;
            // sang trang mới vẽ lại khung
            Menu::display_plane_list();
        } else if (ch == LEFT && cur_page > 0) {
            --cur_page;
            cur_row = 0;
            // sang trang mới vẽ lại khung
            Menu::display_plane_list();
        } else if (ch == TAB) {
            // Quay về menu quản lý – sau khi xong, load lại số plane & reset trang
            enter_manage_plane();
            Menu::display_plane_list();
            number_of_planes = get_plane_count();
            if (number_of_planes == 0) max_page = 1;
            else if (number_of_planes % PLANES_PER_PAGE == 0){
                max_page = number_of_planes / PLANES_PER_PAGE;
            }
            else {
                max_page = (number_of_planes / PLANES_PER_PAGE) + 1;
            }
            cur_page = 0;
            cur_row = 0;
        } else if (ch == ESC) {
            return;
        }
    }
}


// Flight* Console::find_info_next(Flight *cur, char *date_dep, char *destination) {
//     while(cur != nullptr && !((date_dep != nullptr && strcmp(cur->date_dep.to_string(), date_dep) == 0) && (destination != nullptr && cur->destination == destination))) {
//         cur = cur->next;
//     } 
//     return cur;
// }

void Console::enter_available_flights(int choice) {
    load_flights_before_view();
    // if(input != nullptr) {
    //     //thông báo chưa có chuyến bay;
    //     return;
    // }
    char ch = '\0';
    unsigned int total_flights = count_flights();
    
    unsigned int number_of_pages;
    if (total_flights == 0) number_of_pages = 1;
    else if (total_flights % FLIGHTS_PER_PAGE == 0){
        number_of_pages = total_flights / FLIGHTS_PER_PAGE;
    }
    else {
        number_of_pages = (total_flights / FLIGHTS_PER_PAGE) + 1;
    }
    

    Flight **pages = new Flight*[number_of_pages];
    pages[0] = list;

    for (unsigned int i = 1; i < number_of_pages; ++i) {
        Flight *j = pages[i - 1];
        for (int count = 0; j != nullptr && count < FLIGHTS_PER_PAGE; j = j->next, ++count);
        pages[i] = j;
    }

    unsigned int cur_page = 0, cur_row = 0;
    int idx[2] = {};
    char dep_date[11] = "\0";
    char destination[LEN_DESTINATION] = "\0";

    Menu::display_flight_list();
    Menu::display_enter_flight_details();
    
    while (true) {
        Menu::gotoxy(35,27);
        if (choice == 1){
            // in ra các phím điều hướng không có tab ở user
            Menu::display_list_instructions(cur_page + 1, number_of_pages);
        }
        else if (choice == 2){
            // in ra các phím điều hướng có tab ở manager
            Menu::display_list_instructions_tab(cur_page + 1, number_of_pages);
        }
        

        Flight *page_start = pages[cur_page];
        Flight *page_end = (cur_page + 1 < number_of_pages) ? pages[cur_page + 1] : nullptr;

        unsigned int count_on_page = 0;
        Flight *tmp = page_start;
        while (tmp != page_end && tmp != nullptr) {
            if ((strlen(dep_date) == 0 || strcmp(tmp->date_dep.to_string(), dep_date) == 0) &&
                (strlen(destination) == 0 || strcmp(tmp->destination, destination) == 0)) {
                count_on_page++;
            }
            tmp = tmp->next;
        }

        tmp = page_start;
        int i = 0;
        while (tmp != page_end && tmp != nullptr) {
            if ((strlen(dep_date) == 0 || strcmp(tmp->date_dep.to_string(), dep_date) == 0) &&
                (strlen(destination) == 0 || strcmp(tmp->destination, destination) == 0)) {
                if (i == cur_row) {
                    Menu::gotoxy(22, i + 7);
                    std::cout << ">>";
                } 
                Menu::gotoxy(26, i + 7);
                std::cout << tmp->flight_id;
                Menu::gotoxy(43, i + 7);
                std::cout << tmp->plane_id;
                Menu::gotoxy(62, i + 7);
                std::cout << tmp->date_dep << "|" << tmp->time_dep;
                Menu::gotoxy(80, i + 7);
                std::cout << tmp->destination;
                Menu::gotoxy(99, i + 7);
                switch (tmp->cur_status) {
                    case status::cancelled:
                        std::cout << "cancelled";
                        break;
                    case status::available:
                        std::cout << "available";
                        break;
                    case status::sold_out:
                        std::cout << "sold out";
                        break;
                    case status::completed:
                        std::cout << "completed";
                        break;
                }
                ++i;
            }
            tmp = tmp->next;
        }
        Menu::gotoxy(70, 6 + 7 + 7);
        std::cout << dep_date;
        Menu::gotoxy(74, 6 + 7 + 7 + 2);
        std::cout << destination;

        if (cur_row >= count_on_page && cur_row <= count_on_page + 1) {
            if (cur_row == count_on_page) {
                Menu::gotoxy(70 + idx[0], 6 + 7 + 7);
               
                enter(dep_date, idx[0], 11, ch,
                    [&](char &c) { return (c >= '0' && c <= '9' || c == '/'); });
                int read = 0, day, month, year;
                read = sscanf(dep_date, "%d/%d/%d", &day, &month, &year);
                // chuẩn hoá lại chuỗi nhập ngày tháng
                if (read == 3){ // Nếu tách được 3 số là day, month và year
                    // chuẩn hoá lại cho đúng định dạng ngày
                    if (dep_date[1] == '/'){
                        for (int i = idx[0]; i > 0; i--){
                            dep_date[i] = dep_date[i - 1];
                        }
                        dep_date[0] = '0';
                        idx[0]++;
                    }
                    if (dep_date[4] == '/'){
                        for (int i = idx[0]; i > 3; i--){
                            dep_date[i] = dep_date[i - 1];
                        }
                        dep_date[3] = '0';
                        idx[0]++;
                    }
                }
            } else if (cur_row == count_on_page + 1) {
                Menu::gotoxy(74 + idx[1], 6 + 7 + 7 + 2);

                enter(destination, idx[1], LEN_DESTINATION, ch,
                    [&](char &c) {
                        if (c >= 'a' && c <= 'z') c -= 32;
                        return (c >= 'A' && c <= 'Z' || c == ' ');
                    });
                    
            }
        } else {
            Menu::gotoxy(0, 0);
            ch = _getch();
            #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    ch = _getch(); // Lấy mã thực tế của phím
                }
            #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        ch = _getch(); // Lấy mã thực tế của phím
                    }
                }
            #endif
        }
        


        if (ch == UP && cur_row > 0) {
            Menu::gotoxy(22, cur_row + 7);
            std::cout << "  ";
            --cur_row;
        } else if (ch == DOWN && cur_row + 1 < count_on_page + 2) {
            if(cur_row + 1 < count_on_page) {
                Menu::gotoxy(22, cur_row + 7);
                std::cout << "  ";
            }
            ++cur_row;
        } else if (ch == RIGHT && cur_page < number_of_pages - 1) {
            ++cur_page;
            cur_row = 0;
            // sang trang mới vẽ lại khung
            Menu::display_flight_list();
            Menu::display_enter_flight_details();
        } else if (ch == LEFT && cur_page > 0) {
            --cur_page;
            cur_row = 0;
            // sang trang mới vẽ lại khung
            Menu::display_flight_list();
            Menu::display_enter_flight_details();
        } else if (ch == ESC) {
            Console::input = nullptr;
            delete[] pages;
            return;
        } else if (ch == ENTER) {
            if (cur_row >= count_on_page && cur_row <= count_on_page + 1) { // enter ở thanh tìm kiếm chuyến bay
                Flight *filtered_list = nullptr;
                Flight **filtered_pages = nullptr;
                unsigned int filtered_total = 0;

                Flight *tmp = list;
                while (tmp != nullptr) {
                    if ((strlen(dep_date) == 0 || strcmp(tmp->date_dep.to_string(), dep_date) == 0) &&
                        (strlen(destination) == 0 || strcmp(tmp->destination, destination) == 0)) {
                        if (filtered_list == nullptr) {
                            filtered_list = tmp;
                        }
                        ++filtered_total;
                    }
                    tmp = tmp->next;
                }

                unsigned int filtered_pages_count;
                if (filtered_total == 0) filtered_pages_count = 1;
                else if (filtered_total % FLIGHTS_PER_PAGE == 0){
                    filtered_pages_count = filtered_total / FLIGHTS_PER_PAGE;
                }
                else {
                    filtered_pages_count = (filtered_total / FLIGHTS_PER_PAGE) + 1;
                }
                filtered_pages = new Flight*[filtered_pages_count];
                filtered_pages[0] = filtered_list;

                tmp = filtered_list;
                for (unsigned int i = 1; i < filtered_pages_count; ++i) {
                    for (int count = 0; tmp != nullptr && count < FLIGHTS_PER_PAGE; tmp = tmp->next, ++count);
                    filtered_pages[i] = tmp;
                }

                delete[] pages;
                pages = filtered_pages;
                number_of_pages = filtered_pages_count;
                cur_page = 0;
                cur_row = 0;
                Menu::display_flight_list();
                Menu::display_enter_flight_details();
                continue;
            }
            else { // enter ở các chuyến bay để đặt vé
                Flight *selected_flight = page_start;
                for (unsigned int j = 0; j < cur_row && selected_flight != nullptr; ++j) {
                
                    selected_flight = selected_flight->next;
                }
                if (input == nullptr) {
                    // input == nullptr tức là manager, chỉ xem số ghế không cho đặt vé
                    enter_available_tickets(selected_flight, 2); // 2 là manager
                    Menu::display_flight_list();
                    Menu::display_enter_flight_details();
                    continue;
                } else {
                    if (selected_flight->valid_user(input->CMND)) {
                        
                        enter_available_tickets(selected_flight, 1); // 1 là user
                        Menu::display_flight_list();
                        Menu::display_enter_flight_details();
                        load_flights_before_view();
                    } else {
                        //in ra thông báo
                        Menu::display_booking_error();
                        Menu::display_flight_list();
                        Menu::display_enter_flight_details();
                        continue;
                    }
                }
                // break;
            }
        } else if (ch == TAB) {
            if(input == nullptr) {
                enter_flight_manager_menu();
                Menu::display_flight_list();
                Menu::display_enter_flight_details();
                load_flights_before_view();
            } else {

            }
        }
    }

    delete[] pages;
}

int Console::get_plane_count() {
    int count = 0;
    while (count < MAX_PLANE && Console::list_planes[count] != nullptr) {
        count++;
    }
    return count;
}

bool Console::search_plane_id(char *target) {
    for (int i = 0; i < MAX_PLANE && list_planes[i] != nullptr; i++) {
        if (strcmp(list_planes[i]->plane_id, target) == 0) return true;
    }
    return false;
}

void Console::add_plane(const Plane &other) {

    int i = 0;
    while (i < MAX_PLANE && list_planes[i] != nullptr) {
        i++;
        if(list_planes[i]->plane_id == other.plane_id) {
            //in ra thông báo có rồi;
            Menu::display_aircraft_exist();
            return;
        }
    }


    list_planes[i] = new Plane();
    strcpy(list_planes[i]->plane_id, other.plane_id);
    strcpy(list_planes[i]->plane_type, other.plane_type);
    list_planes[i]->number_of_seats = other.number_of_seats;
    list_planes[i]->number_flights_performed = other.number_flights_performed;
    
    

    std::ofstream out("data/Planes/"+ std::string(other.plane_id) + ".txt",std::ios::app);
    out << i << std::endl;
    out << other;

}

void Console::delete_plane(const char *plane_id) {
    // Kiểm tra xem có chuyến bay nào đang dùng máy bay này không
    for (Flight *p = list; p != nullptr; p = p->next) {
        if (p->plane_id != nullptr && strcmp(p->plane_id, plane_id) == 0) {
            // Nếu có chuyến bay đang sử dụng máy bay này không cho phép xoá
            Menu::display_delete_plane_error();
            return;
        }
    }

    // Nếu khong có chuyến bay dùng nó, hỏi xác nhận người dùng

    Menu::display_delete_plane_confirm();
    int curr_row = 0;
    while (true) {
        int idx;
        // Xóa con trỏ cũ ở tất cả dòng
        for (int i = 0; i < 2; ++i) {
            Menu::gotoxy(45, 5 + i);
            std::cout << "  ";
        }

        // In con trỏ ở dòng đang chọn
        Menu::gotoxy(45, 5 + curr_row);
        std::cout << ">>";

        Menu::gotoxy(0, 0);
        char ch = _getch();

        #ifdef _WIN32
            if (ch == -32 || ch == 224) ch = _getch();
        #else
            if (ch == ESC && _getch() == '[') ch = _getch();
        #endif

        // Điều hướng
        if (ch == UP && curr_row > 0) {
            --curr_row;
        } else if (ch == DOWN && curr_row < 1) {
            ++curr_row;
        } else if (ch == RIGHT || ch == LEFT) {
            continue;
        } else if (ch == ENTER) {
            if (curr_row == 0) {
                break; // tiếp tục xử lý xoá sau
            } else {
                return; // nếu không xoá thì thoát luôn
            }
        }
    
    }

    //  Nếu vẫn xác nhận xoá thì mới cho phép xoá
    for (int i = 0; i < MAX_PLANE && list_planes[i] != nullptr; i++) {
        if (strcmp(list_planes[i]->plane_id, plane_id) == 0) {
            int j;

            // Xóa file máy bay
            std::filesystem::remove("data/Planes/" + std::string(list_planes[i]->plane_id) + ".txt");
            delete list_planes[i];

            // Dịch các máy bay phía sau lên
            for (j = i; j < MAX_PLANE - 1 && list_planes[j + 1] != nullptr; j++) {
                list_planes[j] = list_planes[j + 1];
            }
            list_planes[j] = nullptr;

            // Ghi lại file máy bay từ vị trí i
            for (int k = i; k < j; ++k) {
                std::ofstream out("data/Planes/" + std::string(list_planes[k]->plane_id) + ".txt", std::ios::out);
                if (out) {
                    out << k << std::endl;
                    out << *list_planes[k];
                }
            }

            Menu::display_success_delete_aircraft();
            return;
        }
    }

    // Nếu không tìm thấy máy bay
    Menu::display_aircraft_not_found();
}

void Console::update_plane(const Plane &other) {
    for (int i = 0; i < MAX_PLANE && list_planes[i] != nullptr; i++) {
        if (strcmp(list_planes[i]->plane_id, other.plane_id) == 0) {
            strncpy(list_planes[i]->plane_type, other.plane_type, LEN_PLANE_TYPE);
            list_planes[i]->number_of_seats = other.number_of_seats; 
            //đếm danh sách chuyến bay sử dụng máy bay này có số vè đã bán phải >= other.number_of_seats
            //và dồn danh sách vé ở chuyến bay
            list_planes[i]->number_flights_performed = other.number_flights_performed;


            std::ofstream out("data/Planes/"+ std::string(list_planes[i]->plane_id) + ".txt");
            out << i << std::endl;
            out << other;
            return;
        }
    }
    // không tìm thấy máy bay
    Menu::display_aircraft_not_found();
    return;
}
void Console::enter_plane_information() {

    if (get_plane_count() == MAX_PLANE) {
        // Nếu danh sách máy bay đầy, không thể thêm
        Menu::display_full_aircraft_list();
        return;
    }

    Plane other = Plane();
    char ch;
    int idx[4] = {0}; // Chỉ số con trỏ cho từng trường nhập (cột)
    int column = 0;
    char num_str_seat[5] = {}; // Chuỗi nhập số ghế của máy bay
    char num_str[5] = {}; // Chuỗi nhâp số lần bay của máy bay

    Menu::display_add_aircraft();
    while (true) {

        // Hiển thị thông tin đã nhập
        Menu::gotoxy(52, 6);
        std::cout << other.plane_id;

        Menu::gotoxy(54, 9);
        std::cout << other.plane_type;
        Menu::gotoxy(66, 12);
        if (other.number_of_seats > 0) {
            std::cout << num_str_seat;
        }

        Menu::gotoxy(61, 15);
        if (other.number_flights_performed > 0) {
            std::cout << num_str;
        }


        // Nhập liệu theo các trường (cột)
        switch (column) {
            case 0:
                Menu::gotoxy(52 + idx[column], 6);
                enter(other.plane_id, idx[column], LEN_PLANE_ID, ch,
                    [&](char &c) {
                        if (c >= 'a' && c <= 'z') c -= 32;
                        return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
                    });
                
                break;
            case 1:
                Menu::gotoxy(54 + idx[column], 9);
                enter(other.plane_type, idx[column], LEN_PLANE_TYPE, ch,
                    [&](char &c) { 
                        if (c >= 'a' && c <= 'z')
                            c -= 32;
                        return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ';
                    }); 

                break;
            case 2: {
                Menu::gotoxy(66 + idx[column], 12);
                
                enter(num_str_seat, idx[column], 5, ch,
                    [&](char &c) { return (c >= '0' && c <= '9'); });
                other.number_of_seats = atoi(num_str_seat);
                break;
            }
            case 3:
                Menu::gotoxy(61 + idx[column], 15);
                
                enter(num_str, idx[column], 5, ch,
                    [&](char &c) { return (c >= '0' && c <= '9'); });
                other.number_flights_performed = atoi(num_str);
                break;
        }
        
        // Xử lý các phím di chuyển và phím chức năng
        if (ch == UP) {
            if (column > 0) {
                column--;
                continue;
            }
        }
        else if (ch == DOWN) {
            if (column < 3) {
                ++column;
                continue;
            }
        } 
        else if (ch == ENTER) {
            // Hiện tại đang ở dòng cuối cùng
            // Nếu các trường dữ liệu còn trống, không cho kết thúc
            if (search_plane_id(other.plane_id)) {
                // Nếu đã tồn tại mã máy bay, thông báo lỗi và reset lại trường mã
                Menu::display_aircraft_exist();
                // memset(other.plane_id, 0, sizeof(other.plane_id));
                // idx[0] = 0;
                column = 0;
                Menu::display_add_aircraft();
                continue;
            }
            if (strlen(other.plane_type) == 0){
                column = 1;
                continue;
            }
            if (other.number_of_seats < 20){
                column = 2;
                continue;
            }
            if (other.number_flights_performed <= 0){
                column = 3;
                continue;
            }
            // Đã nhập xong tất cả các trường, tiến hành thêm máy bay
            add_plane(other);
            Menu::display_success_add_aircraft();
            Menu::display_add_aircraft();
            // return;
        }
        else if (ch == ESC) {
            // Nếu nhấn ESC, thoát khỏi hàm nhập liệu
            return;
        }
    }
}

void Console::enter_plane_update() {
    if (get_plane_count() == 0) {
        // Nếu danh sách máy bay rỗng, không thể sửa
        Menu::display_empty_aircraft_list();
        return;
    }

    Plane other = Plane();
    char ch;
    int idx[4] = {0};
    int column = 0;
    char num_str_seat[5] = {}; // Chuỗi nhập số ghế của máy bay
    char num_str[5] = {}; // Chuỗi nhâp số lần bay của máy bay
    // bool planeIdValidated = false; // Cờ xác nhận mã máy bay đã được kiểm tra

    Menu::display_edit_aircraft_details();
    while (true) {

        // Hiển thị thông tin đã nhập
        Menu::gotoxy(66, 6);
        std::cout << other.plane_id;
        Menu::gotoxy(54, 9);
        std::cout << other.plane_type;
        Menu::gotoxy(66, 12);
        if (other.number_of_seats > 0) {
            std::cout << other.number_of_seats;
        }
        Menu::gotoxy(61, 15);
        if (other.number_flights_performed > 0) {
            std::cout << other.number_flights_performed;
        }

        // Nhập liệu theo các trường (cột)
        switch (column) {
            case 0:
                Menu::gotoxy(66 + idx[column], 6);
                enter(other.plane_id, idx[column], LEN_PLANE_ID, ch,
                    [&](char &c) { 
                        if (c >= 'a' && c <= 'z')
                            c -= 32;
                        return ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
                    });
                break;
            case 1:
                Menu::gotoxy(54 + idx[column], 9);
                enter(other.plane_type, idx[column], LEN_PLANE_TYPE, ch,
                    [&](char &c) { 
                        if (c >= 'a' && c <= 'z')
                            c -= 32;
                        return ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ');
                    });
                break;
            case 2: {
                Menu::gotoxy(66 + idx[column], 12);

                enter(num_str_seat, idx[column], 5, ch,
                    [&](char &c) { return (c >= '0' && c <= '9'); });
                other.number_of_seats = atoi(num_str_seat);
                break;
            }
            case 3:
                Menu::gotoxy(61 + idx[column], 15);

                enter(num_str, idx[column], 5, ch,
                    [&](char &c) { return (c >= '0' && c <= '9'); });
                other.number_flights_performed = atoi(num_str);
                break;
        }

        // Xử lý các phím di chuyển và phím chức năng
        if (ch == UP || ch == 'A') {
            if (column > 0)
                --column;
        } 
        else if (ch == DOWN || ch == 'B') {
            if (column < 3)
                ++column;
        } 
        else if (ch == ENTER) {
            // Ở trường mã máy bay (column 0) thì cần kiểm tra xác nhận nếu chưa được xác thực

            if (!search_plane_id(other.plane_id)) {
                // Nếu không tìm thấy mã máy bay, thông báo lỗi và reset lại trường mã
                Menu::display_aircraft_not_found();
                // memset(other.plane_id, 0, sizeof(other.plane_id));
                // idx[0] = 0;
                // Giữ nguyên column = 0 để nhập lại mã máy bay
                Menu::display_edit_aircraft_details();
                continue;
            }
            // Hiện tại đang ở dòng cuối cùng
            // Nếu các trường dữ liệu còn trống, không cho kết thúc
            if (strlen(other.plane_type) == 0){
                column = 1;
                continue;
            }
            if (other.number_of_seats < 20){
                column = 2;
                continue;
            }
            if (other.number_flights_performed <= 0){
                column = 3;
                continue;
            }
            // Khi đã nhập đủ thông tin, tiến hành cập nhật
            Console::update_plane(other);
            Menu::display_success_update_aircraft();
            Menu::display_edit_aircraft_details();
            // return;
        } 
        else if (ch == ESC) {
            // Nếu nhấn ESC, thoát khỏi chế độ sửa
            return;
        }
    }
}


void Console::enter_plane_delete() {

    if (get_plane_count() == 0) {
        // Nếu danh sách máy bay rỗng, không thể xoá
        Menu::display_empty_aircraft_list();
        return;
    }

    char plane_id[LEN_PLANE_ID] = {};
    char ch;
    int idx = 0;
    Menu::display_delete_aircraft();
    while (true) {

        // Hiển thị giao diện nhập xoá máy bay
        Menu::gotoxy(69, 6);
        std::cout << plane_id;

        // Vòng lặp nhập mã máy bay
        while (true) {
            enter(plane_id, idx, LEN_PLANE_ID, ch,
                  [&](char &c) {
                      if (c >= 'a' && c <= 'z')
                          c -= 32;
                      return ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
                  });

            // Trường hợp nhận được phím đặc biệt (arrow keys) trả về -32
            // Xử lý phím mũi tên
            #ifdef _WIN32
                if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
                    _getch(); // Bỏ qua ký tự của phím mũi tên
                    continue;
                }
            #else
                if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                    if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                        _getch(); // Bỏ qua ký tự của phím mũi tên
                        continue;
                    }
                }
            #endif
            break; // Nếu không phải arrow key thì thoát vòng lặp nhập
        }

        // Nếu nhấn ESC thì thoát luôn
        if (ch == ESC)
            return;
            
        // Nếu nhấn ENTER, tiến hành kiểm tra và xử lý xoá
        if (ch == ENTER) {
            if (!search_plane_id(plane_id)) {
                // Nếu không tìm thấy mã máy bay, hiển thị thông báo và cho phép nhập lại
                Menu::display_aircraft_not_found();
                Menu::display_delete_aircraft();
                continue;
            }
            else {
                // Đã tìm thấy mã, tiến hành xoá
                delete_plane(plane_id);
                Menu::display_delete_aircraft();
                return;
            }
        }
    }   
}



void Console::merge_sort() {
    int n = get_plane_count();
    auto merge = [&](int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        Plane* L[n1], *R[n2];

        for (int i = 0; i < n1; i++)
            L[i] = list_planes[left + i];
        for (int j = 0; j < n2; j++)
            R[j] = list_planes[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i]->number_flights_performed <= R[j]->number_flights_performed)
                list_planes[k++] = L[i++];
            else
                list_planes[k++] = R[j++];
        }

        while (i < n1) list_planes[k++] = L[i++];
        while (j < n2) list_planes[k++] = R[j++];
    };

    for (int len = 1; len < n; len *= 2) {
        for (int left = 0; left < n - len; left += 2 * len) {
            int right = std::min(left + 2 * len - 1, n - 1);
            int mid = left + len - 1;
            merge(left, mid, right);
        }
    }
    for(int i = 0; i < n/2; ++i) {
        int j = n - i - 1;
        Plane *tmp = list_planes[j];
        list_planes[j] = list_planes[i];
        list_planes[i] = tmp;
    }
}

// Handle flight
bool Console::search_flight_id(const char *flight_id){
    Flight *curr = list;
    while (curr != nullptr){
        if (strcmp(curr->flight_id, flight_id) == 0) return true;
        curr = curr->next;
    }
    return false;
}
int Console::get_plane_by_id(const char* plane_id) {
    for (int i = 0; i < MAX_PLANE && list_planes[i] != nullptr; i++) {
        if (strcmp(list_planes[i]->plane_id, plane_id) == 0) {
            return i;
        }
    }
    return -1;
}
void Console::create_flight(Flight &other) {
    int idx = get_plane_by_id(other.plane_id);
    if (idx == -1 || list_planes[idx] == nullptr) {
        Menu::display_aircraft_not_found();
        return;
    }

    Flight* new_node = new Flight(other); // Assumes Flight has a proper copy constructor
    new_node->total_seats = new unsigned int(list_planes[idx]->number_of_seats);
    new_node->plane_id = list_planes[idx]->plane_id;
    // strcpy(new_node->plane_id, list_planes[idx]->plane_id);

    if (*new_node->total_seats > 0) {
        new_node->tickets = new Ticket[*new_node->total_seats];
        for (unsigned int i = 0; i < *new_node->total_seats; ++i) {
            new_node->tickets[i].CMND = nullptr;
        }
    }

    // Insert into the linked list in sorted order
    Flight *tmp = new Flight();
    tmp->next = list;
    list = tmp;

    Flight *p = list;
    for(; p->next != nullptr && strcmp(new_node->flight_id, p->next->flight_id) > 0; p = p->next);
    new_node->next = p->next;
    p->next = new_node;

    list = list->next;
    delete tmp; // tmp là một đối tượng mà


    new_node->cur_status = status::available;
    // list_planes[idx]->number_flights_performed++; // Increment flight count for the plane

    // Write to file
    std::string path = "data/Flights/" + std::string(new_node->flight_id) + ".txt";
    std::ofstream out(path);
    if (out) {
        out << *new_node;
    } else {
        // Menu::display_file_error(path.c_str());
    }

    // // Update plane file to reflect incremented number_flights_performed
    // std::ofstream plane_out("data/Planes/" + std::string(list_planes[idx]->plane_id) + ".txt");
    // if (plane_out) {
    //     plane_out << idx << std::endl;
    //     plane_out << *list_planes[idx];
    // }
}





void Console::update_flight(const char *flight_id, const date_departure &new_date, const time_departure &new_time){
    Flight *curr = list;

    while (curr != nullptr) {
        if (strcmp(curr->flight_id, flight_id) == 0){
            if(curr->cur_status == status::cancelled || curr->cur_status == status::completed) {
                //in ra thông báo
                return;
            }
            curr->date_dep = new_date;
            curr->time_dep = new_time;

            return;
        }
        curr = curr->next;
    }
    std::ofstream out("data/Flights/" + std::string(curr->flight_id) + ".txt");
    out << *curr;
}

void Console::cancel_flight(const char *flight_id) {

    // Hỏi có thực sự muổn huỷ chuyến không, nếu không thì khỏi huỷ

    Menu::display_cancel_flight_confirm();
    int curr_row = 0;
    while (true) {
        int idx;
        // Xóa con trỏ cũ ở tất cả dòng
        for (int i = 0; i < 2; ++i) {
            Menu::gotoxy(54, 5 + i);
            std::cout << "  ";
        }

        // In con trỏ ở dòng đang chọn
        Menu::gotoxy(54, 5 + curr_row);
        std::cout << ">>";

        Menu::gotoxy(0, 0);
        char ch = _getch();

        #ifdef _WIN32
            if (ch == -32 || ch == 224) ch = _getch();
        #else
            if (ch == ESC && _getch() == '[') ch = _getch();
        #endif

        // Điều hướng
        if (ch == UP && curr_row > 0) {
            --curr_row;
        } else if (ch == DOWN && curr_row < 1) {
            ++curr_row;
        } else if (ch == RIGHT || ch == LEFT) {
            continue;
        } else if (ch == ENTER) {
            if (curr_row == 0) {
                break; // Nếu yes tiếp tục xử lý huỷ chuyến
            } else {
                return; // nếu không huỷ thì thoát
            }
        }
    
    }

    Flight *curr = list;
    // Tìm chuyến trong danh sách
    while (curr != nullptr) {
        if (strcmp(curr->flight_id, flight_id) == 0) {
            // Nếu đã hoàn tất thì không thể hủy
            if (curr->cur_status == status::completed) {
                Menu::display_cannot_cancel_flight();
                return;
            }
            if (curr->cur_status == status::cancelled){
                // Nếu đã huỷ rồi thì không huỷ nữa
                Menu::display_already_cancelled_flight();
                return;
            }
            // Đánh dấu hủy
            curr->cur_status = status::cancelled;

            // Ghi lại file
            {
                std::string path = "data/Flights/" + std::string(curr->flight_id) + ".txt";
                std::ofstream out(path);
                if (out) {
                    out << *curr;  // nhớ định nghĩa operator<< cho Flight
                } else {
                    // Menu::display_file_error(path.c_str());  
                }
            }
            Menu::display_success_cancel_flight();
            return;
        }
        curr = curr->next;
    }

    // Nếu không tìm thấy chuyến
    Menu::display_flight_not_found();
}

void Console::enter_flight_information() {
    Flight *other = new Flight; // Use unique_ptr for automatic cleanup
    char ch = '\0';
    int idx[5] = {0};
    int column = 0;
    char date_str[11] = "\0", time_str[6] = "\0";
    int day, month, year, hour, minute, read;

    
    while (true) {
        Menu::display_create_new_flight();

        Menu::gotoxy(53, 6); std::cout << other->flight_id;
        Menu::gotoxy(52, 9); std::cout << other->plane_id;
        Menu::gotoxy(71, 12); std::cout << date_str;
        Menu::gotoxy(66, 15); std::cout << time_str;
        Menu::gotoxy(55, 18); std::cout << other->destination;

        switch (column) {
            case 0:
                Menu::gotoxy(53 + idx[column], 6);
                enter(other->flight_id, idx[column], LEN_FLIGHT_ID, ch,
                    [&](char &c) {
                        if (c >= 'a' && c <= 'z') c -= 32;
                        return (c >= 'A' && c <= 'Z' || c >= '0' && c <= '9');
                    });
                break;
            case 1:
                Menu::gotoxy(52 + idx[column], 9);
                enter(other->plane_id, idx[column], LEN_PLANE_ID, ch,
                    [&](char &c) {
                        if (c >= 'a' && c <= 'z') c -= 32;
                        return (c >= 'A' && c <= 'Z' || c >= '0' && c <= '9');
                    });
                break;
            case 2:
                Menu::gotoxy(71 + idx[column], 12);
                enter(date_str, idx[column], 11, ch,
                    [&](char &c) { return (c >= '0' && c <= '9' || c == '/'); });
                read = 0;
                read = sscanf(date_str, "%d/%d/%d", &day, &month, &year);
                // chuẩn hoá lại chuỗi nhập ngày tháng
                if (read == 3){ // Nếu tách được 3 số là day, month và year
                    // chuẩn hoá lại cho đúng định dạng ngày
                    if (date_str[1] == '/'){
                        for (int i = idx[column]; i > 0; i--){
                            date_str[i] = date_str[i - 1];
                        }
                        date_str[0] = '0';
                        idx[column]++;
                    }
                    if (date_str[4] == '/'){
                        for (int i = idx[column]; i > 3; i--){
                            date_str[i] = date_str[i - 1];
                        }
                        date_str[3] = '0';
                        idx[column]++;
                    }
                }
                break;
            case 3:
                Menu::gotoxy(66 + idx[column], 15);
                enter(time_str, idx[column], 6, ch,
                    [&](char &c) { return (c >= '0' && c <= '9' || c == ':'); });
                read = 0;
                read = sscanf(time_str, "%d:%d", &hour, &minute);
                // chuẩn hoá lại chuỗi nhập thời gian
                if (read == 2){ // Nếu tách dược 2 số nguyên là hour và minute
                    // Chuẩn hoá chuỗi lại cho đúng định dạng
                    if (time_str[1] == ':'){
                        for (int i = idx[column]; i > 0; i--){
                            time_str[i] = time_str[i - 1];
                        }
                        time_str[0] = '0';
                        idx[column]++;
                    }
                    if (time_str[4] == '\0'){
                        time_str[4] = time_str[3];
                        time_str[3] = '0';
                        idx[column]++;
                    }
                }
                break;
            case 4:
                Menu::gotoxy(55 + idx[column], 18);
                enter(other->destination, idx[column], LEN_DESTINATION, ch,
                    [&](char &c) {
                        if (c >= 'a' && c <= 'z') c -= 32;
                        return (c >= 'A' && c <= 'Z' || c == ' ');
                    });
                break;
        }

        if (ch == UP && column > 0) --column;
        else if (ch == DOWN && column < 4) ++column;
        else if (ch == ENTER) {
            if (column == 0) {
                if (search_flight_id(other->flight_id)) {
                    Menu::display_flight_exist();
                    Menu::display_create_new_flight();
                    // memset(other->flight_id, 0, LEN_FLIGHT_ID);
                    // idx[0] = 0;
                    // Menu::display_create_new_flight();
                    ch = '\0';
                    continue;
                } else {
                    ++column;
                }
            } else if (column == 1) {
                if (!search_plane_id(other->plane_id)) {
                    Menu::display_aircraft_not_found();
                    Menu::display_create_new_flight();
                    // Menu::display_create_new_flight();
                    ch = '\0';
                    // memset(other->plane_id, 0, LEN_PLANE_ID);
                    // idx[1] = 0;
                    continue;
                } else {
                    ++column;
                }
            } else if (column < 4) {
                ++column;
                continue;
            } else {
                bool failed = false;
                if (search_flight_id(other->flight_id)) {
                    Menu::display_flight_exist();
                    // Menu::display_create_new_flight();
                    // memset(other->flight_id, 0, LEN_FLIGHT_ID);
                    // idx[0] = 0;
                    // Menu::display_create_new_flight();
                    // continue;
                    failed = true;
                }
                if (!search_plane_id(other->plane_id)) {
                    Menu::display_aircraft_not_found();
                    // Menu::display_create_new_flight();
                    // Menu::display_create_new_flight();
                    // memset(other->plane_id, 0, LEN_PLANE_ID);
                    // idx[1] = 0;
                    // continue;
                    failed = true;
                }
                if(failed) {
                    ch = '\0';
                    Menu::display_create_new_flight();
                    continue;
                }
                if (strlen(other->flight_id) == 0) { column = 0; continue; }
                if (strlen(other->plane_id) == 0) { column = 1; continue; }
                if (strlen(other->destination) == 0) { column = 4; continue; }
                if (!other->valid_date(day, month, year)) { column = 2; continue; }
                if (!other->valid_time(hour, minute)) { column = 3; continue; }

                other->date_dep.day = day;
                other->date_dep.month = month;
                other->date_dep.year = year;
                other->time_dep.hour = hour;
                other->time_dep.minute = minute;
                if(is_completed(&other->date_dep, &other->time_dep)) {
                    //in ra thông báo không được đặt;
                    Menu::display_create_flight_error();
                    column = 2;
                    continue;
                }

                // Call create_flight and check success
                int idx = get_plane_by_id(other->plane_id);
                if (idx == -1 || list_planes[idx] == nullptr) {
                    Menu::display_aircraft_not_found();
                    column = 1;
                    continue;
                }

                create_flight(*other);
                Menu::display_success_create_flight();
                Menu::display_create_new_flight();
                // other =  new Flight;
                ch = '\0';
                // delete[]other;
                // return; // unique_ptr automatically deletes other
            }
        } else if (ch == ESC) {
            return; // unique_ptr automatically deletes other
        }
    }
}
void Console::enter_flight_update(){
    if (count_flights() == 0){
        // Nếu danh sach chuyến bay rỗng, không thể sửa
        Menu::display_empty_flight_list();
        return;
    }

    char flight_id[LEN_FLIGHT_ID] = {};
    date_departure new_date = {0, 0, 0};
    time_departure new_time = {-1, -1};

    char ch = '\0';
    int idx[3] = {0}, column = 0;
    char date_str[11] = {}; // Mảng chứa chuỗi nhập ngày dd/mm/yyyy
    char time_str[6] = {}; // Mảng chứa chuỗi nhập thời gian hh:mm
    int day, month, year; // Các biến này để lưu giá trị khi lấy từ chuỗi date_str
    int hour, minute;
    int read; // Biến này để đọc xem có bao giá trị được tách ra khi dùng hàm sscanf

    
    while (true) {
        Menu::display_edit_flight_schedule();

        // Hiện thị thông tin đã nhập
        Menu::gotoxy(59, 6);
        std::cout << flight_id;
        Menu::gotoxy(75, 9);
        std::cout << date_str;
        Menu::gotoxy(70, 12);
        std::cout << time_str;
        
        // Nhập liệu theo các trường (cột)
        switch (column) {
            case 0: {
                Menu::gotoxy(59 + idx[column], 6);
                enter(flight_id, idx[column], LEN_FLIGHT_ID, ch,
                    [&](char &c) { 
                        if(c >= 'a' && c <= 'z') c -= 32;
                        return (c >= 'A' && c <= 'Z'
                            || c >= 48 && c <= 57);
                    });
                break;
            }         
            case 1: {
                Menu::gotoxy(75 + idx[column], 9);
                enter(date_str, idx[column], 11, ch,
                    [&](char &c) {
                        return (c >= 48 && c <= 57 || c == '/');
                    });
                
                // Nhập xong trường ngày rồi tiến hành lấy dữ liệu từ chuỗi nhập đó gán váo 3 số nguyên
                read = 0;
                read = sscanf(date_str, "%d/%d/%d", &day, &month, &year);
                if (read == 3){ // Nếu tách được 3 số là day, month và year
                    // chuẩn hoá lại cho đúng định dạng ngày
                    if (date_str[1] == '/'){
                        for (int i = idx[column]; i > 0; i--){
                            date_str[i] = date_str[i - 1];
                        }
                        date_str[0] = '0';
                        idx[column]++;
                    }
                    if (date_str[4] == '/'){
                        for (int i = idx[column]; i > 3; i--){
                            date_str[i] = date_str[i - 1];
                        }
                        date_str[3] = '0';
                        idx[column]++;
                    }
                }
                break;
            }     
            case 2: {
                Menu::gotoxy(70 + idx[column], 12);
                enter(time_str, idx[column], 6, ch,
                    [&](char &c) {
                        return ((c >= 48 && c <= 57) || c == ':');
                    });
                
                // Nhập xong trường thời gian rồi tiến hành lấy dữ liệu từ chuỗi nhập đó gán váo 2 số nguyên
                read = 0;
                read = sscanf(time_str, "%d:%d", &hour, &minute);
                if (read == 2){ // Nếu tách dược 2 số nguyên là hour và minute
                    // Chuẩn hoá chuỗi lại cho đúng định dạng
                    if (time_str[1] == ':'){
                        for (int i = idx[column]; i > 0; i--){
                            time_str[i] = time_str[i - 1];
                        }
                        time_str[0] = '0';
                        idx[column]++;
                    }
                    if (time_str[4] == '\0'){
                        time_str[4] = time_str[3];
                        time_str[3] = '0';
                        idx[column]++;
                    }
                }
                break;
            }

        }

        // Xử lý các phím di chuyển và phím chức năng
        if (ch == UP || ch == 'A') {
            if (column > 0){
                column--;
            }
        } 
        else if (ch == DOWN || ch == 'B') {
            if (column < 2){
                column++;
            }
        } 
        else if (ch == ENTER) {
            if (! search_flight_id(flight_id)){
                // Nếu không tìm thấy mã chuyến bay, thông báo lỗi và reset lại trường mã
                Menu::display_flight_not_found();      
                Menu::display_edit_flight_schedule();   
                // memset(flight_id, 0, LEN_FLIGHT_ID);
                // idx[0] = 0;
                ch = '\0';
                continue;
            }
            // Hiện tại đang ở dòng cuối cùng
            Flight *temp;
            if (strlen(flight_id) == 0) { column = 0; continue; }
            if (!temp->valid_date(day, month, year)) { column = 1; continue; }
            if (!temp->valid_time(hour, minute)) { column = 2; continue; }

            new_date.day = day;
            new_date.month = month;
            new_date.year = year;
            new_time.hour = hour;
            new_time.minute = minute;
            if(is_completed(&new_date, &new_time)) {
                //in ra thông báo không được đặt;
                Menu::display_create_flight_error();
                column = 1;
                continue;
            }
            
            // Nhập liệu thành công
            update_flight(flight_id, new_date, new_time);
            Menu::display_success_update_flight();
            Menu::display_edit_flight_schedule();
            // Update thành công ngày, giờ chuyến bay
            // return;
        }
        else if (ch == ESC) {
            // Nếu nhấn ESC, thoát khỏi chế độ sửa
            return;
        }
    }
}
void Console::enter_flight_cancel() {
    if (count_flights() == 0) {
        Menu::display_empty_flight_list();
        return;
    }

    char flight_id[LEN_FLIGHT_ID] = "\0";
    char ch;
    int idx = 0;
    Flight *targetFlight = nullptr;

    Menu::display_cancel_flight();
    while (true) {
        Menu::gotoxy(59, 6);
        std::cout << flight_id;

        enter(flight_id, idx, LEN_FLIGHT_ID, ch,
            [&](char &c) {
                if (c >= 'a' && c <= 'z')
                    c -= 32;
                return ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
            });

    // Xử lý phím mũi tên
    #ifdef _WIN32
        if (ch == -32 || ch == 224) { // Phím mũi tên trên Windows có mã tiền tố
            _getch(); // Bỏ qua ký tự của phím mũi tên
            continue;
        }
    #else
        if (ch == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
            if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                _getch(); // Bỏ qua ký tự của phím mũi tên
                continue;
            }
        }
    #endif


        // Nếu nhấn ESC thì thoát luôn
        if (ch == ESC){
            return;
        }
            
        // Nếu nhấn ENTER, tiến hành kiểm tra và xử lý huỷ chuyến
        if (ch == ENTER) {
            if (!search_flight_id(flight_id)){
                // Nếu không tìm thấy mã chuyến bay, hiện thị thông báo và cho phép nhập lại
                Menu::display_flight_not_found();
                Menu::display_cancel_flight();
                ch = '\0';
                continue;
            // } else if (targetFlight->cur_status == status::completed) {
            //     Menu::display_cannot_cancel_flight();
            //     return;
            } else {
                cancel_flight(flight_id);
                Menu::display_cancel_flight();
                ch = '\0';
                // return;
            }
                
        }
    }
}

