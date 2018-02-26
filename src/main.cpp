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
        if( l_argv == "-server") {
            p_game.getNetwork()->start_sever();
            continue;
        }
        if( l_argv == "-client") {
            if( i+1 < (int)argc)
            {
                p_game.getNetwork()->start_client( argv[ i+1]);
                i++;
            }
                else
            {
                p_game.getNetwork()->start_client();
            }
            continue;
        }
    }
    p_game.Start();
    return 0;
}
