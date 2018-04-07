#ifndef SHADER_INCLUDED_H
#define SHADER_INCLUDED_H

#include <string>
#include <GL/glew.h>

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
	void disableArray( int i);
	void disableFullVertexArray();

	void Bind();
	void setSize( GLfloat x, GLfloat y);
	//void setTextureUnit(unsigned int TextureUnit);

	void update( glm::mat4 model, glm::mat4 getCurrentViewProjectionMatrix, glm::mat4 aa = glm::mat4(1));
	void updateWithout(  Transform *t_transform, glm::mat4 mvp) ;

	inline GLint& GetProgram() { return p_program; }
	void BindArray( GLuint Data, int Type, GLenum Type_Attrib = GL_FLOAT, int Attrib_size = 4);
	virtual ~Shader();

	GLuint getAntribute( int i) { return p_attribute[i]; }
protected:
private:
    static const unsigned int NUM_ATTRIBUTE = 3;
	static const unsigned int NUM_SHADERS = 2;
	static const unsigned int NUM_UNIFORMS = 2;
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
};

#endif
