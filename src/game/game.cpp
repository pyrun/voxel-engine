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
    p_graphic = new Graphic( 1024, 800);

    // Runnig
    p_isRunnig = true;
    p_blocklist = new BlockList("objects");

    // Framenrate setzten
    framenrate.Set( 60);
    p_blocklist->Draw( p_graphic);
    p_world = new World( "tileset.png", p_blocklist);

    p_gui = new Gui;

    p_sun = new Sun;

    glGenBuffers(1, &p_vboCursor);
}

Game::~Game() {
    glDeleteBuffers(1, &p_vboCursor);
    delete p_world;
    delete p_blocklist;
    delete p_gui;
    delete p_sun;
    delete p_graphic;
}

void Game::ViewCurrentBlock( int view_width) {
    float depth;

    // Voxel Anzeigen
    glReadPixels( p_graphic->GetWidth() / 2, p_graphic->GetHeight() / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    glm::vec4 viewport = glm::vec4(0, 0, p_graphic->GetWidth(), p_graphic->GetHeight());
    glm::vec3 wincoord = glm::vec3(p_graphic->GetWidth() / 2, p_graphic->GetHeight() / 2, depth);
    glm::vec3 objcoord = glm::unProject(wincoord, p_graphic->GetCamera()->GetView(), p_graphic->GetCamera()->GetProjection(), viewport);

    glm::vec3 testpos = p_graphic->GetCamera()->GetPos();
    glm::vec3 prevpos = p_graphic->GetCamera()->GetPos();

    Chunk* chunk = p_world->GetChunk( 0, 0, 0);

    // wo hin man sieht den block finden
    int mx, my, mz;
    for(int i = 0; i < view_width/2; i++) {
        // Advance from our currect position to the direction we are looking at, in small steps
        prevpos = testpos;
        testpos += p_graphic->GetCamera()->GetForward() * 0.02f;

        // hack die komma zahl ab z.B. 13,4 -> 13,0
        mx = floorf(testpos.x);
        my = floorf(testpos.y);
        mz = floorf(testpos.z);

        // falls wir ein block finden das kein "Air" ist dann sind wir fertig
        Tile *tile = p_world->GetTile( mx, my, mz);
        if( !tile )
            continue;

        int px = floorf(prevpos.x);
        int py = floorf(prevpos.y);
        int pz = floorf(prevpos.z);

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
                Chunk *tmp = p_world->GetChunkWithPos( mX, mY, mZ);
                if( tmp)
                    p_world->SetTile( tmp, mX, mY, mZ, p_blocklist->GetBlockID( "water")->getID());
                else
                    printf( "Game::ViewCurrentBlock Block nicht vorhanden wo man es setzen möchte\n");
            }

            printf( "Game::ViewCurrentBlock Set: %d %d %d %d\n", mx, my, mz, tile->ID);
            break;
        }
        if( tile->ID && p_input.Map.Destory && !p_input.MapOld.Destory) {
            Chunk *tmp = p_world->GetChunkWithPos( mx, my, mz);
            if( tmp)
                p_world->SetTile( tmp, mx, my, mz, EMPTY_BLOCK_ID);
            else
                printf( "Game::ViewCurrentBlock Chunk nicht vorhanden\n");
            break;
        }

        // tile
        if( tile->ID) {
            DrawBox( mx, my, mz);
            break;
        }
    }
}

void Game::ViewCross() {
    GLfloat cross[4][4] = {
                {-0.05, 0, 0, 13},
                {+0.05, 0, 0, 13},
                {0, -0.05, 0, 13},
                {0, +0.05, 0, 13},
            };

    glm::mat4 one(1);
    p_graphic->GetVertexShader()->BindArray( p_vboCursor, 0, GL_FLOAT);
    p_graphic->GetVertexShader()->Bind();// Shader
    p_graphic->GetVertexShader()->EnableVertexArray( 0);
    Transform f_form;

    //p_graphic->GetVertexShader()->Update( f_form, p_graphic->GetCamera(), p_graphic->GetCamera());
    p_graphic->GetVertexShader()->UpdateWithout( one, p_graphic->GetCamera());

    //p_graphic->GetVoxelShader()->UpdateWithout( one, p_graphic->GetCamera());
    glBindBuffer(GL_ARRAY_BUFFER, p_vboCursor);
    glBufferData(GL_ARRAY_BUFFER, sizeof cross, cross, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, 4);
}

static float fract(float value) {
	float f = value - floorf(value);
	if(f > 0.5)
		return 1 - f;
	else
		return f;
}

void Game::Start() {
    p_config.SetTransparency( true);

    Object *obj = new Object;
    obj->Init();

    while( p_isRunnig) { // Runnig
        p_input.Reset();
        p_isRunnig = p_input.Handle( p_graphic->GetWidth(), p_graphic->GetHeight(), p_graphic->GetWindow());
        // Input einsehen
//        float Speed = 0.1f * framenrate.getMSframe() *framenrate.getLimit()/ 1000.0f;
        float Speed = 0.1f;
        //printf( "%f %f \n", Speed, framenrate.getMSframe());
        Camera *cam = p_graphic->GetCamera();
        cam->horizontalAngle ( -p_input.Map.MousePos.x * 2);
        cam->verticalAngle  ( p_input.Map.MousePos.y * 2);

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
            p_graphic->ResizeWindow( p_input.getResizeW(), p_input.getResizeH());
        //cos_i++;
        // Framenrate anfangen zu zählen
        framenrate.StartCount();

        // Zeichen oberfläche aufräumen
        p_sun->Process( p_graphic->GetVoxelShader(), p_graphic);




        Timer p_timer;
        p_timer.Start();
        p_sun->SetDay();

        p_graphic->GetDisplay()->Clear();

        // chunks zeichnen
        p_world->Draw( p_graphic, &p_config);

        obj->draw( p_graphic->GetObjectShader(), p_graphic->GetCamera());


		// View Cross
        //ViewCross();

        // Debug
        //DrawBox( -1, -1, -1);

        //p_gui->Draw( p_graphic);
        // Block anzeigen was in der Sichtweite ist
        ViewCurrentBlock( 275); // 275 = 2,75Meter

		// World process
        p_world->Process();

        // Swap die Buffer um keine Renderfehler zu bekommen
        p_graphic->GetDisplay()->SwapBuffers();

        // fehler anzeigen -> schleife eine meldung bedeutet ich habe verkackt
        GLenum error =  glGetError();
        if(error) {
            std::cout << error << std::endl;
        }

        // Framenrate begrenzen
        framenrate.CalcDelay();

        // Titel setzten
        //Title = "FPS: " + NumberToString( (double)(int)framenrate.GetFramenrate());
        Title = "FPS_" + NumberToString( framenrate.getFrameratePrecisely() );
        Title = Title + " ms_" + NumberToString( framenrate.getMSframe());
        Title = Title + " X_" + NumberToString( cam->GetPos().x) + " Y_" + NumberToString( cam->GetPos().y) + " Z_" + NumberToString( cam->GetPos().z );
//      Title = Title + "Tile X " + NumberToString( (float)x) + " Y " + NumberToString(  (float)y) + " Z " + NumberToString(  (float)z );
        Title = Title + " Chunks_" + NumberToString( (double)p_world->GetAmountChunks());
//        if( p_world->GetWorldTree() != NULL)
//            p_world->GetWorldTree()->SetTile( cam->GetPos().x, cam->GetPos().y, cam->GetPos().z, 1);
        p_graphic->GetDisplay()->SetTitle( Title);

        // Measure speed
        //printf("%s\n", Title.c_str());
        // http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html

    }
    delete obj;
}

void Game::DrawBox( GLshort bx, GLshort by, GLshort bz) {
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
    f_form.GetPos().x = bx;
    f_form.GetPos().y = by;
    f_form.GetPos().z = bz;

    // Shader einstellen
    p_graphic->GetVertexShader()->BindArray( p_vboCursor, 0);
    p_graphic->GetVertexShader()->Bind();// Shader
    p_graphic->GetVertexShader()->EnableVertexArray( 0);
    p_graphic->GetVertexShader()->Update( f_form, p_graphic->GetCamera(), p_graphic->GetCamera());

    // Vbo übertragen
    glBindBuffer(GL_ARRAY_BUFFER, p_vboCursor);
    glBufferData(GL_ARRAY_BUFFER, t_box.size() * sizeof(block_data), &t_box[0], GL_DYNAMIC_DRAW);

    // Zeichnen
    glDrawArrays( GL_LINES, 0, 24);
}
