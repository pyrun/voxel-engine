#include "game/game.h"

int main(int argc, char *argv[]) {
    game p_game;
    for( int i = 0; i < (int)argc; i++) {
        std::string l_argv = argv[i];
        // check
        if( l_argv == "-openvr") {
            p_game.startVR();
            continue;
        }
    }
    p_game.Start();
    return 0;
}
