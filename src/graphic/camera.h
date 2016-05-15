#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H 1

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Camera {
public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar, float ortho_size = 50);

	inline glm::mat4 GetViewProjection() const {
		return projection * glm::lookAt(pos, pos + forward, up);
	}
	inline glm::mat4 GetViewProjectionOrtho() const{
		return projectionortho * glm::lookAt(pos, pos+forward, up); //glm::lookAt( t_pos, t_pos+ppos, glm::vec3( 0.0f, 1.0f, 0.0f));
	}

	inline glm::vec3& GetPos() { return pos; }
	inline glm::vec3& GetForward() { return forward; }
	inline glm::vec3& GetUp() { return up; }

	inline glm::mat4& GetProjection() {
	    return projection;
	}
	inline glm::mat4 GetView() {
	    return glm::lookAt(pos, pos + forward, up);
	}

	void MoveForward(float amt);
	void MoveForwardCross( float amt);
	void MoveUp( float amt);

	void MoveRight(float amt);

	void Pitch(float angle, float push = false);

	void RotateY(float angle, bool push = false);

protected:
private:
	glm::mat4 projection;
	glm::mat4 projectionortho;
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
};

#endif
