#include <iostream>
#include "../include/Menu.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <conio.h>

void Menu::gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x; // col
    coord.Y = y; // row
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
#else
#include <stdlib.h>
#include <unistd.h>

void Menu::gotoxy(int x, int y)
{
    std::cout << "\033[" << y << ";" << x << "H";
}
// Định nghĩa lại Sleep cho Linux/macOS
void Sleep(long long time)
{
    usleep(time * 1000); // usleep nhận tham số là micro-giây (1 mili-giây = 1000 micro-giây)
}
#endif

void Menu::display_navigation_instructions()
{
    std::cout << R"(

            Use UP/DOWN to navigate fields       
            Press ENTER to confirm input          
            Press TAB to go back                  

    )";
}
void Menu::display_list_instructions()
{
    std::cout << R"(
        
         [<] Previous Page    [>] Next Page    [ESC] Exit        Page:  |    

             [^] Move Up                     [v] Move Down        
        
    )";
}

void Menu::display_login_frame()
{
    system("cls");
    std::cout << R"(
                                        _________________________________________
                                       |                                         |
                                       |           WELCOME TO OUR AIRLINE        |
                                       |               WHO ARE YOU ?             |
                                       |_________________________________________|
                                       |                                         | 
                                       |                  USER                   |                       	  
                                       |_________________________________________|
                                       |                                         |  
                                       |                 MANAGER                 |      
                                       |_________________________________________|                                      
    )";
    // 📌 Hiển thị điều hướng trang
    gotoxy(35, 26);
    std::cout << "[<] Previous Page    [>] Next Page  ";
    gotoxy(35, 27);
    std::cout << "[^] Move Up          [v] Move Down";
    // show_navigation_instructions();
}

void Menu::display_manager_menu()
{
    system("cls");
    std::cout << R"(
                             ___________________________________________________________
                            |                                                           |
                            |                      MANAGER MENU                         |
                            |___________________________________________________________|
                            |                                                           |
                            |  [1] Update Plane List                                    |
                            |___________________________________________________________|
                            |                                                           |
                            |  [2] Update Flight List                                   |
                            |___________________________________________________________|
                            |                                                           |
                            |  [3] Flight Statistics (Sorted by Most Flights)           |
                            |___________________________________________________________|
                                                                                      
                            [ESC] Back                                 
                                                                    
                            
    )" << std::endl;
}
void Menu::display_flight_manager_menu()
{
    system("cls");
    std::cout << R"(
                             ___________________________________________________________
                            |                                                           |
                            |                   FLIGHT MANAGEMENT                       |
                            |___________________________________________________________|
                            |                                                           |
                            |  [1] Update Flights                                       |
                            |___________________________________________________________|
                            |                                                           |
                            |  [2] Print Passenger List for a Flight                    |
                            |___________________________________________________________|
                            |                                                           |
                            |  [3] View Available Seats for a Flight                    |
                            |___________________________________________________________|
                                                                                      
                                                     
                            [ESC] Back )";
}
void Menu::display_manage_plane()
{ // A
    system("cls");
    std::cout << R"(
                                        ________________________________________
                                       |                                        |
                                       |          PLANE MANAGEMENT              |
                                       |________________________________________|
                                       |                                        |
                                       |              Add Plane                 |
                                       |________________________________________|
                                       |                                        |
                                       |              Delete Plane              |
                                       |________________________________________|
                                       |                                        |
                                       |              Edit Plane Details        |
                                       |________________________________________|

    )";
}
void Menu::display_add_aircraft()
{
    system("cls");
    std::cout << R"(
                                        __________________________________________________________
                                       |                                                          |
                                       |                       ADD NEW AIRCRAFT                   |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Aircraft ID:                                            |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Aircraft Type:                                          |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Number of Seats (>=20):                                 |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Flights Performed:                                      |
                                       |__________________________________________________________|

    )";
}

void Menu::display_delete_aircraft()
{
    system("cls");
    std::cout << R"(
                                         ____________________________________________________
                                        |                                                    |
                                        |                DELETE AIRCRAFT                     |
                                        |____________________________________________________|
                                        |                                                    |
                                        |  Enter Aircraft ID to delete:                      |
                                        |____________________________________________________|
                                        
                                       
    )";
}
void Menu::display_aircraft_exist()
{
    system("cls");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |    AIRCRAFT ID ALREALDY EXISTS   |
                                            |__________________________________|
         )";
    Sleep(2000);
}
void Menu::display_empty_aircraft_list()
{
    system("cls");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |    THE AIRCRAFT LIST IS EMPTY    |
                                            |__________________________________|
         )";
    Sleep(2000);
}
void Menu::display_full_aircraft_list()
{
    system("cls");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |    THE AIRCRAFT LIST IS FULL     |
                                            |__________________________________|
         )";
    Sleep(2000);
}
void Menu::display_success_add_aircraft()
{
    system("cls");
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |    SUCCESSFULLY ADDED AIRCRAFT    |
                                            |___________________________________|
        )";
    Sleep(2000);
}
void Menu::display_success_delete_aircraft()
{
    system("cls");
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |   SUCCESSFULLY DELETED AIRCRAFT   |
                                            |___________________________________|
        )";
    Sleep(2000);
}
void Menu::display_success_update_aircraft()
{
    system("cls");
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |   SUCCESSFULLY UPDATED AIRCRAFT   |
                                            |___________________________________|
        )";
    Sleep(2000);
}
void Menu::display_aircraft_not_found()
{
    system("cls");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |        AIRCRAFT NOT FOUND        |
                                            |__________________________________|
                )";
    Sleep(2000);
}

void Menu::display_search_flight()
{
    system("cls");
    std::cout << R"(
                                         ____________________________________________________
                                        |                                                    |
                                        |                  SEARCH FLIGHT                     |
                                        |____________________________________________________|
                                        |                                                    |
                                        |  Enter Flight ID:                                  |
                                        |____________________________________________________|
                                        |                                                    |
                                        |  Enter Destination:                                |
                                        |____________________________________________________|
                                        |                                                    |
                                        |  Enter Dep Date & Time:                            |
                                        |____________________________________________________|
                                        
                                       
    )";
}
void Menu::display_edit_aircraft_details()
{
    system("cls");
    std::cout << R"(
                                        __________________________________________________________
                                       |                                                          |
                                       |                  EDIT PLANE DETAILS                      |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Enter Plane ID to edit:                                 |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Plane Type:                                             |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Number of Seats (>=20):                                 |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Flights Performed:                                      |
                                       |__________________________________________________________|

    )";
}
void Menu::display_manage_flights()
{ // B
    system("cls");
    std::cout << R"(
                                        ________________________________________
                                       |                                        |
                                       |            FLIGHT MANAGEMENT           |
                                       |________________________________________|
                                       |                                        |
                                       |     >     Create New Flight            |
                                       |________________________________________|
                                       |                                        |
                                       |        Edit Flight Departure Time      |
                                       |________________________________________|
                                       |                                        |
                                       |             Cancel Flight              |
                                       |________________________________________|

    )";
}
void Menu::display_create_new_flight()
{
    system("cls");
    std::cout << R"(
                                        ___________________________________________
                                       |                                           |
                                       |          CREATE NEW FLIGHT                |
                                       |___________________________________________|
                                       |                                           |
                                       |  Flight ID:                               |
                                       |___________________________________________|
                                       |                                           |
                                       |  Departure Date (dd/mm/yyyy):             |
                                       |___________________________________________|
                                       |                                           |  
                                       |  Departure Time (hh:mm):                  |
                                       |___________________________________________|
                                       |                                           |
                                       |  Destination:                             |
                                       |___________________________________________|
                                       |                                           |
                                       |  Flight Status (0: Canceled, 1: Ready):   |
                                       |___________________________________________|
    )";
}
void Menu::display_edit_flight_schedule()
{
    system("cls");
    std::cout << R"(
                                        _______________________________________________
                                       |                                               |
                                       |         EDIT FLIGHT SCHEDULE                  |
                                       |_______________________________________________|
                                       |                                               |
                                       |  Enter Flight ID:                             |
                                       |_______________________________________________|
                                       |                                               |
                                       |  New Departure Date (dd/mm/yyyy):             |
                                       |_______________________________________________|
                                       |                                               |  
                                       |  New Departure Time (hh:mm):                  |
                                       |_______________________________________________|
    )";
}

void Menu::display_enter_flight_ID()
{
    system("cls");
    std::cout << R"(
                                        ________________________________________
                                       |                                        |
                                       |            ENTER FLIGHT ID             |
                                       |________________________________________|
                                       |                                        |
                                       |  Enter Flight ID:                      |
                                       |________________________________________|
                                      
    )";
}

void Menu::display_enter_user_information()
{
    system("cls");
    std::cout << R"(
                                        ___________________________________________
                                       |                                           |
                                       |         ENTER USER INFORMATION            |
                                       |___________________________________________|
                                       |                                           |
                                       |  Enter Last Name:                         |
                                       |___________________________________________|
                                       |                                           |
                                       |  Enter First Name:                        |
                                       |___________________________________________|
                                       |                                           |
                                       |  Enter Gender (Male: 1, Female: 0):       |
                                       |___________________________________________|
                                       |                                           |
                                       |  Enter CMND:                              |
                                       |___________________________________________|

    )";
}

void Menu::display_identification_information()
{
    system("cls");
    std::cout << R"(
                                        ___________________________________________
                                       |                                           |
                                       |       IDENTIFICATION INFORMATION          |
                                       |___________________________________________|
                                       |                                           |
                                       |  Full Name:                               |
                                       |___________________________________________|
                                       |                                           |
                                       |  Gender:                                  |
                                       |___________________________________________|
                                       |                                           |
                                       |  ID Card Number:                          |
                                       |___________________________________________|

    )";
}

void Menu::display_book_ticket()
{ // C
    system("cls");
    std::cout << R"(
									    ______________________________________________________________________________
									   |                                                                              |
									   |                               BOOK A TICKET                                  |
									   |______________________________________________________________________________|
									   |                                                                              |
                                       |  Enter Flight Date (dd/mm/yyyy):                                             |
									   |______________________________________________________________________________|
									   |                                                                              |
									   |                               AVAILABLE FLIGHTS                              |
									   |______________________________________________________________________________|
									   |    Flight ID    |  Departure Time  |  Available Seats  |    Destination      |
									   |-----------------|------------------|-------------------|---------------------|
                                       |                 |                  |                   |                     |       
                                       |                 |                  |                   |                     |
                                       |                 |                  |                   |                     |
                                       |                 |                  |                   |                     |
                                       |                 |                  |                   |                     |
									   |_________________|__________________|___________________|_____________________|
									   |                                                                              |
                                       |  Enter Flight ID to book:                                                    |
									   |______________________________________________________________________________|
									   |                                                                              |
                                       |  Enter Passenger's National ID card number :                                 |
									   |______________________________________________________________________________|
									   )";
}

void Menu::display_cancel_ticket()
{ // D
    system("cls");
    std::cout << R"(
		
                                        ______________________________________________________________________________
									   |                                                                              |
									   |                              CANCEL TICKET                                   |
									   |______________________________________________________________________________|
									   |                                                                              |
                                       |  Enter Passenger's National ID card number:                                  |
									   |______________________________________________________________________________|
									   |                                                                              |
									   |                              BOOKED FLIGHTS                                  |
									   |______________________________________________________________________________|
									   |    Flight ID    |  Departure Time  |  Available Seats  |    Destination      |
									   |-----------------|------------------|-------------------|---------------------|
                                       |                 |                  |                   |                     |       
                                       |                 |                  |                   |                     |
                                       |                 |                  |                   |                     |
                                       |                 |                  |                   |                     |
                                       |                 |                  |                   |                     |
									   |_________________|__________________|___________________|_____________________|
									   |                                                                              |
                                       |  Enter Flight ID to cancel:                                                  |
									   |______________________________________________________________________________|
										)";
}

void Menu::display_passenger_list(int current_page, int max_pages)
{ // E
    system("cls");

    std::cout << R"(
                        ___________________________________________________________________________________________
                        |                                                                                          |
                        |                  PASSENGER LIST FOR FLIGHT FLIGHT ID:                                    |
                        |__________________________________________________________________________________________|
                        |   Plane ID:                           |   Destination:                                   |
                        |__________________________________________________________________________________________|
                        |   Departure Date:                     |   Time:                                          |
                        |__________________________________________________________________________________________|
                        |   Status:                                                                                |
                        |__________________________________________________________________________________________|
                        |  Last Name      |  First Name      |  CMND            |  Gender  |  Seat No.             |
                        |_________________|__________________|__________________|__________|_______________________|)";

    // 📌 Hiển thị điều hướng trang
    gotoxy(35, 26);
    std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    gotoxy(35, 27);
    std::cout << "[^] Move Up          [v] Move Down";
}

void Menu::display_available_flights()
{ // F
    system("cls");
    std::cout << R"(
			 _________________________________________________________
			|                                                         |
            |     AVAILABLE FLIGHTS ON             TO                 |
            |_________________________________________________________|
			|    Flight ID    |  Departure Time |   Available Seats   |
			|-----------------|-----------------|---------------------|
			|                 |                 |                     |
			|                 |                 |                     |
			|                 |                 |                     |
			|                 |                 |                     |
			|                 |                 |                     |
			|_________________|_________________|_____________________|
  )";
}
void Menu::display_available_tickets(int current_page, int max_pages)
{
    system("cls");
    std::cout << R"(
                        ___________________________________________________________________________________________
                        |                                                                                          |
                        |                 AVAILABLE TICKETS FOR FLIGHT ID:                                         |
                        |__________________________________________________________________________________________|
                        |   Plane ID:                           |   Destination:                                   |
                        |__________________________________________________________________________________________|
                        |   Departure Date:                     |   Time:                                          |
                        |__________________________________________________________________________________________|
                        |   Status:                                                                                |
                        |__________________________________________________________________________________________|
                        |   Seat Number   |                           Status                                       |
                        |_________________|________________________________________________________________________|)";

    // 📌 Hiển thị điều hướng trang
    gotoxy(35, 26);
    std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    gotoxy(35, 27);
    std::cout << "[^] Move Up          [v] Move Down";
}

void Menu::display_plane_statistics(int current_page, int max_pages)
{ // h;
    system("cls");
    // PLANE FLIGHT PERFORMANCE STATS
    std::cout << R"(
                         _____________________________________________________________________________________________
                        |                                                                                            |
                        |                         PLANE FLIGHT PERFORMANCE STATS                                     |
                        |____________________________________________________________________________________________|
                        |  Plane ID       |  Plane Type                                 |  Number Flights Performed  |
                        |_________________|_____________________________________________|____________________________|
									   )";
    // 📌 Hiển thị điều hướng trang
    gotoxy(35, 26);
    std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    gotoxy(35, 27);
    std::cout << "[^] Move Up          [v] Move Down";
}

void Menu::display_enter_flight_details()
{
    // system("cls");
    gotoxy(25,15);
    std::cout << R"(
                         _______________________________________________________________________________________
                        |                |  Date of Dep (dd/mm/yyyy):                                           |
                        |  SEARCH FLIGHT |______________________________________________________________________|
                        |                |  Destination (e.g., New York):                                       |
                        |________________|______________________________________________________________________|
        )";
}
void Menu::display_flight_list(int current_page, int max_pages)
{
    system("cls");
    std::cout << R"(
                         ______________________________________________________________________________________
                        |                                   FLIGHT LIST                                        |
                        |______________________________________________________________________________________|
                        | Flight ID      | Plane ID         | Dep Date & Time | Destination      | Status      |
                        |________________|__________________|_________________|__________________|_____________|
    )" << std::endl;

    // 📌 Hiển thị điều hướng trang
    gotoxy(35, 26);
    std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    gotoxy(35, 27);
    std::cout << "[^] Move Up          [v] Move Down   [TAB] FLight Management";
    
}
void Menu::display_plane_list(int current_page, int max_pages)
{
    system("cls");
    std ::cout << R"(
                         _____________________________________________________________________________________
                        |                                                                                     |
                        |                                      PLANE LIST                                     |
                        |_____________________________________________________________________________________|
                        |  Plane ID            |  Plane Type                              |  Number Of Seats  |
                        |______________________|__________________________________________|___________________|

           )";
    // 📌 Hiển thị điều hướng trang
    gotoxy(35, 26);
    std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    gotoxy(35, 27);
    std::cout << "[^] Move Up          [v] Move Down";
    // show_navigation_instructions();
}
void Menu::display_available_tickets_menu(int current_page, int max_pages)
{
    system("cls");
    std::cout << R"(
                         ____________________________________________________________________________________
                        |                                                                                    |
                        |                                 AVAILABLE TICKETS                                  |
                        |____________________________________________________________________________________|
                        |             Seat Number              |                     Status                  |
                        |______________________________________|_____________________________________________|

           )";
    // 📌 Hiển thị điều hướng trang
    gotoxy(35, 26);
    std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    gotoxy(35, 27);
    std::cout << "[^] Move Up          [v] Move Down";
}
void Menu :: display_not_found() {
    system("cls");
    gotoxy(120, 10);
    std ::cout << R"(
                                        __________________________________ 
                                       |                                  |
                                       |        No flight found!          |
                                       |                                  |
                                       |   Press any key to continue...   |
                                       |__________________________________|
                )";
    char ch = _getch();
    
    
}