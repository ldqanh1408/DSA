#include "../include/Constants.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>

    char _getch() {
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif
