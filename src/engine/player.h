#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "world.h"
#include "input.h"
#include "config.h"

class player
{
    public:
        player( world *world);
        virtual ~player();

        void createObject();

        void raycastView( Input *input, glm::vec3 position, glm::vec3 lookat, int forward);

        void setPosition( glm::vec3 position);
        void setName( std::string name);
        void setId( unsigned int id);
        void setGUID( RakNet::RakNetGUID guid);

        void input( Input *input, Camera *camera, int delta);

        void changeWorldTo( world *world);

        glm::vec3 getPositonHead( bool height = true);
        object *getObject();
        int getId() { return p_object_id; }
        world* getWorld() { return p_target_world; }
        std::string getName() { return p_name; }
        RakNet::RakNetGUID getGUID() { return p_guid; }
    protected:

    private:
        glm::vec3 p_position;
        config *p_config;

        std::string p_name;
        RakNet::RakNetGUID p_guid;

        int p_object_id;
        world *p_target_world;
};


class player_handle {
    public:
        player_handle();
        ~player_handle();

        bool fileExists(std::string filename);
        player *createPlayer( world *world);
        void load( std::string path);
        void load_player( std::string folder_player, world *world);

        player *getPlayerByGUID( RakNet::RakNetGUID guid);
        player *getPlayerByObject( object *object);
        std::vector<player *> getPlayer() { return p_players; }
    private:
        std::vector<player *> p_players;
};

#endif // PLAYER_H
