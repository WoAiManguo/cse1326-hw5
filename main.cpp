#include "game.hpp"
#include "fltkio.hpp"
#include "consoleio.hpp"
#include <cstdio>
#include <cstdlib>

enum IOoption{
    CONSOLE,
    FLTK
};

int main(){
    char* input;
    scanf("%s", &input);

    printf("Enter CONSOLE for consoleio. Enter FLTK for fltkio: ");

    switch(input){
        case CONSOLE: GameIO* io = new ConsoleIO(); break;
        case FLTK: GameIO* io = new FltkIO(); break;
        case default: printf("Unrecognized option. Please try again: ");
    }
    

    Game checkers_game(io);

    checkers_game.run();

    delete io;
    io = NULL;

    return 0;
}