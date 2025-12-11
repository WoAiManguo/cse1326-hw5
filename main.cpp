#include "game.hpp"
#include "fltkio.hpp"
#include "consoleio.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>

int main(){
    GameIO* io = NULL;
    char input[100];

    while (io == NULL) {
        std::cout << "Enter CONSOLE for consoleio. Enter FLTK for fltk GUI: ";
        if (!(std::cin >> input)) return 0;

        if (strcmp(input, "CONSOLE") == 0) {
            io = new ConsoleIO();
        } else if (strcmp(input, "FLTK") == 0) {
            io = new FltkIO();
        } else {
            std::cout << "Unrecognized option. Please try again." << std::endl;
        }
    }

    while (true) {
        Game checkers_game(io);
        checkers_game.run();
    }

    delete io;
    io = NULL;

    return 0;
}