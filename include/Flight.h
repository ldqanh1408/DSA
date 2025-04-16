#pragma once
#include "Ticket.h"
#include "Constants.h"
#include <cstring>
#include <fstream>
struct date_departure {
    int day, month, year;

    // Overload for std::ofstream
    friend std::ofstream& operator<<(std::ofstream &out, const date_departure &x) {
        if (x.day < 10) out << "0";
        out << x.day << "/";
        if (x.month < 10) out << "0";
        out << x.month << "/";
        out << x.year;
        return out;
    }

    // Overload for std::ostream (e.g., std::cout)
    friend std::ostream& operator<<(std::ostream &out, const date_departure &x) {
        if (x.day < 10) out << "0";
        out << x.day << "/";
        if (x.month < 10) out << "0";
        out << x.month << "/";
        out << x.year;
        return out;
    }
};
struct time_departure {
    int hour, minute;

    // Overload for std::ofstream
    friend std::ofstream& operator<<(std::ofstream &out, const time_departure &t) {
        if (t.hour < 10) out << "0";
        out << t.hour << ":";
        if (t.minute < 10) out << "0";
        out << t.minute;
        return out;
    }
    friend std::ostream& operator<<(std::ostream &out, const time_departure &t) {
        if (t.hour < 10) out << "0";
        out << t.hour << ":";
        if (t.minute < 10) out << "0";
        out << t.minute;
        return out;
    }
};

enum struct status {
    cancelled = 0,    // Hủy chuyến
    available = 1,    // Còn vé
    sold_out = 2,      // Hết vé
    completed = 3     // Hoàn tất
};

struct Flight {
    char flight_id[LEN_FLIGHT_ID];
    char destination[LEN_DESTINATION]; 
    char *plane_id = nullptr;

    date_departure date_dep;
    time_departure time_dep;
    
    status cur_status;

    Ticket *tickets; // khi khởi tạo chuyến bay list = new Ticket[số chỗ]
    unsigned int *total_seats;

    // danh sách chuyến bay trỏ đến nhau

    Flight *next;

    Flight();

    bool valid_user(char *CMND);
    bool valid_date(int day, int month, int year);
};
