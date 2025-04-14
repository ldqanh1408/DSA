#pragma once
#include "Plane.h"
#include "Flight.h"
#include "Passenger.h"
#include "AVL_TREE.h"
#include "Ticket.h"
#include "Menu.h"
#include "Constants.h"
#include <iostream>  // std::cout, std::endl
#include <cstring>   // strcmp
#include <conio.h>   // _getch
#include <iomanip>   // std::setw
#include <algorithm> // std::min
#include <vector>   // std::vector
struct Console
{
    Flight *list = nullptr; // Khai báo extern
    Passenger *input = nullptr;
    AVL_TREE manager;
    Plane *list_planes[MAX_PLANE];

    // void buy_ticket(Passenger input, Flight *booking) {
    //     //in ra danh sách vé còn trống
    //     Ticket new_ticket;

    // }
    void enter_plane_statistics();
    unsigned int count_flights();
    void start_program();
    void enter_manager_menu();
    void enter_plane_list();
    void enter_available_tickets(Flight *flight);
    void enter_user_information();
    void enter_available_flights();
    void enter_flight_manager_menu();
    void enter_passenger_list(Flight *flight);

    void count_passengers(Flight *flight, int &count);
    int *list_passengers(Flight *flight, int &n);

    void enter_flight_id();

    void add_plane(const Plane &other);
    void delete_plane(const char *plane_id);
    void update_plane(const Plane &other);
    bool search_plane_id(char *target);
    int get_plane_count();

    void enter_plane_information();
    void enter_plane_update();
    void enter_plane_delete();
    void enter_manage_plane();
    void merge_sort();

    void enter_flight_id_for_available_tickets();
    void display_available_tickets(Flight &flight, int current_page, int &max_pages);
    Flight *create_sample_flight();
};