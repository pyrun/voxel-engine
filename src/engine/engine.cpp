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
    p_world_player = NULL;
    p_timecap = 12; // ms -> 90hz

    p_config = new config();
    p_graphic = new graphic( p_config);
    p_landscape_generator = new landscape( p_config);
    p_network = new network( p_config);

    // install lua
    script::add_lib( "world", &lua_object_install);

    // load all objects
    p_object_handle = new object_handle();
    p_object_handle->load( p_config);

    // set block list up
    p_blocklist = new block_list( p_config);
    p_blocklist->init( p_graphic, p_config);

    // set up start world
    world *l_world = new world( p_blocklist, "0", p_object_handle);
    l_world->setGenerator( p_landscape_generator);

    p_worlds.push_back( l_world);
    p_world_player = p_worlds[0];

    // player
    p_players.push_back( new player(p_world_player) );

    p_player = p_players[0];
    //p_player = NULL;


}

engine::~engine() {
    for( auto l_world:p_worlds)
        delete l_world;
    p_worlds.clear();
    if( p_openvr)
        delete p_openvr;
    delete p_blocklist;
    delete p_graphic;
    delete p_landscape_generator;
    delete p_config;
}

void engine::startVR() {
    p_openvr = new openvr();
    p_framecap = false;
}

void engine::startServer() {
    p_network->start_sever();
}

void engine::startClient( std::string address) {
    p_network->start_client( address);
}


void engine::render( glm::mat4 view, glm::mat4 projection) {
    Shader *l_shader = NULL;

    // biding deferred shading
    p_graphic->bindDeferredShading();
    p_graphic->getDisplay()->clear( false);

    // render voxel
    l_shader = p_graphic->getVoxelShader();
    l_shader->Bind();
    l_shader->update( MAT_PROJECTION, projection);
    l_shader->update( MAT_VIEW, view);
    p_world_player->drawVoxel( p_graphic, l_shader);

    // render object
    l_shader = p_graphic->getObjectShader();
    l_shader->Bind();
    l_shader->update( MAT_PROJECTION, projection);
    l_shader->update( MAT_VIEW, view);
    p_world_player->drawObjects( p_graphic, l_shader);

    // draw debug lines
    l_shader = p_graphic->getDebugShader();
    l_shader->Bind();
    l_shader->update( MAT_PROJECTION, projection);
    l_shader->update( MAT_VIEW, view);
    l_shader->update( MAT_MODEL, glm::mat4( 1));
    p_world_player->drawObjectsDebug( p_graphic, l_shader);

    // we are done
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

void engine::walk( int l_delta) {
    if( p_player) {
        p_player->input( &p_input, p_graphic->getCamera(), l_delta);
    }
}


void engine::run() {
    // set variables
    Timer l_timer;
    std::string l_title;
    struct clock l_clock;
    glm::mat4 l_mvp;
    uint32_t l_delta = 0;

    p_world_player->createObject( "player", glm::vec3( -5.5f, 10.0f, -5.5f) );
    p_world_player->createObject( "box", glm::vec3( 5.5f, 10.0f, 5.5f) );

    //p_world_player->createObject( "player", glm::vec3( +5.0f, 10.9f, +5.0f) );

    /*p_world_player->createObject( "player", glm::vec3( -5.8f, 10.9f, -5.5f) );

    p_world_player->createObject( "player", glm::vec3( -5.0f, 10.0f, +5.5f) );*/

    if( !p_network->isClient()) {
        if( !p_world_player->load()) {
            int l_size = 3;
            int l_end = -3;
            for( int x = -l_size; x <= l_size; x++)
                for( int z = -l_size; z <= l_size; z++)
                    for( int y = 1; y > l_end; y--)
                        p_world_player->addChunk( glm::vec3( x, y, z), true);
        }
    }

    if( p_openvr) { // dirty hack -> force openVR resolution
        //glm::vec2 l_oldSize = p_graphic->getDisplay()->getSize();
        //p_graphic->getDisplay()->setSize( p_openvr->getScreenSize());

        SDL_SetWindowSize( p_graphic->getWindow(), p_openvr->getScreenSize().x, p_openvr->getScreenSize().y);
        p_graphic->getDisplay()->setSize( p_openvr->getScreenSize());
        p_graphic->resizeWindow( p_openvr->getScreenSize());
        p_graphic->resizeDeferredShading();
    }

    p_graphic->getCamera()->addPos( glm::vec3( 0, 5, 0));

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
        //if( p_config->get( "fly", "engine", "false") == "true")
        //    fly( l_delta);


        if( p_input.getResize()) {
            p_graphic->resizeWindow( glm::vec2( p_input.getResizeW(), p_input.getResizeH()) );
            p_config->set( "width", std::to_string( p_input.getResizeW()), "graphic");
            p_config->set( "height", std::to_string( p_input.getResizeH()), "graphic");
        }

        for( world *l_world:p_worlds) {
            l_world->process_object_handling();
            l_world->process_thrend_physic();
        }

        if( p_player && p_world_player->getPhysicFlag()) {
            walk( l_delta);
            p_player->raycastView( &p_input, p_graphic->getCamera()->getPos(), p_graphic->getCamera()->getForward(), 300);
        }

        if( p_input.Map.Inventory && !p_input.MapOld.Inventory ) {

        }

        /// render #1 openVR
        if( p_openvr ) {
            cam->setPos( p_player->getPositonHead( false));

            object *l_obj = p_world_player->getObject( p_player->getId());
            glm::vec3 l_head = p_openvr->getHeadPosition();
            l_head.y = 0;
            l_obj->setDrawOffset( l_head);

            glm::vec3 l_head_rotation = p_openvr->getHeadRotation();
            l_head_rotation.x = 0;
            l_head_rotation.z = 0;
            l_obj->setRotation( l_head_rotation);

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

            p_openvr->renderFrame();
        } else {
            object *l_obj = p_world_player->getObject( p_player->getId());
            glm::vec3 l_body_rotation = glm::vec3( 0, cam->getHorizontalAngle(), 0);
            l_obj->setRotation( l_body_rotation);
        }


        /// render #2 window
        //cam->setPos( glm::vec3( -5, 10, -5));
        //cam->setPos( p_player->getPositonHead( true));
        //cam->setPos( p_player->getPositonHead());

        cam->setPos( p_player->getPositonHead());
        glm::mat4 l_view_cam = p_graphic->getCamera()->getView();
        glm::mat4 l_projection = p_graphic->getCamera()->getProjection();

        render( l_view_cam, l_projection);
        p_graphic->getDisplay()->clear( false);
        p_graphic->renderDeferredShading();

        p_graphic->getDisplay()->swapBuffers();

        /// calculation  time
        // print opengl error number
        GLenum l_error =  glGetError();
        if( l_error) {
            std::cout << "engine::run OpenGL error #" << l_error << std::endl;
        }

        // framerate
        p_framerate.push_back( l_clock.delta);
        if( p_framerate.size() > 10)
            p_framerate.erase( p_framerate.begin());
        float l_average_delta_time = 0;
        for( int i = 0; i < (int)p_framerate.size(); i++)
            l_average_delta_time += (float)p_framerate[i];
        l_average_delta_time = l_average_delta_time/(float)p_framerate.size();

        double averageFrameTimeMilliseconds = 1000.0/(l_average_delta_time==0?0.001:l_average_delta_time);
        l_title = "FPS_" + NumberToString( averageFrameTimeMilliseconds );
        l_title = l_title + " " + NumberToString( (double)l_timer.GetTicks()) + "ms";
        l_title = l_title + " X_" + NumberToString( cam->getPos().x) + " Y_" + NumberToString( cam->getPos().y) + " Z_" + NumberToString( cam->getPos().z );
        l_title = l_title + " Chunks_" + NumberToString( (double) p_world_player->getAmountChunks()) + "/" + NumberToString( (double)p_world_player->getAmountChunksVisible() );
        p_graphic->getDisplay()->setTitle( l_title);

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
