#pragma once

#include <iostream>

namespace Menu {
    void gotoxy(int x, int y);
    void display_flight_manager_menu(); 
    void display_navigation_instructions();
    void display_list_instructions();
    void display_login_frame();
    void display_manager_menu();

    // Aircraft Management

    void display_manage_plane();
    void display_add_aircraft();
    void display_delete_aircraft();
    void display_edit_aircraft_details();
    void display_plane_list(int current_page, int max_pages);

    // Plane notifications
    void display_empty_aircraft_list();
    void display_aircraft_exist();
    void display_full_aircraft_list();
    void display_success_add_aircraft();
    void display_success_delete_aircraft();
    void display_success_update_aircraft();
    void display_aircraft_not_found();

        // Flight Management
    void display_manage_flights();
    void display_search_flight();
    void display_create_new_flight();
    void display_edit_flight_schedule();
    void display_enter_flight_ID();
    void display_cancel_flight();

    // Flight notifications
    void display_flight_exist();
    void display_success_create_flight();
    void display_success_update_flight();
    void display_success_cancel_flight();
    void display_empty_flight_list();
    void display_flight_not_found();
    void display_cannot_cancel_flight();

    // Ticket Management
    void display_book_ticket();
    void display_cancel_ticket();

    // Passenger Information
    void display_passenger_list(int current_page, int max_pages);

    // Flight Queries
    void display_available_flights();
    void display_available_tickets(int current_page, int max_pages);
    void display_plane_statistics(int current_page, int max_pages);
    void display_flight_list(int current_page, int max_pages);

        // User Information
    void display_enter_user_information();
    void display_identification_information();
    void display_enter_flight_details();
}



