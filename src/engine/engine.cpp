#include "engine.h"
#include "../graphic/glerror.h"
#include "../system/timer.h"
#include <stdio.h>

engine *p_engine = NULL;

/// force calculation
static int lua_calcForceUp(lua_State* state) {
    glm::vec3 l_up(0.0f, 1.0f, 0.0f);
    if( !lua_isnumber( state, 1) && !lua_isnumber( state, 2) ) {
        printf( "lua_calcForceUp call wrong argument\n");
        return 0;
    }

    // get position
    float l_delta = lua_tonumber( state, 1);
    float l_speed = lua_tonumber( state, 2);

    // get tile
    glm::vec3 l_force = glm::cross( glm::normalize(glm::cross( p_engine->getGraphic()->getCamera()->getUp(), p_engine->getGraphic()->getCamera()->getForward())), l_up) * l_delta * l_speed;

    // push the vector
    lua_pushnumber( state, l_force.x);
    lua_pushnumber( state, l_force.y);
    lua_pushnumber( state, l_force.z);

    // finish
    return 3;
}

static int lua_calcForceSide(lua_State* state) {
    if( !lua_isnumber( state, 1) && !lua_isnumber( state, 2)) {
        printf( "lua_calcForceSide call wrong argument\n");
        return 0;
    }

    // get position
    float l_delta = lua_tonumber( state, 1);
    float l_speed = lua_tonumber( state, 2);

    // get tile
    glm::vec3 l_force = glm::cross( p_engine->getGraphic()->getCamera()->getUp(), p_engine->getGraphic()->getCamera()->getForward()) * l_delta * l_speed;

    // push the vector
    lua_pushnumber( state, l_force.x);
    lua_pushnumber( state, l_force.y);
    lua_pushnumber( state, l_force.z);

    // finish
    return 3;
}

void lua_engine_install( lua_State *state) {
    // defined functions
    lua_pushcfunction( state, lua_calcForceUp);
    lua_setglobal( state, "calcForceUp");
    lua_pushcfunction( state, lua_calcForceSide);
    lua_setglobal( state, "calcForceSide");
}

std::string NumberToString( double Number) {
    char buffer[255];
    sprintf( buffer, "%0.2f", Number);
    std::string temp = buffer;
    return temp;
}

void extern_createWorld( std::string name) {
    p_engine->createWorld( name, false);
}

world *extern_getWorld( std::string name) {
    return p_engine->getWorld( name);
}

block_list *extern_blocklist() {
    return p_engine->getBlocklist();
}

void extern_createObject( world *world, std::string type, glm::vec3 position, unsigned int id) {
    if( p_engine == NULL || world == NULL || p_engine->getNetwork() == NULL)
        return;
    p_engine->getNetwork()->sendCreateObject( RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, world, type, position, id);
}

void extern_transferData( world *world, object *object) {
    if( p_engine == NULL || world == NULL || object == NULL || p_engine->getNetwork() == NULL)
        return;
    if( p_engine->getNetwork()->isServer()) {
        if( !p_engine->getPlayerHandle()->getPlayerByObject( object) )
            p_engine->getNetwork()->sendMatrixObject( RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, world, object);
        else {
            std::vector<player *> l_players = p_engine->getPlayerHandle()->getPlayer();
            for( player *l_player:l_players) {
                if( l_player->getId() != object->getId())
                    p_engine->getNetwork()->sendMatrixObject( l_player->getGUID(), false, world, object);
            }
        }
    } else {
        if( p_engine->getPlayer() != NULL && p_engine->getPlayer()->getId() == object->getId() )
            p_engine->getNetwork()->sendMatrixObject( RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, world, object);
    }
}

void extern_deleteObject( world *world, unsigned int id) {
    if( p_engine == NULL || world == NULL || p_engine->getNetwork() == NULL)
        return;
    p_engine->getNetwork()->sendDeleteObject( RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, world, id);
}

void extern_changeCall( world *world, Chunk *chunk, glm::ivec3 position, unsigned int id) {
    p_engine->getNetwork()->sendBlockChange( world, chunk, position, id);
}

static int engine_thread(void *data)
{
    for ( ;; ) {
        engine *l_engine = (engine*)data;

        l_engine->process_thread();

        SDL_Delay(1);
    }

    return 0;
}

engine::engine() {
    // set values
    p_openvr = NULL;
    p_player = NULL;
    p_network = NULL;
    p_isRunnig = true;
    p_framecap = true;
    p_timecap = 12; // ms -> 90hz

    p_config = new config();
    #ifndef NO_GRAPHICS
        p_graphic = new graphic( p_config);
    #else
        p_graphic = NULL;

        // This line is only needed, if you want debug the program
        SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");

        // minimum SDL init
        if(SDL_Init( SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
            printf( "engine::engine failed to start the SDL system\n");
            return;
        }

        // initializing glew
        glewExperimental = GL_TRUE;
        GLenum res = glewInit();
        if(res != GLEW_OK) {
            printf( "engine::engine glew was failed to initialize\n" );
        }
    #endif

    printf( "test\n");

    p_landscape_generator = new landscape( p_config);
    p_players = new player_handle();

    // install lua
    install_lua();

    // load all objects
    p_object_handle = new object_handle();
    p_object_handle->load( p_config);

    // set block list up
    p_blocklist = new block_list( p_config);
    p_blocklist->init( p_graphic, p_config);

    // creating thread
    p_thread = SDL_CreateThread( engine_thread, "eninge_thread", (void *)this);

    // get standard player
    p_player_file = p_config->get( "folder", "player", "players/") + p_config->get( "use", "player", "noname");
}

engine::~engine() {
    delete p_players;
    for( auto l_world:p_worlds)
        delete l_world;
    p_worlds.clear();
    if( p_openvr)
        delete p_openvr;
    delete p_blocklist;
    delete p_graphic;
    delete p_landscape_generator;
    delete p_config;
    if( p_network)
        delete p_network;
}

void engine::startVR() {
    p_openvr = new openvr();
    p_framecap = false;
}

void engine::startServer() {
    p_network = new network( p_config);
    p_network->start_sever();
}

void engine::startClient( std::string address) {
    p_network = new network( p_config);
    p_network->start_client( address);
}

void engine::install_lua() {
    script::add_lib( "world", &lua_object_install);
    script::add_lib( "engine", &lua_engine_install);
}

void engine::render( glm::mat4 view, glm::mat4 projection) {
    Shader *l_shader = NULL;

    // biding deferred shading
    p_graphic->bindDeferredShading();
    p_graphic->getDisplay()->clear( false);

    // render voxel
    if( p_player && p_player->getWorld()) {
        l_shader = p_graphic->getVoxelShader();
        l_shader->Bind();
        l_shader->update( MAT_PROJECTION, projection);
        l_shader->update( MAT_VIEW, view);
        p_player->getWorld()->drawVoxel( p_graphic, l_shader);

        // render object
        l_shader = p_graphic->getObjectShader();
        l_shader->Bind();
        l_shader->update( MAT_PROJECTION, projection);
        l_shader->update( MAT_VIEW, view);
        p_player->getWorld()->drawObjects( p_graphic, l_shader);

        // draw debug lines
        l_shader = p_graphic->getDebugShader();
        l_shader->Bind();
        l_shader->update( MAT_PROJECTION, projection);
        l_shader->update( MAT_VIEW, view);
        l_shader->update( MAT_MODEL, glm::mat4( 1));
        p_player->getWorld()->drawObjectsDebug( p_graphic, l_shader);
    }

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

        if( p_input.Map.Inventory && !p_input.MapOld.Inventory) {
            /*if( p_player == p_players[1]) {
                p_player = p_players[0];
            } else {
                p_player = p_players[1];
            }*/
            /*if( p_player->getWorld() == p_worlds[0])
                p_player->changeWorldTo( p_worlds[1]);
            else
                p_player->changeWorldTo( p_worlds[0]);*/
            p_player->getWorld()->createObject( "evil_bot", p_player->getObject()->getPosition() + glm::vec3( 0, 0, 1));
        }
    }
}

world *engine::createWorld( std::string name, bool player) {
    for( int i = 0; i < (int)p_worlds.size(); i++) {
        if( p_worlds[i]->getName() == name)
            return p_worlds[i];
    }
    world *l_world = new world( p_blocklist, name, p_object_handle, player);
    l_world->setGenerator( p_landscape_generator);

    if( p_network) {
        l_world->changeCall = &extern_changeCall;
        l_world->createObjectCall = &extern_createObject;
        l_world->objectSyncCall = &extern_transferData;
        l_world->deleteObjectCall = &extern_deleteObject;
    }
    p_worlds.push_back( l_world);

    return l_world;
}

void engine::loadWorld( std::string name, bool player) {
    world *l_world = createWorld( name, player);
    if( !l_world->load()) {
        int l_size = 3;
        int l_end = -3;
        for( int x = -l_size; x <= l_size; x++)
            for( int z = -l_size; z <= l_size; z++)
                for( int y = 1; y > l_end; y--)
                    l_world->addChunk( glm::vec3( x, y, z), true);
    }
}

world *engine::getWorld( std::string name) {
    for( int i = 0; i < (int)p_worlds.size(); i++) {
        if( p_worlds[i]->getName() == name)
            return p_worlds[i];
    }
    return NULL;
}

void engine::process_thread() {

}

void engine::run() {
    // set variables
    timer l_timer;
    std::string l_title;
    struct clock l_clock;
    glm::mat4 l_mvp;
    uint32_t l_delta = 0;

    // set up start world
    if( (p_network && (p_network->isServer() || p_network->isNone())) || !p_network ) {
        // load world
        loadWorld( "0", true);
        loadWorld( "1");

        // load player
        if( !p_player ) {
            p_players->load_player( p_player_file, p_worlds[0]);
            p_player = p_players->getPlayer()[0];
            p_player->createObject();
        }
    } else { // client network
        p_players->load_player( p_player_file);
    }

    #ifndef NO_GRAPHICS

    /*p_worlds[0]->createObject( "player", glm::vec3( 40.0f, 10.0f, -5.5f) );
    p_worlds[0]->createObject( "box", glm::vec3( 5.5f, 10.0f, 5.5f) );

    p_worlds[0]->createObject( "hand", glm::vec3( 0.0f, 10.0f, 5.5f) );

    p_worlds[0]->createObject( "evil_bot", glm::vec3( 5.2f, 10.9f, 0.0f) );*/

    // set resolution
    if( p_openvr) {
        SDL_SetWindowSize( p_graphic->getWindow(), p_openvr->getScreenSize().x, p_openvr->getScreenSize().y);
        p_graphic->getDisplay()->setSize( p_openvr->getScreenSize());
        p_graphic->resizeDeferredShading();
        p_graphic->resizeWindow( p_openvr->getScreenSize());
    }

    p_graphic->getCamera()->addPos( glm::vec3( 0, 5, 0));

    #endif // NO_GRAPHICS

    // set up clock
    l_clock.tick();

    // loop
    while( p_isRunnig) {
        l_timer.start();

        p_input.Reset();
        p_isRunnig = p_input.Handle( p_graphic->getWidth(), p_graphic->getHeight(), p_graphic->getWindow());

        Camera *l_cam = p_graphic->getCamera();
        l_cam->horizontalAngle ( -p_input.Map.MousePos.x * 2);
        l_cam->verticalAngle  ( p_input.Map.MousePos.y * 2);

        /// process
        if( p_input.getResize()) {
            p_graphic->resizeWindow( glm::vec2( p_input.getResizeW(), p_input.getResizeH()) );
            p_config->set( "width", std::to_string( p_input.getResizeW()), "graphic");
            p_config->set( "height", std::to_string( p_input.getResizeH()), "graphic");
        }

        //network
        if( p_network) {
            p_engine = this;
            p_network->createWorld = &extern_createWorld;
            p_network->getWorld = &extern_getWorld;
            p_network->getBlocklist = &extern_blocklist;
            p_network->process( &p_worlds, p_players);

            // load player if client no player load
            if( !p_player && p_network->isClient() ) {
                // try load player if world there
                if( p_worlds.size() > 0 ) {
                    p_players->load_player( p_player_file, p_worlds[0]);
                    p_player = p_players->getPlayer()[0];
                }
            }
        }

        // every world
        for( world *l_world:p_worlds) {
            lua_object_set_targets( l_world);
            l_world->process_object_handling();
            l_world->process_thrend_physic();
            // set sync time
            if( p_network)
                l_world->setObjectSync( 1);
                ;//l_world->setObjectSync( p_network->getTotalAveragePing());
        }

        if( p_player && p_player->getWorld() && p_player->getWorld()->getPhysicFlag()) {
            lua_object_set_targets(  p_player->getWorld());
            p_engine = this;
            walk( l_delta);
            p_player->raycastView( &p_input, p_graphic->getCamera()->getPos(), p_graphic->getCamera()->getForward(), 300);
        }

        if( p_input.Map.Refresh && !p_input.MapOld.Refresh ) {
            for( world *l_world:p_worlds)
                l_world->reloadScripts();
        }

        /// render #1 openVR
        if( p_openvr ) {
            l_cam->setPos( p_player->getPositonHead( false));

            object *l_obj = p_player->getWorld()->getObject( p_player->getId());
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
            if( p_player && p_player->getWorld()) {
                object *l_obj = p_player->getWorld()->getObject( p_player->getId());
                if( l_obj) {
                    glm::vec3 l_body_rotation = glm::vec3( 0, l_cam->getHorizontalAngle(), 0);
                    l_obj->setRotation( l_body_rotation);
                }
            }
        }


        /// render #2 window
        if( p_player)
            l_cam->setPos( p_player->getPositonHead());
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
        l_title = l_title + " " + NumberToString( (double)l_timer.getTicks()) + "ms";
        l_title = l_title + " X_" + NumberToString( l_cam->getPos().x) + " Y_" + NumberToString( l_cam->getPos().y) + " Z_" + NumberToString( l_cam->getPos().z );
        if( p_player) {
            if( p_player->getWorld())
                l_title = l_title + " Chunks_" + NumberToString( (double) p_player->getWorld()->getAmountChunks()) + "/" + NumberToString( (double)p_player->getWorld()->getAmountChunksVisible() );
            l_title = l_title + " Name: \"" + p_player->getName() + "\"";
            l_title = l_title + " Id: \"" + NumberToString( (double)p_player->getId()) + "\"";
        }
        if( p_network != NULL && p_network->isClient())
            l_title = l_title + " Ping: \"" + NumberToString( (double)p_network->getAveragePing( p_network->getServerGUID() )) + "\"";
        p_graphic->getDisplay()->setTitle( l_title);

        // one at evry frame
        l_clock.tick();

        // frame rate
        if( l_clock.delta < p_timecap && p_framecap) {
            int l_sleep = p_timecap - l_clock.delta;
            SDL_Delay( l_sleep==0?1:l_sleep);

        }

        l_delta = l_timer.getTicks();
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
