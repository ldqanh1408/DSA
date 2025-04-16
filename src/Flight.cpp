#include "../include/Flight.h"


Flight::Flight() : next(nullptr) {}

bool Flight::valid_user(char *CMND) {
    for(int i = 0; i < *total_seats; ++i) {
        if(tickets[i].CMND != nullptr && strcmp(tickets[i].CMND, CMND) == 0) return false;
    }
    return true;
}
bool Flight::valid_date(int day, int month, int year)
{
    if (year < 1)
        return false;

    if (month < 1 || month > 12)
        return false;

    // số ngày của từng tháng
    int days_in_month[] = {31, 28, 31, 30, 31, 30,
                           31, 31, 30, 31, 30, 31};

    // năm nhuận
    bool is_leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (is_leap && month == 2)
        days_in_month[1] = 29;

    // kiểm tra ngày
    if (day < 1 || day > days_in_month[month - 1])
        return false;

    return true;
}