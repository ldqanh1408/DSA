#pragma once
#include "Constants.h"
#include <cstring>
#include <fstream>
#include <iostream>
struct Ticket {
    unsigned int seat; // so ve la so cho ngoi
    char *CMND = nullptr;
    bool is_sold=false;


    Ticket();
    Ticket(unsigned int _seat, char *CMND);
    

};