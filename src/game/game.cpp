#include "game.h"
#include "../graphic/glerror.h"
#include "../system/timer.h"
#include <stdio.h>

std::string NumberToString( double Number) {
    /*std::ostringstream ss;
    ss << Number;
    return ss.str();*/
    char buffer[255];
    sprintf( buffer, "%0.2f", Number);
    std::string temp = buffer;
    return temp;
}

Game::Game() {
    p_graphic = new graphic( 1024, 800);

    // Runnig
    p_isRunnig = true;
    p_blocklist = new BlockList("objects");

    // Framenrate setzten
    framenrate.Set( 200);
    p_blocklist->Draw( p_graphic);
    p_world = NULL;
    p_world = new World( "tileset.png", p_blocklist);

    p_gui = new Gui;

    p_vboCursor = NULL;
}

Game::~Game() {
    glDeleteBuffers(1, &p_vboCursor);
    p_vboCursor = NULL;
    delete p_world;
    delete p_blocklist;
    delete p_gui;
    delete p_graphic;
}

void Game::viewCurrentBlock( int view_width) {
    float depth;

    // Voxel Anzeigen
    glReadPixels( p_graphic->getWidth() / 2, p_graphic->getHeight() / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    glm::vec4 viewport = glm::vec4(0, 0, p_graphic->getWidth(), p_graphic->getHeight());
    glm::vec3 wincoord = glm::vec3(p_graphic->getWidth() / 2, p_graphic->getHeight() / 2, depth);
    glm::vec3 objcoord = glm::unProject(wincoord, p_graphic->getCamera()->GetView(), p_graphic->getCamera()->GetProjection(), viewport);

    glm::vec3 testpos = p_graphic->getCamera()->GetPos();
    glm::vec3 prevpos = p_graphic->getCamera()->GetPos();

    // wo hin man sieht den block finden
    int mx, my, mz;
    for(int i = 0; i < view_width/2; i++) {
        // Advance from our currect position to the direction we are looking at, in small steps
        prevpos = testpos;
        testpos += p_graphic->getCamera()->GetForward() * 0.02f;

        // hack die komma zahl ab z.B. 13,4 -> 13,0
        mx = floorf(testpos.x/CHUNK_SCALE);
        my = floorf(testpos.y/CHUNK_SCALE);
        mz = floorf(testpos.z/CHUNK_SCALE);

        // falls wir ein block finden das kein "Air" ist dann sind wir fertig
        Tile *tile = p_world->GetTile( mx, my, mz);
        if( !tile )
            continue;

        int px = floorf(prevpos.x/CHUNK_SCALE);
        int py = floorf(prevpos.y/CHUNK_SCALE);
        int pz = floorf(prevpos.z/CHUNK_SCALE);

        int face;
        // Welche Seite wird angeklickt
        if(px > mx)
            face = 0;
        else if(px < mx)
            face = 3;
        else if(py > my)
            face = 1;
        else if(py < my)
            face = 4;
        else if(pz > mz)
            face = 2;
        else if(pz < mz)
            face = 5;

        // Wenn es leer ist wird es gesetzt
        if( tile->ID && p_input.Map.Place && !p_input.MapOld.Place ) {
            int mX, mY, mZ;
            mX = mx;
            mY = my;
            mZ = mz;
            if(face == 0)
                mX++;
            if(face == 3)
                mX--;
            if(face == 1)
                mY++;
            if(face == 4)
                mY--;
            if(face == 2)
                mZ++;
            if(face == 5)
                mZ--;

            if( !p_world->GetTile( mX, mY, mZ)) {
                Chunk *tmp = p_world->getChunkWithPos( mX, mY, mZ);
                if( tmp)
                    p_world->SetTile( tmp, mX, mY, mZ, p_blocklist->GetBlockID( "water")->getID());
                else
                    printf( "Game::ViewCurrentBlock Block nicht vorhanden wo man es setzen möchte\n");
            }

            printf( "Game::ViewCurrentBlock Set: %d %d %d %d\n", mx, my, mz, tile->ID);
            break;
        }
        if( tile->ID && p_input.Map.Destory && !p_input.MapOld.Destory) {
            Chunk *tmp = p_world->getChunkWithPos( mx, my, mz);
            if( tmp)
                p_world->SetTile( tmp, mx, my, mz, EMPTY_BLOCK_ID);
            else
                printf( "Game::ViewCurrentBlock Chunk nicht vorhanden\n");
            break;
        }

        // tile
        if( tile->ID) {
            drawBox( glm::vec3( mx, my, mz)*glm::vec3( CHUNK_SCALE));
            break;
        }
    }
}

void Game::viewCross() {
    std::vector<glm::vec4> l_vertices;
    l_vertices.resize(4);
    l_vertices[0] = glm::vec4( -0.05, 0, 0, 14);
    l_vertices[1] = glm::vec4( +0.05, 0, 0, 14);
    l_vertices[2] = glm::vec4( 0, -0.05, 0, 14);
    l_vertices[3] = glm::vec4( 0, +0.05, 0, 14);

    if( p_vboCursor == NULL)
        glGenBuffers(1, &p_vboCursor);

    Transform f_form;

    float p_width = p_graphic->getWidth();
    float p_hight = p_graphic->getHeight();
    glm::mat4 one = glm::ortho( (float)-1, (float)1, (float)-1, (float)1);

    // Shader einstellen
    p_graphic->getVertexShader()->BindArray( p_vboCursor, 0, GL_FLOAT, 4);
    p_graphic->getVertexShader()->Bind();// Shader
    p_graphic->getVertexShader()->EnableVertexArray( 0);
    p_graphic->getVertexShader()->updateWithout( &f_form, one); //p_graphic->getCamera()->getViewProjection());

    // Vbo übertragen
    glBindBuffer(GL_ARRAY_BUFFER, p_vboCursor);
    glBufferData(GL_ARRAY_BUFFER, l_vertices.size() * sizeof(glm::vec4), &l_vertices[0], GL_DYNAMIC_DRAW);

    glDrawArrays( GL_LINES, 0, l_vertices.size());

}

void Game::render( glm::mat4 viewProjection) {
    // chunks zeichnen
    p_world->draw( p_graphic, &p_config, viewProjection);

    //obj->draw( p_graphic->getObjectShader(), p_graphic->getCamera());
    //obj2->draw( p_graphic->getObjectShader(), p_graphic->getCamera());


    // View Cross
    //viewCross();

    // Debug
    //DrawBox( 1, 1, 1);

    //p_gui->Draw( p_graphic);
    // Block anzeigen was in der Sichtweite ist
    //if( p_world)
    //    viewCurrentBlock( 275); // 275 = 2,75Meter
}

void Game::Start() {
    p_config.SetTransparency( true);

    Object *obj = new Object;
    obj->Init();
    Object *obj2 = new Object;
    obj2->Init();

    p_openvr = new openvr();

    if( p_world)
        for( int i = 0; i < 15; i++)
            for( int x = 0; x < 15; x++)
                for( int z = -3; z < 3; z++)
                    p_world->addChunk( glm::tvec3<int>( i, z, x) );
    //p_world->addChunk( glm::tvec3<int>( 0, -1, 0) );


    Timer l_timer;
    int number = 0;

    glm::mat4 l_mvp;
    while( p_isRunnig) { // Runniz
        l_timer.Start();

        p_input.Reset();
        p_isRunnig = p_input.Handle( p_graphic->getWidth(), p_graphic->getHeight(), p_graphic->getWindow());
        // Input einsehen
//        float Speed = 0.1f * framenrate.getMSframe() *framenrate.getLimit()/ 1000.0f;
        float Speed = 0.1f;
        //printf( "%f %f \n", Speed, framenrate.getMSframe());
        Camera *cam = p_graphic->getCamera();
        cam->horizontalAngle ( -p_input.Map.MousePos.x * 2);
        cam->verticalAngle  ( p_input.Map.MousePos.y * 2);

        if( p_input.Map.Inventory ) {
            if( p_world)
                p_world->addChunk( { number++, -1, 0});
        }

        if( p_input.Map.Up )
            cam->MoveForwardCross( Speed);
        if( p_input.Map.Down )
            cam->MoveForwardCross(-Speed);
        if( p_input.Map.Right )
            cam->MoveRight(-Speed);
        if( p_input.Map.Left )
            cam->MoveRight( Speed);
        if( p_input.Map.Jump )
            cam->MoveUp( Speed);
        if( p_input.Map.Shift )
            cam->MoveUp( -Speed);
        if( p_input.Map.Inventory && !p_input.MapOld.Inventory) {
            /*if( p_config.GetSupersampling()) {

                p_config.SetSupersampling( false);

                printf( "Game::Start Supersampling deactive\n");
            } else {
                printf( "Game::Start Supersampling active\n");
                p_config.SetSupersampling( true);
            }*/
            cam->zoom( -1.5);
        }
        if( !p_input.Map.Inventory && p_input.MapOld.Inventory) {
            cam->zoom( 1.5);
        }

        if( p_input.getResize())
            p_graphic->resizeWindow( p_input.getResizeW(), p_input.getResizeH());

        // Framenrate anfangen zu zählen
        framenrate.StartCount();

		// World process
		if( p_world)
            p_world->process();


        /// Render
        p_openvr->renderForLeftEye();
        l_mvp = p_openvr->getViewProjectionMatrixLeft();
        p_openvr->renderModels( l_mvp);
        render( l_mvp);
        p_openvr->renderEndLeftEye();

        p_openvr->renderForRightEye();
        l_mvp = p_openvr->getViewProjectionMatrixRight();
        p_openvr->renderModels( l_mvp);
        render( l_mvp);
        p_openvr->renderEndRightEye();

        l_timer.Start();
        p_openvr->renderFrame();


        p_graphic->getDisplay()->clear();
        l_mvp = l_mvp;// p_graphic->getCamera()->getViewProjection();
        //
        render( l_mvp);

        // Swap die Buffer um keine Renderfehler zu bekommen
        p_graphic->getDisplay()->swapBuffers();

        // fehler anzeigen -> schleife eine meldung bedeutet ich habe verkackt
        GLenum error =  glGetError();
        if(error) {
            std::cout << error << std::endl;
        }

        // Titel setzten
        Title = "FPS_" + NumberToString( framenrate.getFrameratePrecisely() );
        Title = Title + " " + NumberToString( (double)l_timer.GetTicks()) + "ms";
        Title = Title + " X_" + NumberToString( cam->GetPos().x) + " Y_" + NumberToString( cam->GetPos().y) + " Z_" + NumberToString( cam->GetPos().z );
        if( p_world)
            Title = Title + " Chunks_" + NumberToString( (double)p_world->GetAmountChunks());
        p_graphic->getDisplay()->setTitle( Title);

        // Framenrate begrenzen
        framenrate.CalcDelay();

    }
    delete obj;
}

void Game::drawBox( glm::vec3 pos) {
    std::vector<block_data> t_box;

    // Chunk Vbo Data Struct
    t_box.resize( 24 );

    // x-y side
    t_box[0] = block_data(0, 0, 0, 14);
    t_box[1] = block_data(1, 0, 0, 14);
    t_box[2] = block_data(0, 0, 0, 14);
    t_box[3] = block_data(0, 1, 0, 14);
    t_box[4] = block_data(1, 0, 0, 14);
    t_box[5] = block_data(1, 1, 0, 14);
    t_box[6] = block_data(1, 1, 0, 14);
    t_box[7] = block_data(0, 1, 0, 14);
    // x-y & z+1
    t_box[8] = block_data(0, 0, 1, 14);
    t_box[9] = block_data(1, 0, 1, 14);
    t_box[10] = block_data(0, 0, 1, 14);
    t_box[11] = block_data(0, 1, 1, 14);
    t_box[12] = block_data(1, 0, 1, 14);
    t_box[13] = block_data(1, 1, 1, 14);
    t_box[14] = block_data(1, 1, 1, 14);
    t_box[15] = block_data(0, 1, 1, 14);
    // restlichen linien
    t_box[16] = block_data(0, 0, 0, 14);
    t_box[17] = block_data(0, 0, 1, 14);
    t_box[18] = block_data(0, 1, 0, 14);
    t_box[19] = block_data(0, 1, 1, 14);
    t_box[20] = block_data(1, 0, 0, 14);
    t_box[21] = block_data(1, 0, 1, 14);
    t_box[22] = block_data(1, 1, 0, 14);
    t_box[23] = block_data(1, 1, 1, 14);

    Transform f_form;
    f_form.setPos( pos);
    f_form.setScale( glm::vec3( CHUNK_SCALE) );

    if( p_vboCursor == NULL)
        glGenBuffers(1, &p_vboCursor);


    // Shader einstellen
    p_graphic->getVertexShader()->BindArray( p_vboCursor, 0);
    p_graphic->getVertexShader()->Bind();// Shader
    p_graphic->getVertexShader()->EnableVertexArray( 0);
    p_graphic->getVertexShader()->update( &f_form, p_graphic->getCamera()->getViewProjection());

    // Vbo übertragen
    glBindBuffer(GL_ARRAY_BUFFER, p_vboCursor);
    glBufferData(GL_ARRAY_BUFFER, t_box.size() * sizeof(block_data), &t_box[0], GL_DYNAMIC_DRAW);

    // Zeichnen
    glDrawArrays( GL_LINES, 0, 24);
}
