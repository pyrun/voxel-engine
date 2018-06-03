#include "player.h"

player::player( world *world)
{
    p_object_id = world->createObject( "player", glm::vec3( 0, 20, 0));

    p_target_world = world;
}

player::~player()
{

}

void player::input( Input *input, Camera *camera, int delta) {
    glm::vec3 l_up(0.0f, 1.0f, 0.0f);
    object *l_player = p_target_world->getObject( p_object_id);
    float l_speed = 0.001f;

    if( input->Map.Up && l_player->getHit( physic::hit_side::ground)) {
        glm::vec3 l_force = glm::cross( glm::normalize(glm::cross( camera->getUp(), camera->getForward())), l_up) * (float)delta * l_speed;

        l_player->addVelocity( l_force + glm::vec3( 0, 0.0, 0));
    }

    if( input->Map.Down && l_player->getHit( physic::hit_side::ground)) {
        glm::vec3 l_force = glm::cross( glm::normalize(glm::cross( camera->getUp(), camera->getForward())), l_up) * (float)delta * l_speed;

        l_player->addVelocity( -l_force + glm::vec3( 0, 0.0, 0));
    }

    if( input->Map.Right && l_player->getHit( physic::hit_side::ground) ) {
        glm::vec3 l_force = -glm::cross( camera->getUp(), camera->getForward()) * (float)delta * l_speed;
        l_player->addVelocity( l_force);
    }

    if( input->Map.Left && l_player->getHit( physic::hit_side::ground) ) {
        glm::vec3 l_force = glm::cross( camera->getUp(), camera->getForward()) * (float)delta * l_speed;
        l_player->addVelocity( l_force);
    }

    if( input->Map.Jump && l_player->getHit( physic::hit_side::ground) && l_player->getVerlocity().y < 0.0001f)
        l_player->addVelocity( glm::vec3( 0.f, 0.052f, 0.f));
    if( input->Map.Shift )
        ;
}

glm::vec3 player::getPositonHead() {
    object *l_player = p_target_world->getObject( p_object_id);
    return l_player->getType()->getHead()+l_player->getPosition();
}

void player::setPosition( glm::vec3 position)
{
    p_position = position;
}
