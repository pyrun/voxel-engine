#include "camera.h"

Camera::Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar, float ortho_size) {
    // set camera
    p_position = pos;
    p_fov = fov;
    p_aspect = aspect;
    p_zNear = zNear;
    p_zFar = zFar;

    p_horizontalAngle = 0;
    p_verticalAngle = 0;

    // calc zoom
    zoom( 0);

    p_projection_ortho = glm::ortho<float>(-ortho_size, ortho_size,-ortho_size,ortho_size, p_zNear, p_zFar);

    // calc
    calcOrientation();
}

void Camera::MoveForward(float amt) {
    p_position += p_direction * amt;
}

void Camera::MoveForwardCross( float amt) {
    glm::vec3 tup(0.0f, 1.0f, 0.0f);
    //static const glm::vec3 UP(0.0f, 0.0f, 0.0f);
   // pos += tforward * amt;
    p_position += glm::cross( glm::normalize(glm::cross(p_up, p_direction)), tup)*amt;
}

void Camera::MoveUp( float amt) {
    p_position.y += amt;
}

void Camera::MoveRight(float amt) {
    p_position += glm::cross(p_up, p_direction) * amt;
}

void Camera::calcOrientation() {
    // directions
    p_direction = glm::vec3( cos( p_verticalAngle) * sin(p_horizontalAngle), sin(p_verticalAngle) , cos(p_verticalAngle) * cos(p_horizontalAngle) );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(p_horizontalAngle - 3.14f/2.0f),
        0,
        cos(p_horizontalAngle - 3.14f/2.0f)
    );
    // up vector
    p_up = glm::cross( right, p_direction );
}

void Camera::verticalAngle(float angle, float push) {
    p_verticalAngle -= angle;
    if( p_verticalAngle > 1.5)
        p_verticalAngle = 1.5;
    if( p_verticalAngle < -1.5)
        p_verticalAngle = -1.5;
    calcOrientation();
}

void Camera::horizontalAngle(float angle, bool push) {
    p_horizontalAngle += angle;
    calcOrientation();
    if( p_horizontalAngle < 0)
        p_horizontalAngle += 3.14*2;
    if( p_horizontalAngle > 3.14*2)
        p_horizontalAngle -= 3.14*2;
}

void Camera::resize( float aspect) {
    p_aspect = aspect;
    zoom();
}

void Camera::zoom( float change) {
    p_fov += change;
    p_projection = glm::perspective(p_fov, p_aspect, p_zNear, p_zFar);
}
