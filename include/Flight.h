#pragma once
#include "Ticket.h"
#include "Constants.h"
#include <cstring>

struct Flight {
    char flight_code[LEN_FLIGHT_CODE];
    char departure[LEN_DEPARTURE];
    char flight_number[LEN_FLIGHT_CODE];
    enum struct Status {
        cancelled = 0,    // Hủy chuyến
        available = 1,    // Còn vé
        sold_out = 2,      // Hết vé
        completed = 3     // Hoàn tất
    } cur_status;

    Ticket **tickets; // khi khởi tạo chuyến bay list = new Ticket*[số chỗ]
    unsigned int number_of_seats;

    // danh sách chuyến bay trỏ đến nhau
    Flight *next;
    Flight();
    bool is_exist(char *CMND);
};