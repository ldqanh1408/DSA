#pragma once

#include <iostream>
#include "Constants.h"

namespace Menu {
    void notification();
    void gotoxy(int x, int y);
    void display_flight_manager_menu(); 
    void display_navigation_instructions();
    void display_list_instructions(int current_page, int max_pages);
    void display_list_instructions_tab(int current_page, int max_pages);
    void display_login_frame();
    void display_manager_menu();

    // Aircraft Management

    void display_manage_plane();
    void display_add_aircraft();
    void display_delete_aircraft();
    void display_edit_aircraft_details();
    void display_plane_list();

    // Plane notifications
    void display_empty_aircraft_list();
    void display_aircraft_exist();
    void display_full_aircraft_list();
    void display_success_add_aircraft();
    void display_success_delete_aircraft();
    void display_success_update_aircraft();
    void display_aircraft_not_found();
    void display_delete_plane_confirm();
    void display_delete_plane_error();

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
    void display_already_cancelled_flight();
    void display_create_flight_error();
    void display_cancel_flight_confirm();

    // Ticket Management
    void display_book_ticket();
    void display_cancel_ticket();

    // Passenger Information
    void display_passenger_list();

    void display_delete_passenger();

    // Flight Queries
    void display_available_flights();
    void display_available_tickets();
    void display_plane_statistics();
    void display_flight_list();

        // User Information
    void display_enter_user_information();
    void display_enter_user_id_card();
    void display_identification_information();
    void display_enter_flight_details();
    void display_the_tickets_of_user();
    void display_booking_success();
    void display_booking_error();
    void display_user_exist();
    void display_user_not_found();
    void display_user_create_success();

    void display_flight_not_booked_by_user();
    void display_flight_sold_out();
    void display_flight_cancelled();
    void display_flight_completed();
}



