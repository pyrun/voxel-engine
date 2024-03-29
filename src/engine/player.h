#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "world.h"
#include "input.h"
#include "config.h"
#include "../graphic/entity_draw.h"

struct player_teleport {
    glm::vec3 position;
};

class player
{
    public:
        player( world *world = NULL);
        virtual ~player();

        void createObject();

        void raycastView( input::handle *input, glm::vec3 position, glm::vec3 lookat, int forward);
        player_teleport *handleTeleport( input::handle *input, glm::vec3 position, glm::vec3 lookat, int forward);

        void setPosition( glm::vec3 position);
        void setName( std::string name);
        void setId( unsigned int id);
        void setGUID( RakNet::RakNetGUID guid);
        void setWorld( world *world);

        void input( input::handle *input, Camera *camera, int delta);

        void changeWorldTo( world *world);

        glm::vec3 getPositonHead( bool height = true);
        object *getObject();
        int getId() { return p_object_id; }
        world* getWorld() { return p_target_world; }
        std::string getName() { return p_name; }
        RakNet::RakNetGUID getGUID() { return p_guid; }
        entity_draw *getEntityDraw() { return &p_entity_draw; }
    protected:

    private:
        glm::vec3 p_position;
        config *p_config;

        std::string p_name;
        RakNet::RakNetGUID p_guid;

        int p_object_id;
        world *p_target_world;

        entity_draw p_entity_draw;
};


class player_handle {
    public:
        player_handle();
        ~player_handle();

        bool fileExists(std::string filename);
        player *createPlayer( world *world);
        void load( std::string path);
        void load_player( std::string folder_player, world *world = NULL);

        void deletePlayerByGUID( RakNet::RakNetGUID guid);

        player *getPlayerByName( std::string name);
        player *getPlayerByGUID( RakNet::RakNetGUID guid);
        player *getPlayerByObject( object *object);
        std::vector<player *> getPlayer() { return p_players; }
    private:
        std::vector<player *> p_players;
};

#endif // PLAYER_H
