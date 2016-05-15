#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QFormLayout>
#include <QtGui/QFont>

#include "SDL2/SDL.h"
#include "editor/editor.h"

Uint32 timerGameLoop(Uint32 interval, void *param) {
    //printf("timerGameLoop::NULL test\n");
    return(interval);
}

#define REFRESH_RATE 16 // ms

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // SDL Init
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "main::NULL Unable to initialize SDL:  %s\n",SDL_GetError());
        return 1;
    }

    SDL_TimerID l_gameloop = SDL_AddTimer( REFRESH_RATE, timerGameLoop, NULL);

    Editor l_editor;
    l_editor.resize( 1024, 800);
    l_editor.setWindowTitle("QSplitter");
    l_editor.show();

    int t_return = app.exec();
    SDL_RemoveTimer( l_gameloop);
    return t_return;
}
