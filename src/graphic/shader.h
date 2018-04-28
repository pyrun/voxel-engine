#ifndef SHADER_INCLUDED_H
#define SHADER_INCLUDED_H

#include <string>
#include <GL/glew.h>

#include "transform.h"

enum shader_mat{
    MAT_VIEW = 0,
    MAT_PROJECTION,
    MAT_MODEL
};

class ShaderVector {
public:
    int x;
    int y;
    int z;
    bool operator< ( const ShaderVector& rhs) {
        if( x + y + z > rhs.x + rhs.y + rhs.z)
            return true;
        return false;
    }
    bool operator> (const ShaderVector& rhs){
        if( x + y + z < rhs.x + rhs.y + rhs.z)
            return true;
        return false;
    }
    bool operator==(const ShaderVector& r) {
        if( x == r.x)
            if( y  == r.y)
                if( z == r.z)
                    return true;
        return false;
    }
};

class Shader {
public:
	Shader(const std::string& fileName);

	void EnableVertexArray( int i);
	void disableArray( int i);
	void disableFullVertexArray();

	void Bind();
	void setSize( GLfloat x, GLfloat y);
	void setLightMatrix( glm::mat4 matrix);
	//void setTextureUnit(unsigned int TextureUnit);
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setFloat(const std::string &name, float value) const;
	void setInt(const std::string &name, int value) const;

	void update( shader_mat mat, glm::mat4 matrix);

	inline GLint& GetProgram() { return p_program; }
	void BindArray( GLuint Data, int Type, GLenum Type_Attrib = GL_FLOAT, int Attrib_size = 4);
	virtual ~Shader();

	GLuint getAntribute( int i) { return p_attribute[i]; }
	glm::mat4 getVP() { return p_projection * p_view; }
protected:
private:
    static const unsigned int NUM_ATTRIBUTE = 4;
	static const unsigned int NUM_SHADERS = 2;
	static const unsigned int NUM_UNIFORMS = 5;
	int p_attribute_flag[NUM_ATTRIBUTE];
	void operator=(const Shader& shader) {}
	Shader(const Shader& shader) {}

	std::string LoadShader(const std::string& fileName);
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint CreateShader(const std::string& text, unsigned int type);

	GLint p_attribute[NUM_ATTRIBUTE];
	GLint p_program;
	GLint p_shaders[NUM_SHADERS];
	GLint p_uniforms[NUM_UNIFORMS];

	GLfloat p_old_alpha_value;

	glm::mat4 p_projection;
	glm::mat4 p_view;
};

#endif
