#include "camera.h"

Camera::Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar, float ortho_size) {
    // set camera
    m_position = pos;
    m_fov = fov;
    m_aspect = aspect;
    m_zNear = zNear;
    m_zFar = zFar;

    m_horizontalAngle = 0;
    m_verticalAngle = 0;

    // calc zoom
    zoom( 0);

    this->projectionortho = glm::ortho<float>(-ortho_size, ortho_size,-ortho_size,ortho_size, m_zNear, m_zFar);

    // calc
    calcOrientation();
}

void Camera::MoveForward(float amt) {
    m_position += m_direction * amt;
}

void Camera::MoveForwardCross( float amt) {
    glm::vec3 tup(0.0f, 1.0f, 0.0f);
    //static const glm::vec3 UP(0.0f, 0.0f, 0.0f);
   // pos += tforward * amt;
    m_position += glm::cross( glm::normalize(glm::cross(m_up, m_direction)), tup)*amt;
}

void Camera::MoveUp( float amt) {
    m_position.y += amt;
}

void Camera::MoveRight(float amt) {
    m_position += glm::cross(m_up, m_direction) * amt;
}

void Camera::calcOrientation() {
    // directions
    m_direction = glm::vec3( cos( m_verticalAngle) * sin(m_horizontalAngle), sin(m_verticalAngle) , cos(m_verticalAngle) * cos(m_horizontalAngle) );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(m_horizontalAngle - 3.14f/2.0f),
        0,
        cos(m_horizontalAngle - 3.14f/2.0f)
    );
    // up vector
    m_up = glm::cross( right, m_direction );
}

void Camera::verticalAngle(float angle, float push) {
    m_verticalAngle -= angle;
    if( m_verticalAngle > 1.5)
        m_verticalAngle = 1.5;
    if( m_verticalAngle < -1.5)
        m_verticalAngle = -1.5;
    calcOrientation();
}

void Camera::horizontalAngle(float angle, bool push) {
    m_horizontalAngle += angle;
    calcOrientation();
    if( m_horizontalAngle < 0)
        m_horizontalAngle += 3.14*2;
    if( m_horizontalAngle > 3.14*2)
        m_horizontalAngle -= 3.14*2;
}

void Camera::resize( float aspect) {
    m_aspect = aspect;
    zoom();
}

void Camera::zoom( float change) {
    m_fov += change;
    m_projection = glm::perspective(m_fov, m_aspect, m_zNear, m_zFar);
}
