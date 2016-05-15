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
    m_graphic = new Graphic( 1024, 800);

    // Runnig
    m_isRunnig = true;
    m_blocklist = new BlockList("objects");

    // Framenrate setzten
    framenrate.Set( 60);
    m_blocklist->Draw( m_graphic);
    m_world = new World( "tileset.png", m_blocklist);

    m_gui = new Gui;

    m_sun = new Sun;

    glGenBuffers(1, &m_vboCursor);
}

Game::~Game() {
    glDeleteBuffers(1, &m_vboCursor);
    delete m_world;
    delete m_blocklist;
    delete m_gui;
    delete m_sun;
    delete m_graphic;
}

void Game::ViewCurrentBlock( int view_width) {
    float depth;

    // Voxel Anzeigen
    glReadPixels( m_graphic->GetWidth() / 2, m_graphic->GetHeight() / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    glm::vec4 viewport = glm::vec4(0, 0, m_graphic->GetWidth(), m_graphic->GetHeight());
    glm::vec3 wincoord = glm::vec3(m_graphic->GetWidth() / 2, m_graphic->GetHeight() / 2, depth);
    glm::vec3 objcoord = glm::unProject(wincoord, m_graphic->GetCamera()->GetView(), m_graphic->GetCamera()->GetProjection(), viewport);

    glm::vec3 testpos = m_graphic->GetCamera()->GetPos();
    glm::vec3 prevpos = m_graphic->GetCamera()->GetPos();

    Chunk* chunk = m_world->GetChunk( 0, 0, 0);

    // wo hin man sieht den block finden
    int mx, my, mz;
    for(int i = 0; i < view_width/2; i++) {
        // Advance from our currect position to the direction we are looking at, in small steps
        prevpos = testpos;
        testpos += m_graphic->GetCamera()->GetForward() * 0.02f;

        // hack die komma zahl ab z.B. 13,4 -> 13,0
        mx = floorf(testpos.x);
        my = floorf(testpos.y);
        mz = floorf(testpos.z);

        // falls wir ein block finden das kein "Air" ist dann sind wir fertig
        Tile *tile = m_world->GetTile( mx, my, mz);
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
        if( tile->ID && m_input.Map.Place && !m_input.MapOld.Place ) {
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

            if( !m_world->GetTile( mX, mY, mZ)) {
                Chunk *tmp = m_world->GetChunkWithPos( mX, mY, mZ);
                if( tmp)
                    m_world->SetTile( tmp, mX, mY, mZ, m_blocklist->GetBlockID( "water")->getID());
                else
                    printf( "Game::ViewCurrentBlock Chunk nicht vorhanden");
            }

            printf( "Set: %d %d %d %d\n", mx, my, mz, tile->ID);
            break;
        }
        if( tile->ID && m_input.Map.Destory && !m_input.MapOld.Destory) {
            Chunk *tmp = m_world->GetChunkWithPos( mx, my, mz);
            if( tmp)
                m_world->SetTile( tmp, mx, my, mz, 0 );
            else
                printf( "Game::ViewCurrentBlock Chunk nicht vorhanden");
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
    m_graphic->GetVertexShader()->BindArray( m_vboCursor, 0, GL_FLOAT);
    m_graphic->GetVertexShader()->Bind();// Shader
    m_graphic->GetVertexShader()->EnableVertexArray( 0);
    Transform f_form;

    //m_graphic->GetVertexShader()->Update( f_form, m_graphic->GetCamera(), m_graphic->GetCamera());
    m_graphic->GetVertexShader()->UpdateWithout( one, m_graphic->GetCamera());

    //m_graphic->GetVoxelShader()->UpdateWithout( one, m_graphic->GetCamera());
    glBindBuffer(GL_ARRAY_BUFFER, m_vboCursor);
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
    m_config.SetTransparency( true);

    Object *obj = new Object;
    obj->Init();

    while( m_isRunnig) { // Runnig
        m_input.Reset();
        m_isRunnig = m_input.Handle( m_graphic->GetWidth(), m_graphic->GetHeight(), m_graphic->GetWindow());
        // Input einsehen
        float Speed = 0.1f;
        Camera *cam = m_graphic->GetCamera();
        cam->RotateY( -m_input.Map.MousePos.x * 2);
        cam->Pitch( m_input.Map.MousePos.y * 2);

        if( m_input.Map.Up )
            cam->MoveForwardCross( Speed);
        if( m_input.Map.Down )
            cam->MoveForwardCross(-Speed);
        if( m_input.Map.Right )
            cam->MoveRight(-Speed);
        if( m_input.Map.Left )
            cam->MoveRight( Speed);
        if( m_input.Map.Jump )
            cam->MoveUp( Speed);
        if( m_input.Map.Shift )
            cam->MoveUp( -Speed);
        if( m_input.Map.Place);
        if( m_input.Map.Destory && !m_input.MapOld.Destory);
        if( m_input.Map.Inventory && !m_input.MapOld.Inventory) {
            if( m_config.GetSupersampling()) {

                m_config.SetSupersampling( false);

                printf( "Game::Start Supersampling deactive\n");
            } else {
                printf( "Game::Start Supersampling active\n");
                m_config.SetSupersampling( true);
            }
        }
        //cos_i++;
        // Framenrate anfangen zu zählen
        framenrate.StartCount();

        // Zeichen oberfläche aufräumen
        m_sun->Process( m_graphic->GetVoxelShader(), m_graphic);



        Timer m_timer;
        m_timer.Start();
        m_sun->SetDay();

        m_graphic->GetDisplay()->Clear();

        // chunks zeichnen
        m_world->Draw( m_graphic, &m_config);

        obj->draw( m_graphic->GetObjectShader(), m_graphic->GetCamera());


		// View Cross
        ViewCross();

        // Debug
        //DrawBox( -1, -1, -1);

        //m_gui->Draw( m_graphic);

        // Block anzeigen was in der Sichtweite ist
        ViewCurrentBlock( 275); // 275 = 2,75Meter

		// World process
        m_world->Process();

        //m_world->GetChunk( 0, -1, 0)->SetChange( true);

        // Swap die Buffer um keine Renderfehler zu bekommen
        m_graphic->GetDisplay()->SwapBuffers();

        if( m_timer.GetTicks() != 0)
        ;//    printf( "Timer %d \n", m_timer.GetTicks());

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
        Title = Title + " X_" + NumberToString( cam->GetPos().x) + " Y_" + NumberToString( cam->GetPos().y) + " Z_" + NumberToString( cam->GetPos().z );
//      Title = Title + "Tile X " + NumberToString( (float)x) + " Y " + NumberToString(  (float)y) + " Z " + NumberToString(  (float)z );
        Title = Title + " Chunks_" + NumberToString( (double)m_world->GetAmountChunks());
//        if( m_world->GetWorldTree() != NULL)
//            m_world->GetWorldTree()->SetTile( cam->GetPos().x, cam->GetPos().y, cam->GetPos().z, 1);
        //m_graphic->GetDisplay()->SetTitle( Title);

        // Measure speed

        printf("%s\n", Title.c_str());
        // http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html

    }
    delete obj;
}

void Game::DrawBox( GLshort bx, GLshort by, GLshort bz) {
    std::vector<ChunkVboDataStruct> t_box;

    // Chunk Vbo Data Struct
    t_box.resize( 24 );

    // x-y side
    t_box[0] = CVDS_SetBlock(0, 0, 0, 14);
    t_box[1] = CVDS_SetBlock(1, 0, 0, 14);
    t_box[2] = CVDS_SetBlock(0, 0, 0, 14);
    t_box[3] = CVDS_SetBlock(0, 1, 0, 14);
    t_box[4] = CVDS_SetBlock(1, 0, 0, 14);
    t_box[5] = CVDS_SetBlock(1, 1, 0, 14);
    t_box[6] = CVDS_SetBlock(1, 1, 0, 14);
    t_box[7] = CVDS_SetBlock(0, 1, 0, 14);
    // x-y & z+1
    t_box[8] = CVDS_SetBlock(0, 0, 1, 14);
    t_box[9] = CVDS_SetBlock(1, 0, 1, 14);
    t_box[10] = CVDS_SetBlock(0, 0, 1, 14);
    t_box[11] = CVDS_SetBlock(0, 1, 1, 14);
    t_box[12] = CVDS_SetBlock(1, 0, 1, 14);
    t_box[13] = CVDS_SetBlock(1, 1, 1, 14);
    t_box[14] = CVDS_SetBlock(1, 1, 1, 14);
    t_box[15] = CVDS_SetBlock(0, 1, 1, 14);
    // restlichen linien
    t_box[16] = CVDS_SetBlock(0, 0, 0, 14);
    t_box[17] = CVDS_SetBlock(0, 0, 1, 14);
    t_box[18] = CVDS_SetBlock(0, 1, 0, 14);
    t_box[19] = CVDS_SetBlock(0, 1, 1, 14);
    t_box[20] = CVDS_SetBlock(1, 0, 0, 14);
    t_box[21] = CVDS_SetBlock(1, 0, 1, 14);
    t_box[22] = CVDS_SetBlock(1, 1, 0, 14);
    t_box[23] = CVDS_SetBlock(1, 1, 1, 14);

    Transform f_form;
    f_form.GetPos().x = bx;
    f_form.GetPos().y = by;
    f_form.GetPos().z = bz;

    // Shader einstellen
    m_graphic->GetVertexShader()->BindArray( m_vboCursor, 0);
    m_graphic->GetVertexShader()->Bind();// Shader
    m_graphic->GetVertexShader()->EnableVertexArray( 0);
    m_graphic->GetVertexShader()->Update( f_form, m_graphic->GetCamera(), m_graphic->GetCamera());

    // Vbo übertragen
    glBindBuffer(GL_ARRAY_BUFFER, m_vboCursor);
    glBufferData(GL_ARRAY_BUFFER, t_box.size() * sizeof(ChunkVboDataStruct), &t_box[0], GL_DYNAMIC_DRAW);

    // Zeichnen
    glDrawArrays( GL_LINES, 0, 24);
}
