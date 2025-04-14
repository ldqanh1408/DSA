#include <iostream>
#include "../include/Console.h"
#include <cstdlib>
#include <ctime>
#include <cstring>


void generateRandomFlights(Flight *&test) {
    
    test = new Flight;

    const char* destinations[] = {
        "Hanoi", "HoChiMinh", "DaNang", "Paris", "London",
        "Tokyo", "Seoul", "Singapore", "Bangkok", "Sydney"
    };
    int num_dest = sizeof(destinations) / sizeof(destinations[0]);

    snprintf(test->flight_id, LEN_FLIGHT_ID, "FL%03d", rand() % 900 + 100);

    if (test->plane_id) {
        delete[] test->plane_id;
        test->plane_id = nullptr;
    }
    
    test->plane_id = new char[LEN_PLANE_ID];
    snprintf(test->plane_id, LEN_PLANE_ID, "PL%03d", rand() % 900 + 100);


    // Random Destination
    snprintf(test->destination, LEN_DESTINATION, "%s", destinations[rand() % num_dest]);

    // Random Date: 1-28/1-12/2025
    test->date_dep = {
        static_cast<unsigned short>(rand() % 28 + 1),
        static_cast<unsigned short>(rand() % 12 + 1),
        2025
    };

    // Random Time: 0-23h, 0-59m
    test->time_dep = {
        static_cast<unsigned short>(rand() % 24),
        static_cast<unsigned short>(rand() % 60)
    };

    // Random Seats: 50-200
    test->total_seats = new unsigned int(rand() % 151 + 50);
    test->tickets = new Ticket[*test->total_seats];

    // Random Status
    switch (rand() % 4) {
    case 0:
        test->cur_status = status::cancelled;
        break;
    case 1:
        test->cur_status = status::available;
        break;
    case 2:
        test->cur_status = status::sold_out;
        break;
    case 3:
        test->cur_status = status::completed;
        break;
    }

}
void generateRandomPlane(Plane *&plane) {
    // Cấp phát bộ nhớ cho đối tượng Plane mới
    plane = new Plane;

    // Tạo mã máy bay với định dạng "PLxxx"
    snprintf(plane->plane_id, LEN_FLIGHT_ID, "PL%03d", rand() % 900 + 100);

    // Danh sách các loại máy bay mẫu
    const char* planeTypes[] = {
        "Boeing 737",
        "Airbus A320",
        "Embraer E190",
        "Boeing 777",
        "Airbus A380"
    };
    int num_types = sizeof(planeTypes) / sizeof(planeTypes[0]);

    // Chọn ngẫu nhiên 1 loại máy bay từ danh sách và copy vào plane_type
    snprintf(plane->plane_type, LEN_PLANE_TYPE, "%s", planeTypes[rand() % num_types]);

    // Xác định số chỗ ngồi ngẫu nhiên, ví dụ trong khoảng 100 đến 300
    plane->number_of_seats = rand() % 201 + 100;
    plane->number_flights_performed = rand() % 201 + 100;
}


int main() {
    srand(time(NULL)); // Khởi tạo seed ngẫu nhiên
    Console test;
    // for(int i = 0; i < 55; ++i) {
        
    //     generateRandomPlane(test.list_planes[i]);
        
    // }
    // for(int i = 55; i < MAX_PLANE; ++i) {
    //     test.list_planes[i] = nullptr;
    // }
    // for (int i = 0; i < 55; ++i) {
    //     Flight *new_flight;
    //     generateRandomFlights(new_flight);
        

    //     // Gắn vào danh sách liên kết
    //     if (test.list == nullptr) {
    //         test.list = new_flight;
    //     } else {
    //         new_flight->next = test.list;
    //         test.list = new_flight;
    //     }
    // }

    // while(true) {
    //     test.start_program();
    // }
    // test.create_sample_flight();
    // test.enter_flight_id_for_available_tickets();
    Console console;
    Flight *sample = console.create_sample_flight(); // 🛫 chuyến bay mẫu

    console.list = sample; // ❗️ Gán vào danh sách chính
    console.enter_available_tickets_by_ID();

    return 0;
}


