#include "engine/engine.h"

int main(int argc, char *argv[]) {
    engine *p_engine = new engine();
    for( int i = 0; i < (int)argc; i++) {
        std::string l_argv = argv[i];
        // check
        if( l_argv == "-openvr") {
            p_engine->startVR();
            continue;
        }
        if( l_argv == "-server") {
            //p_engine->getNetwork()->start_sever();
            continue;
        }
        if( l_argv == "-client") {
            if( i+1 < (int)argc)
            {
                //p_engine->getNetwork()->start_client( argv[ i+1]);
                i++;
            }
                else
            {
                //p_engine->getNetwork()->start_client();
            }
            continue;
        }
    }

    p_engine->run();

    delete p_engine;
    return 0;
}
