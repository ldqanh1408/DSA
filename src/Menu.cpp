#include <iostream>
#include "../include/Menu.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <conio.h>

    void Menu::gotoxy(int x, int y){
    COORD coord;
    coord.X = x; // col
    coord.Y = y; // row
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
#else
    #include <stdlib.h>
    #include <unistd.h>  

    void Menu::gotoxy(int x, int y){
        std::cout << "\033[" << y + 1  << ";" << x + 1 << "H";

    }
    // Định nghĩa lại Sleep cho Linux/macOS
    void Sleep(long long time) {
        usleep(time * 1000); // usleep nhận tham số là micro-giây (1 mili-giây = 1000 micro-giây)
    }
#endif

void Menu::notification(){
    std::cout << "\n\n\t\t\t\t\t\t\t\t\t\tPress any key to continue...";
    int ch_temp = _getch();

    // Xử lý phím mũi tên
    #ifdef _WIN32
            if (ch_temp == 0 || ch_temp == 224) { // Phím mũi tên trên Windows có mã tiền tố
                _getch(); 
            }
    #else
            if (ch_temp == ESC) { // Trên macOS, phím mũi tên bắt đầu với ESC
                if (_getch() == '[') { // Kiểm tra ký tự tiếp theo
                   _getch(); // Lấy mã thực tế của phím
                }
            }
    #endif
}
void Menu::display_navigation_instructions()
{
    std::cout << R"(

            Use UP/DOWN to navigate fields       
            Press ENTER to confirm input          
            Press TAB to go back                  

    )";
}
void Menu::display_list_instructions(int current_page, int max_pages)
{
    std::cout << R"(
                                [<] Previous Page    [>] Next Page    [ESC] Exit     Page: )"
                                            << current_page << "|" << max_pages << R"(
                                [^] Move Up          [v] Move Down
)";
}
void Menu::display_list_instructions_tab(int current_page, int max_pages)
{
    std::cout << R"(
                                [<] Previous Page    [>] Next Page    [ESC] Exit     Page: )"
                                            << current_page << "|" << max_pages << R"(
                                [^] Move Up          [v] Move Down    [TAB] Show Functions
)";
}


void Menu::display_login_frame()
{
    system("clear");
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
    system("clear");
    std::cout << R"(
                             ___________________________________________________________
                            |                                                           |
                            |                   FLIGHT MANAGEMENT SYSTEM                |
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
                                                                                      
                            [TAB] Back to Main Menu                                  
                            [ESC] Exit System                                        
                            
    )" << std::endl;
}
void Menu::display_flight_manager_menu()
{
    system("clear");
    std::cout << R"(
                             ___________________________________________________________
                            |                                                           |
                            |                   FLIGHT MANAGEMENT SYSTEM                |
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
                                                                                      
                            [TAB] Back to Main Menu                                  
                            [ESC] Exit System        )";
}
void Menu::display_manage_plane()
{ // A
    system("clear");
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

                                       [^] Move Up          [v] Move Down      [ESC] Exit
    )";
    
}
void Menu::display_add_aircraft()
{
    system("clear");
    std::cout << R"(
                                        __________________________________________________________
                                       |                                                          |
                                       |                       ADD NEW PLANE                      |
                                       |__________________________________________________________|
                                       |                                                          |
                                       |  Plane ID:                                               |
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
    
                                       [^] Move Up          [v] Move Down      [ESC] Exit
    )";
}

void Menu::display_delete_aircraft()
{
    system("clear");
    std::cout << R"(
                                         ____________________________________________________
                                        |                                                    |
                                        |                   DELETE PLANE                     |
                                        |____________________________________________________|
                                        |                                                    |
                                        |  Enter Plane ID to delete:                         |
                                        |____________________________________________________|
                                        
                                        [ESC] Exit
                                       
    )";
}

void Menu::display_delete_passenger()
{
    system("cls");
    std::cout << R"(
                                         ____________________________________________________
                                        |                                                    |
                                        |                   DELETE PASSENGER                 |
                                        |____________________________________________________|
                                        |                                                    |
                                        |  Enter CMND to delete:                             |
                                        |____________________________________________________|
                                        
                                       
    )";
}
void Menu::display_aircraft_exist(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |     PLANE ID ALREADY EXISTS      |
                                            |__________________________________|
         )";
    notification();
}
void Menu::display_empty_aircraft_list(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |      THE PLANE LIST IS EMPTY     |
                                            |__________________________________|
         )";
    notification();
}
void Menu::display_full_aircraft_list(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |      THE PLANE LIST IS FULL      |
                                            |__________________________________|
         )";
    notification();
}
void Menu::display_success_add_aircraft(){
    system("clear");
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |      SUCCESSFULLY ADDED PLANE     |
                                            |___________________________________|
        )";
    notification();
}
void Menu::display_success_delete_aircraft(){
    system("clear");
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |     SUCCESSFULLY DELETED PLANE    |
                                            |___________________________________|
        )";
    notification();
}
void Menu::display_success_update_aircraft(){
    system("clear");
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |     SUCCESSFULLY UPDATED PLANE    |
                                            |___________________________________|
        )";
    notification();
}
void Menu::display_aircraft_not_found(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |        PLANE ID NOT FOUND        |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_flight_exist(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |     FLIGHT ID ALREADY EXISTS     |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_success_create_flight(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |   SUCCESSFULLY CREATED FLIGHT    |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_success_update_flight(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |   SUCCESSFULLY UPDATED FLIGHT    |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_success_cancel_flight(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |   SUCCESSFULLY CANCELLED FLIGHT  |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_already_cancelled_flight(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |     FLIGHT ALREADY CANCELLED     |
                                            |__________________________________|
                )";
    notification();
}

void Menu::display_empty_flight_list(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |     THE FLIGHT LIST IS EMPTY     |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_flight_not_found(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |         FLIGHT NOT FOUND         |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_cannot_cancel_flight(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |     THIS FLIGHT CANNOT CANCEL    |
                                            |__________________________________|
                )";
    notification();
}

void Menu::display_booking_success(){
    system("clear");
    std::cout << R"(
                                             __________________________________ 
                                            |                                  |
                                            |    TICKET BOOKED SUCCESSFULLY    |
                                            |__________________________________|
                )";
    notification();
}
void Menu::display_booking_error(){
    system("clear");
    std::cout << R"(
                                     __________________________________________________ 
                                    |                                                  |
                                    | YOU HAVE ALREADY BOOKED A TICKET FOR THIS FLIGHT |
                                    |__________________________________________________|
                )";
    notification();
}
void Menu::display_create_flight_error(){
    system("clear");
    std::cout << R"(
                                     __________________________________________________ 
                                    |                                                  |
                                    | INVALID TIME: CANNOT CREATE A FLIGHT IN THE PAST |
                                    |__________________________________________________|
                )";
    notification();
}
void Menu::display_user_exist(){
    system("clear");
    std::cout << R"(
                                     _____________________________________________________ 
                                    |                                                     |
                                    |   THIS CMND ALREADY EXISTS IN THE PASSENGER LIST    |
                                    |_____________________________________________________|
                )";
    notification();
}
void Menu::display_user_not_found(){
    system("clear");
    std::cout << R"(
                                     _____________________________________________________ 
                                    |                                                     |
                                    |   USER DOES NOT EXIST. PLEASE CREATE AN ACCOUNT     |
                                    |_____________________________________________________|
                )";
    notification();
}
void Menu::display_user_create_success(){
    system("clear");
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |   ACCOUNT CREATED SUCCESSFULLY    |
                                            |___________________________________|
                )";
    notification();
}
void Menu::display_flight_not_booked_by_user(){
    system("clear");
    std::cout << R"(
                                     ____________________________________________________ 
                                    |                                                    |
                                    |   NO PASSENGERS HAVE BOOKED SEATS ON THIS FLIHGT   |
                                    |____________________________________________________|
                )";
    notification();
}
void Menu::display_flight_sold_out(){
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |      THIS FLIGHT IS SOLD OUT      |
                                            |___________________________________|
                )";
    notification();
}
void Menu::display_flight_cancelled(){
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |      THIS FLIGHT IS CANCELLED     |
                                            |___________________________________|
                )";
    notification();
}
void Menu::display_flight_completed(){
    std::cout << R"(
                                             ___________________________________ 
                                            |                                   |
                                            |      THIS FLIGHT IS COMPLETED     |
                                            |___________________________________|
                )";
    notification();
}
void Menu::display_delete_plane_confirm(){
    system("clear");
    std::cout << R"(
                                     _____________________________________________________ 
                                    |                                                      |
                                    |      NO FLIGHTS USE THIS PLANE - DELETE ANYWAY ?     |
                                    |                                                      |
                                    |           YES                                        |
                                    |           NO                                         |
                                    |______________________________________________________|
                    )";

}
void Menu::display_delete_plane_error(){
    system("clear");
    std::cout << R"(
                                     _____________________________________________________ 
                                    |                                                      |
                                    |     PLANE IN USE BY FLIGHTS - CANNOT DELETE PLANE    |
                                    |                                                      |
                                    |______________________________________________________|
                    )";
    notification();
}

void Menu::display_cancel_flight_confirm(){
    system("clear");
    std::cout << R"(
                                             _______________________________________ 
                                            |                                       |
                                            |        CONFIRM CANCEL FLIGHT ?        |
                                            |                                       |
                                            |             YES                       |
                                            |             NO                        |
                                            |_______________________________________|
                    )";

}
void Menu::display_search_flight()
{
    system("clear");
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
                                        
                                        [^] Move Up          [v] Move Down      [ESC] Exit
                                       
    )";
}
void Menu::display_edit_aircraft_details()
{
    system("clear");
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

                                       [^] Move Up          [v] Move Down      [ESC] Exit
    )";
}
void Menu::display_manage_flights()
{ // B
    system("clear");
    std::cout << R"(
                                        ________________________________________
                                       |                                        |
                                       |            FLIGHT MANAGEMENT           |
                                       |________________________________________|
                                       |                                        |
                                       |        Create New Flight               |
                                       |________________________________________|
                                       |                                        |
                                       |        Edit Flight Departure Time      |
                                       |________________________________________|
                                       |                                        |
                                       |        Cancel Flight                   |
                                       |________________________________________|

                                       [^] Move Up          [v] Move Down      [ESC] Exit
    )";
}
void Menu::display_create_new_flight()
{
    system("clear");
    std::cout << R"(
                                        ___________________________________________
                                       |                                           |
                                       |          CREATE NEW FLIGHT                |
                                       |___________________________________________|
                                       |                                           |
                                       |  Flight ID:                               |
                                       |___________________________________________|
                                       |                                           |
                                       |  Plane ID:                                |
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
                    
                                       [^] Move Up          [v] Move Down      [ESC] Exit
    )";
}
void Menu::display_edit_flight_schedule()
{
    system("clear");
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

                                        [^] Move Up          [v] Move Down      [ESC] Exit
    )";
}

void Menu::display_cancel_flight()
{
    system("clear");
    std::cout << R"(
                                        ________________________________________
                                       |                                        |
                                       |              CANCEL FLIGHT             |
                                       |________________________________________|
                                       |                                        |
                                       |  Enter Flight ID:                      |
                                       |________________________________________|
                   
                                       [ESC] Exit
    )";
}
void Menu::display_enter_flight_ID()
{
    system("clear");
    std::cout << R"(
                                        ________________________________________
                                       |                                        |
                                       |            ENTER FLIGHT ID             |
                                       |________________________________________|
                                       |                                        |
                                       |  Enter Flight ID:                      |
                                       |________________________________________|
                                      
                                       [ESC] Exit
    )";
}
void Menu::display_enter_user_id_card(){
    system("clear");
    std::cout << R"(
                                        ___________________________________________
                                       |                                           |
                                       |         ENTER USER INFORMATION            |
                                       |___________________________________________|
                                       |                                           |
                                       |  Enter CMND:                              |
                                       |___________________________________________|

                                       [ESC] Exit
    )";
}
void Menu::display_enter_user_information()
{
    system("clear");
    std::cout << R"(
                                        ___________________________________________
                                       |                                           |
                                       |         ENTER USER INFORMATION            |
                                       |___________________________________________|
                                       |                                           |
                                       |  Enter CMND:                              |
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

                                       [^] Move Up          [v] Move Down      [ESC] Exit
    )";
}

void Menu::display_identification_information()
{
    system("clear");
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

                                       [^] Move Up          [v] Move Down      [ESC] Exit
    )";
}

void Menu::display_book_ticket()
{ // C
    system("clear");
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
    system("clear");
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

void Menu::display_passenger_list()
{ // E
    system("clear");

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
                        |_________________|__________________|__________________|__________|_______________________|
                        










                    
)";


    // 📌 Hiển thị điều hướng trang

}

void Menu::display_available_flights()
{ // F
    system("clear");
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
void Menu::display_available_tickets() {
    system("clear");
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
                        |_________________|________________________________________________________________________|










                    
               
)";


    // 📌 Hiển thị điều hướng trang
    // gotoxy(35, 26);
    // std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    // gotoxy(35, 27);
    // std::cout << "[^] Move Up          [v] Move Down";
}

void Menu::display_plane_statistics()
{ // h;
    system("clear");
    //PLANE FLIGHT PERFORMANCE STATS
    std::cout << R"(
                         _____________________________________________________________________________________________
                        |                                                                                            |
                        |                         PLANE FLIGHT PERFORMANCE STATS                                     |
                        |____________________________________________________________________________________________|
                        |  Plane ID       |  Plane Type                                 |  Number Flights Performed  |
                        |_________________|_____________________________________________|____________________________|










                    

									   )";       
    // 📌 Hiển thị điều hướng trang
    // gotoxy(35, 26);
    // std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    // gotoxy(35, 27);
    // std::cout << "[^] Move Up          [v] Move Down";
}

void Menu::display_enter_flight_details()
{
    // system("clear");
    std::cout << R"(
                         _______________________________________________________________________________________
                        |                |  Date of Dep (dd/mm/yyyy):                                           |
                        |  SEARCH FLIGHT |______________________________________________________________________|
                        |                |  Destination (e.g., New York):                                       |
                        |________________|______________________________________________________________________|
        )";
}
void Menu::display_flight_list() {
    system("clear");
    std::cout << R"(
                         ______________________________________________________________________________________
                        |                                                                                      |
                        |                                   FLIGHT LIST                                        |
                        |______________________________________________________________________________________|
                        | Flight ID      | Plane ID         | Dep Date & Time | Destination      | Status      |
                        |________________|__________________|_________________|__________________|_____________|










    )" << std::endl;    
    
    // show_
    // 📌 Hiển thị điều hướng trang
    // gotoxy(35, 26);
    // std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    // gotoxy(35, 27);
    // std::cout << "[^] Move Up          [v] Move Down";
    // show_navigation_instructions();
}
void Menu::display_plane_list() {
    system("clear");
    std ::cout << R"(
                         _____________________________________________________________________________________
                        |                                                                                     |
                        |                                      PLANE LIST                                     |
                        |_____________________________________________________________________________________|
                        |  Plane ID            |  Plane Type                              |  Number Of Seats  |
                        |______________________|__________________________________________|___________________|











                    

           )";
    // 📌 Hiển thị điều hướng trang
    // gotoxy(35, 26);
    // std::cout << "[<] Previous Page    [>] Next Page    [ESC] Exit     Page: " << current_page << "|" << max_pages;
    // gotoxy(35, 27);
    // std::cout << "[^] Move Up          [v] Move Down";
    // show_navigation_instructions();
}

void Menu::display_the_tickets_of_user() {
    system("clear");
    std::cout << R"(
                         ______________________________________________________________________________________
                        |                                                                                      |
                        |                               The Tickets Of Youuuu!!!                               |
                        |______________________________________________________________________________________|
                        | Flight ID      | Plane ID         | Dep Date & Time | Destination      | Status      |
                        |________________|__________________|_________________|__________________|_____________|










    )" << std::endl;    
}