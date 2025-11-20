#include "game.hpp"
#include "consoleio.hpp"
#include <cstdio>
#include <cstdlib>

int main(){
    GameIO* io = new ConsoleIO();

    Game checkers_game(io);

    checkers_game.run();

    delete io;
    io = NULL;

    return 0;
}