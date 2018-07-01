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

        void input( Input *input, Camera *camera, int delta);

        void changeWorldTo( world *world);

        glm::vec3 getPositonHead( bool height = true);
        int getId() { return p_object_id; }
        world* getWorld() { return p_target_world; }
        std::string getName() { return p_name; }
    protected:

    private:
        glm::vec3 p_position;
        config *p_config;

        std::string p_name;

        int p_object_id;
        world *p_target_world;
};


class player_handle {
    public:
        player_handle();
        ~player_handle();

        bool fileExists(std::string filename);
        void load( std::string path);
        void load_player( std::string folder_player, world *world);

        std::vector<player *> getPlayer() { return p_players; }
    private:
        std::vector<player *> p_players;
};

#endif // PLAYER_H
