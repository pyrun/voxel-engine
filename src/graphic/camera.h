#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H 1

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Camera {
public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar, float ortho_size = 50);

	void MoveForward(float amt);
	void MoveForwardCross( float amt);
	void MoveUp( float amt);
	void MoveRight(float amt);

	void verticalAngle (float angle, float push = false);
	void horizontalAngle (float angle, bool push = false);
	void resize( float aspect);
	void zoom( float change = 0.0f);

    glm::mat4 GetViewProjection() const{ return m_projection * glm::lookAt(m_position, m_position + m_direction, m_up); }
	inline glm::mat4 GetViewProjectionOrtho() const{ return projectionortho * glm::lookAt(m_position, m_position+m_direction, m_up); }

	inline glm::vec3& GetPos() { return m_position; }
	inline glm::vec3& GetForward() { return m_direction; }
	inline glm::vec3& GetUp() { return m_up; }

	inline glm::mat4& GetProjection() { return m_projection;}
	inline glm::mat4 GetView() { return glm::lookAt(m_position, m_position + m_direction, m_up); }
protected:
    void calcOrientation();
private:
	glm::mat4 m_projection;
	glm::mat4 projectionortho;
	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_up;

	float m_fov;
    float m_horizontalAngle;
    float m_verticalAngle;

    float m_aspect;
    float m_zNear;
    float m_zFar;
};

#endif
