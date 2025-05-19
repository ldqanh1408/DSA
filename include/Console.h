#pragma once
#include "Plane.h"
#include "Flight.h"
#include "Passenger.h"
#include "AVL_TREE.h"
#include "Ticket.h"
#include "Menu.h"
#include "Constants.h"
#include <fstream>
#include <filesystem>
// #include <windows.h>

namespace fs = std::filesystem;

struct Console {
    Flight* list = nullptr;    // Khai báo extern
    Passenger* input = nullptr;
    AVL_TREE manager = AVL_TREE();
    Plane *list_planes[MAX_PLANE];

    // void buy_ticket(Passenger input, Flight *booking) {
    //     //in ra danh sách vé còn trống
    //     Ticket new_ticket;
        
    // }

    void processing();

    
    void load_planes_from_folder();
    
    void load_passengers_from_folder();

    void load_flights_from_folder();
    void load_flights_before_view();

    bool is_completed(date_departure *x, time_departure *y);


    void enter_plane_statistics();
    unsigned int count_flights();
    void start_program();
    void enter_manager_menu();
    void enter_plane_list();
    void enter_available_tickets(Flight *flight, int choice);
    void enter_user_information();
    void enter_available_flights(int choice);
    void enter_flight_manager_menu();
    void enter_passenger_list(Flight *flight);

    void enter_delete_passenger(Flight *flight);
    // void enter_the_tickets_of_you() {
        
    // }
    // void enter_available_tickets(Flight *flight);
    void enter_manage_flights();
    void count_passengers(Flight *flight, int &count);
    int *list_passengers(Flight *flight, int &n);


    void enter_flight_id(int choice);
    Flight* find_info_next(Flight *cur, char *date_dep, char *destination);

    void add_plane(const Plane &other);
    void delete_plane(const char *plane_id);
    void update_plane(const Plane &other);
    bool search_plane_id(char *target);
    int get_plane_count();

    void merge_sort();
    void enter_manage_plane();
    void enter_plane_information();
    void enter_plane_update();
    void enter_plane_delete();
    
    int get_plane_by_id(const char* plane_id);
    bool search_flight_id(const char *flight_id);
    //Handle flight
    void create_flight(Flight &other);
    void update_flight(const char *flight_id, const date_departure &new_date, const time_departure &new_time);
    void cancel_flight(const char *flight_id);

    void enter_flight_information();
    void enter_flight_update();
    void enter_flight_cancel();


};



