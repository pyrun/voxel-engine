#include "player.h"

player::player( world *world)
{
    p_object_id = world->createObject( "player", glm::vec3( 0, 20, 0));

    p_target_world = world;
}

player::~player()
{

}

glm::vec3 player::getPositonHead() {
    object *l_player = p_target_world->getObject( p_object_id);
    return glm::vec3( l_player->getBody()->GetPosition().x, l_player->getBody()->GetPosition().y, l_player->getBody()->GetPosition().z ) + glm::vec3( -0.5, 1.1, 0.5);
}

void player::setPosition( glm::vec3 position)
{
    p_position = position;
}

b3Body *player::getBody()
{
    object *l_player = p_target_world->getObject( p_object_id);
    return l_player->getBody();
}
