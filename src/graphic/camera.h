#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H 1

#define GLM_ENABLE_EXPERIMENTAL

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

    glm::mat4 getViewProjection() const{ return p_projection * glm::lookAt(p_position, p_position + p_direction, p_up); }
	inline glm::mat4 GetViewProjectionOrtho() const{ return projectionortho * glm::lookAt(p_position, p_position+p_direction, p_up); }

	inline glm::vec3& GetPos() { return p_position; }
	inline glm::vec3& GetForward() { return p_direction; }
	inline glm::vec3& GetUp() { return p_up; }

	inline glm::mat4& GetProjection() { return p_projection;}
	inline glm::mat4 GetView() { return glm::lookAt(p_position, p_position + p_direction, p_up); }
protected:
    void calcOrientation();
private:
	glm::mat4 p_projection;
	glm::mat4 projectionortho;
	glm::vec3 p_position;
	glm::vec3 p_direction;
	glm::vec3 p_up;

	float p_fov;
    float p_horizontalAngle;
    float p_verticalAngle;

    float p_aspect;
    float p_zNear;
    float p_zFar;
};

#endif
