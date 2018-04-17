#include "shader.h"

#include <iostream>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& fileName) {
	p_program = glCreateProgram();
	p_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
	p_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);

	for(unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(p_program, p_shaders[i]);

	glBindAttribLocation(p_program, 0, "vertexPosition");
	glBindAttribLocation(p_program, 1, "vertexNormals");
	glBindAttribLocation(p_program, 2, "vertexTexture");

	glLinkProgram(p_program);
	CheckShaderError(p_program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(p_program);
	CheckShaderError(p_program, GL_LINK_STATUS, true, "Invalid shader program");

	p_uniforms[0] = glGetUniformLocation(p_program, "projection");
	p_uniforms[1] = glGetUniformLocation(p_program, "view");
	p_uniforms[2] = glGetUniformLocation(p_program, "model");
	p_uniforms[3] = glGetUniformLocation(p_program, "tilesize");
	p_uniforms[4] = glGetUniformLocation(p_program, "lightSpaceMatrix");

	p_attribute[0] = glGetAttribLocation(p_program, "vertexPosition");
	p_attribute[1] = glGetAttribLocation(p_program, "vertexNormals");
	p_attribute[2] = glGetAttribLocation(p_program, "vertexTexture");

    // GL error anzeigen
    GLenum error =  glGetError(); if(error) {
        printf( "Shader::Shader Error %d\n", (int)error);
    }
    printf( "Shader::Shader successful load %s\n", fileName.c_str());
}

Shader::~Shader() {
	for(unsigned int i = 0; i < NUM_SHADERS; i++) {
        glDetachShader(p_program, p_shaders[i]);
        glDeleteShader(p_shaders[i]);
    }

	glDeleteProgram(p_program);
}

void Shader::EnableVertexArray( int i) {
    // Nur ein bestimmtes nutzen
    glEnableVertexAttribArray(p_attribute[i]);
    p_attribute_flag[ i] = 1;
}

void Shader::disableArray( int i) {
    if( p_attribute_flag[i] == 1)
        glDisableVertexAttribArray(p_attribute[i]);
}

void Shader::disableFullVertexArray() {
    // Vertex Array einschalten
    for(int i = 0; i < NUM_ATTRIBUTE; i++)
        if( p_attribute_flag[i] == 1)
            glDisableVertexAttribArray(p_attribute[i]);
}

void Shader::Bind() {
	glUseProgram(p_program);
}

void Shader::setSize( GLfloat x, GLfloat y) {
    glUniform2f( p_uniforms[3], x, y);
}

void Shader::setLightMatrix( glm::mat4 matrix) {
    glUniformMatrix4fv( p_uniforms[4], 1, GL_FALSE, glm::value_ptr( matrix));
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv( glGetUniformLocation( p_program, name.c_str()), 1, &value[0]);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation( p_program, name.c_str()), value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation( p_program, name.c_str()), value);
}

void Shader::update( shader_mat mat, glm::mat4 matrix) {
    unsigned int l_uniform = 0;
    switch( mat) {
        case MAT_PROJECTION:
            l_uniform = 0;
        break;
        case MAT_VIEW:
            l_uniform = 1;
        break;
        case MAT_MODEL:
            l_uniform = 2;
        break;
    }

    // send matrix
    glUniformMatrix4fv(p_uniforms[ l_uniform], 1, GL_FALSE, glm::value_ptr( matrix));
}

void Shader::BindArray( GLuint Data, int Type, GLenum Type_Attrib, int Attrib_size ) {
    glBindBuffer( GL_ARRAY_BUFFER, Data); //
    glVertexAttribPointer( p_attribute[Type], Attrib_size, Type_Attrib, GL_FALSE, 0, (void*)0);
}

std::string Shader::LoadShader(const std::string& fileName) {
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open()) {
        while(file.good()) {
            getline(file, line);
			output.append(line + "\n");
        }
    } else {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage) {
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        if(isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
    GLuint shader = glCreateShader(type);

    if(shader == 0)
		std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* p[1];
    p[0] = text.c_str();
    GLint lengths[1];
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}
