#include "engine.h"
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

engine::engine() {
    // set values
    p_openvr = NULL;
    p_isRunnig = true;
    p_framecap = true;
    p_timecap = 12; // ms -> 90hz

    // start subystems
    p_config = new config();
    p_graphic = new graphic( p_config);
    p_blocklist = new block_list("blocks");
    // set values after start
    p_blocklist->draw( p_graphic);

    p_tilemap = new texture( p_config->get( "tilemap", "engine", "tileset.png"));
    p_network = new network( p_config, p_tilemap, p_blocklist);
}

engine::~engine() {
    if( p_openvr)
        delete p_openvr;
    delete p_tilemap;
    delete p_blocklist;
    delete p_graphic;
    delete p_network;
    delete p_config;
}

void engine::startVR() {
    p_openvr = new openvr();
    p_framecap = false;
}

void engine::viewCurrentBlock( glm::mat4 viewProjection, int view_width) {
    float depth;

    world *l_world = p_network->getWorld();

    // voxel Anzeigen
    glReadPixels( p_graphic->getWidth() / 2, p_graphic->getHeight() / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    glm::vec4 viewport = glm::vec4(0, 0, p_graphic->getWidth(), p_graphic->getHeight());
    glm::vec3 wincoord = glm::vec3(p_graphic->getWidth() / 2, p_graphic->getHeight() / 2, depth);
    //glm::vec3 objcoord = glm::unProject(wincoord, p_graphic->getCamera()->GetView(), p_graphic->getCamera()->GetProjection(), viewport);

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
        int tile = l_world->GetTile( mx, my, mz);
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
        if( tile && p_input.Map.Place && !p_input.MapOld.Place ) {
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

            if( l_world->GetTile( mX, mY, mZ) == EMPTY_BLOCK_ID) {
                Chunk *tmp = l_world->getChunkWithPos( mX, mY, mZ);
                if( tmp) {
                    p_network->sendBlockChange( tmp, glm::vec3( mX, mY, mZ), p_blocklist->getByName( "treewood")->getID());
                    //l_world->SetTile( tmp, mX, mY, mZ, p_blocklist->getByID( "water")->getID());
                } else {
                    printf( "engine::ViewCurrentBlock Block nicht vorhanden wo man es setzen möchte\n");
                }
            }

            printf( "engine::ViewCurrentBlock Set: %d %d %d %d\n", mx, my, mz, tile);
            break;
        }
        if( tile && p_input.Map.Destory && !p_input.MapOld.Destory) {
            Chunk *tmp = l_world->getChunkWithPos( mx, my, mz);
            if( tmp) {
                p_network->sendBlockChange( tmp, glm::vec3( mx, my, mz), EMPTY_BLOCK_ID);
                //l_world->SetTile( tmp, mx, my, mz, EMPTY_BLOCK_ID);
            } else {
                p_network->getWorld()->addChunk( glm::vec3( 0, 0, 0), false);
                printf( "engine::ViewCurrentBlock Block nicht vorhanden wo man es setzen möchte\n");
            }
            break;
        }

        // tile
        if( tile) {
            //drawBox( viewProjection, glm::vec3( mx, my, mz)*glm::vec3( CHUNK_SCALE));
            break;
        }
    }
}

void engine::render( glm::mat4 view, glm::mat4 projection) {
    Shader *l_shader = p_graphic->getShadow();
    glm::mat4 l_proj = p_graphic->getLightProjection();
    glm::mat4 l_view = p_graphic->getLightView();

    p_graphic->getDisplay()->clear( false);
    l_shader->Bind();
    p_graphic->renderShadow( l_shader);
    //l_shader->update( MAT_PROJECTION, projection);
    //l_shader->update( MAT_VIEW, view);
    p_network->getWorld()->draw( p_graphic, l_shader);
    //p_network->drawEntitys( l_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // finish

    // DeferredShading
    p_graphic->bindDeferredShading();
    p_graphic->getDisplay()->clear( false);

    l_shader = p_graphic->getVoxelShader();
    l_shader->Bind();
    l_shader->update( MAT_PROJECTION, projection);
    l_shader->update( MAT_VIEW, view);
    p_graphic->addShadowMatrix( l_shader);
    p_network->getWorld()->draw( p_graphic, l_shader);

    /*l_shader = p_graphic->getObjectShader();
    l_shader->Bind();
    l_shader->update( MAT_PROJECTION, projection);
    l_shader->update( MAT_VIEW, view);
    p_graphic->addShadowMatrix( l_shader);
    p_network->drawEntitys( l_shader);*/
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void engine::fly( int l_delta) {
    float Speed = 0.005f;
    Camera *cam = p_graphic->getCamera();
    if( p_input.Map.Up )
        cam->MoveForwardCross( Speed*l_delta);
    if( p_input.Map.Down )
        cam->MoveForwardCross(-Speed*l_delta);
    if( p_input.Map.Right )
        cam->MoveRight(-Speed*l_delta);
    if( p_input.Map.Left )
        cam->MoveRight( Speed*l_delta);
    if( p_input.Map.Jump )
        cam->MoveUp( Speed*l_delta);
    if( p_input.Map.Shift )
        cam->MoveUp( -Speed*l_delta);
}

void engine::run() {
    // set variables
    Timer l_timer;
    struct clock l_clock;
    glm::mat4 l_mvp;

    int l_delta = 0;

    Timer l_timer_test;
    /*ServerCreated_ServerSerialized* l_hand = NULL;

    if( p_network->isServer()) {
        l_hand = new ServerCreated_ServerSerialized();
        l_hand->setPosition( glm::vec3( 0, 15, 5) );
        p_network->addObject( l_hand);
        l_hand->p_name = "box";
    }*/

    //l_hand->p_type = p_network->getObjectList()->get( l_hand->p_name.C_String());

    if( p_openvr) { // dirty hack
        glm::vec2 l_oldSize = p_graphic->getDisplay()->getSize();
        p_graphic->getDisplay()->setSize( p_openvr->getScreenSize());
        p_graphic->resizeDeferredShading();
    }

    p_graphic->getCamera()->GetPos().y = 20;

    // set up clock
    l_clock.tick();
    while( p_isRunnig) { // Runniz
        l_timer.Start();

        p_input.Reset();
        p_isRunnig = p_input.Handle( p_graphic->getWidth(), p_graphic->getHeight(), p_graphic->getWindow());

        Camera *cam = p_graphic->getCamera();
        cam->horizontalAngle ( -p_input.Map.MousePos.x * 2);
        cam->verticalAngle  ( p_input.Map.MousePos.y * 2);

        /// process
        if( p_config->get( "fly", "engine", "false") == "true")
            fly( l_delta);

        if( p_input.getResize()) {
            p_graphic->resizeWindow( p_input.getResizeW(), p_input.getResizeH());
            p_config->set( "width", std::to_string( p_input.getResizeW()), "graphic");
            p_config->set( "height", std::to_string( p_input.getResizeH()), "graphic");
        }

        if( p_network->process( l_delta))
            p_isRunnig = false;

        if( p_input.Map.Inventory && !p_input.MapOld.Inventory ) {
            //if( l_hand)
            //    l_hand->setPosition( cam->GetPos() + glm::vec3( 0, 2, 0) );
            //l_hand->getBody()->SetLinearVelocity( q3Vec3( 0, 0, 0));
            //l_hand->getBody()->ApplyForceAtWorldPoint( q3Vec3( 0, -100, 100),  q3Vec3( 0, 0, 0));
        }

        /// render #1 openVR
        if( p_openvr ) {
            glm::mat4 l_projection = p_openvr->getCurrentProjectionMatrix( vr::Eye_Left);
            glm::mat4 l_view_cam =  p_openvr->getCurrentViewMatrix( vr::Eye_Left) * p_graphic->getCamera()->getViewWithoutUp();
            render( l_view_cam, l_projection);
            p_openvr->renderForLeftEye();
            p_graphic->renderDeferredShading();
            p_openvr->renderEndLeftEye();

            l_projection = p_openvr->getCurrentProjectionMatrix( vr::Eye_Right);
            l_view_cam = p_openvr->getCurrentViewMatrix( vr::Eye_Right) * p_graphic->getCamera()->getViewWithoutUp();
            render( l_view_cam, l_projection);
            p_openvr->renderForRightEye();
            p_graphic->renderDeferredShading();
            p_openvr->renderEndRightEye();

            //l_timer.Start();
            p_openvr->renderFrame();
        }


        /// render #2 window


        glm::mat4 l_view_cam = p_graphic->getCamera()->getView();
        glm::mat4 l_projection = p_graphic->getCamera()->getProjection();

        l_timer_test.Start();
        render( l_view_cam, l_projection);
        p_graphic->getDisplay()->clear( false);
        p_graphic->renderDeferredShading();

        if( p_network->getWorld()) {
            viewCurrentBlock( l_projection * l_view_cam, 275); // 275 = 2,75Meter
        }

        p_graphic->getDisplay()->swapBuffers();

        /// calculation  time
        // print opengl error number
        GLenum l_error =  glGetError();
        if( l_error) {
            std::cout << "engine::run OpenGL error #" << l_error << std::endl;
        }

        // framerate
        p_framerate.push_back( l_clock.delta);
        if( p_framerate.size() > 100)
            p_framerate.erase( p_framerate.begin());
        float l_average_delta_time = 0;
        for( int i = 0; i < (int)p_framerate.size(); i++)
            l_average_delta_time += (float)p_framerate[i];
        l_average_delta_time = l_average_delta_time/(float)p_framerate.size();

        double averageFrameTimeMilliseconds = 1000.0/(l_average_delta_time==0?0.001:l_average_delta_time);
        p_title = "FPS_" + NumberToString( averageFrameTimeMilliseconds );
        p_title = p_title + " " + NumberToString( (double)l_timer.GetTicks()) + "ms";
        p_title = p_title + " X_" + NumberToString( cam->GetPos().x) + " Y_" + NumberToString( cam->GetPos().y) + " Z_" + NumberToString( cam->GetPos().z );
        if(  p_network->getWorld())
            p_title = p_title + " Chunks_" + NumberToString( (double) p_network->getWorld()->getAmountChunks());
        p_graphic->getDisplay()->setTitle( p_title);

        // one at evry frame
        l_clock.tick();

        // frame rate
        if( l_clock.delta < p_timecap && p_framecap) {
            int l_sleep = p_timecap - l_clock.delta;
            SDL_Delay( l_sleep==0?1:l_sleep);

        }

        l_delta = l_timer.GetTicks();
    }
}

/*void engine::drawBox( glm::mat4 viewProjection, glm::vec3 pos) {
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
    p_graphic->getVertexShader()->update( f_form.getModel(), viewProjection);

    // Vbo übertragen
    glBindBuffer(GL_ARRAY_BUFFER, p_vboCursor);
    glBufferData(GL_ARRAY_BUFFER, t_box.size() * sizeof(block_data), &t_box[0], GL_DYNAMIC_DRAW);

    // Zeichnen
    glDrawArrays( GL_LINES, 0, 24);
}*/
