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
	void zoom( float change);

    glm::mat4 GetViewProjection() const{
		return m_projection * glm::lookAt(m_position, m_position + m_direction, m_up);
	}
	inline glm::mat4 GetViewProjectionOrtho() const{
		return projectionortho * glm::lookAt(m_position, m_position+m_direction, m_up); //glm::lookAt( t_pos, t_pos+ppos, glm::vec3( 0.0f, 1.0f, 0.0f));
	}

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

	float m_fov = 45.0f;
    float m_horizontalAngle = 0.0f;
    float m_verticalAngle = 0.0f;

    float m_aspect = 0;
    float m_zNear = 0;
    float m_zFar = 0;
};

#endif
