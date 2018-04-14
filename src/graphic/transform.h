#ifndef TRANSFORM_INCLUDED_H
#define TRANSFORM_INCLUDED_H

#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
#include "camera.h"

struct Transform {
public:
	Transform(const glm::vec3& pos = glm::vec3( 0, 0, 0), const glm::vec3& rot = glm::vec3( 0, 0, 0), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f)) {
		this->pos = pos;
		this->rot = rot;
		this->scale = scale;
	}

	inline glm::mat4 getModel() {
	    updateModel();
	    return p_model;
    }

	inline void updateModel() {
	    if( !p_change)
            return;
        p_change = false;
		glm::mat4 posMat = glm::translate(pos);
        glm::mat4 scaleMat = glm::scale(scale);
        glm::mat4 rotX = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0));
        glm::mat4 rotY = glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 rotZ = glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
        glm::mat4 rotMat = rotX * rotY * rotZ;
        p_model = posMat * rotMat * scaleMat;
	}


	inline glm::vec3 getPos() { return pos; p_change = true;}
	inline glm::vec3 getPosConst() const { return pos; }
	inline glm::vec3 GetRot() { return rot; }
	inline glm::vec3 GetScale() { return scale; }

	inline void setPos(glm::vec3 pos) { this->pos = pos; p_change = true; }
	inline void setRot(glm::vec3 rot) { this->rot = rot; p_change = true;}
	inline void setScale(glm::vec3 scale) { this->scale = scale; p_change = true;}
protected:
private:
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;

	glm::mat4 p_model;
	bool p_change;
};

#endif
