#ifndef SHADER_INCLUDED_H
#define SHADER_INCLUDED_H

#include <string>
#include <glew.h>

#include "transform.h"

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
	void DisableVertexAllArray();
	void Bind();
	void SetSun( GLfloat x, GLfloat y, GLfloat z, GLfloat f_strength);
	void SetSize( GLfloat x, GLfloat y);
	void SetAlpha_cutoff( GLfloat x);
	void SetTextureUnit(unsigned int TextureUnit);
	void SetBackgroundcolor(  GLfloat r, GLfloat g,  GLfloat b, GLfloat a);
	void Update(const Transform& transform, Camera *camera, Camera *shadow, glm::mat4 aa = glm::mat4(1));
	void UpdateWithout( glm::mat4 mvp, Camera *t_camera, Transform t_transform = Transform() );
	inline GLuint& GetProgram() { return m_program; }
	void BindArray( GLuint Data, int Type, GLenum Type_Attrib = GL_SHORT, int Attrib_size = 4);
	virtual ~Shader();
protected:
private:
    static const unsigned int NUM_ATTRIBUTE = 3;
	static const unsigned int NUM_SHADERS = 2;
	static const unsigned int NUM_UNIFORMS = 7;
	int m_attribute_flag[NUM_ATTRIBUTE];
	void operator=(const Shader& shader) {}
	Shader(const Shader& shader) {}

	std::string LoadShader(const std::string& fileName);
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint CreateShader(const std::string& text, unsigned int type);

	GLuint m_attribute[NUM_ATTRIBUTE];
	GLuint m_program;
	GLuint m_shaders[NUM_SHADERS];
	GLuint m_uniforms[NUM_UNIFORMS];
};

#endif
