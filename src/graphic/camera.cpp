#include "camera.h"

Camera::Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar, float ortho_size) {
    this->pos = pos;
    this->forward = glm::vec3(0.1f, 0.0f, 0.1f);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->projection = glm::perspective(fov, aspect, zNear, zFar);
    this->projectionortho = glm::ortho<float>(-ortho_size, ortho_size,-ortho_size,ortho_size, zNear, zFar);
}

void Camera::MoveForward(float amt) {
    pos += forward * amt;
}

void Camera::MoveForwardCross( float amt) {
    glm::vec3 tup(0.0f, 1.0f, 0.0f);
    //static const glm::vec3 UP(0.0f, 0.0f, 0.0f);
   // pos += tforward * amt;
    pos += glm::cross( glm::normalize(glm::cross(up, forward)), tup)*amt;
}

void Camera::MoveUp( float amt) {
        pos.y += amt;
}

void Camera::MoveRight(float amt) {
    pos += glm::cross(up, forward) * amt;
}

void Camera::Pitch(float angle, float push) {
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    glm::vec3 temp_forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));

    if( temp_forward.y < -0.99f || temp_forward.y > 0.99f ) // Schutz vor überdrehung
        if( push) {
        } else {
            return;
        }
    forward = temp_forward; // Temp wert setzten
    up = glm::normalize(glm::cross(forward, right));
}

void Camera::RotateY(float angle, bool push) {
    static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

    glm::mat4 rotation = glm::rotate(angle, UP);

    forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
    up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
}
