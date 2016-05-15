#include "shader.h"
#include <iostream>
#include <fstream>

Shader::Shader(const std::string& fileName) {
	m_program = glCreateProgram();
	m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
	m_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);

	for(unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(m_program, m_shaders[i]);

	glBindAttribLocation(m_program, 0, "position");
	glBindAttribLocation(m_program, 1, "texCoord");
	glBindAttribLocation(m_program, 2, "normal");


	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Invalid shader program");

	m_uniforms[0] = glGetUniformLocation(m_program, "g_mvp");
	m_uniforms[1] = glGetUniformLocation(m_program, "g_shadowmvp");
	m_uniforms[2] = glGetUniformLocation(m_program, "g_size");
	m_uniforms[3] = glGetUniformLocation(m_program, "g_shadowmap");
	m_uniforms[4] = glGetUniformLocation(m_program, "g_backgroundcolor");
	m_uniforms[5] = glGetUniformLocation(m_program, "g_alpha_cutoff");
	m_uniforms[6] = glGetUniformLocation(m_program, "g_sun");



    m_attribute[0] = glGetAttribLocation(m_program, "coord");
    m_attribute[1] = glGetAttribLocation(m_program, "data");
    m_attribute[2] = glGetAttribLocation(m_program, "normal");


    // GL error anzeigen
    GLenum error =  glGetError(); if(error) {
        printf( "Shader::Shader Error %d\n", (int)error);
    }
    printf( "Shader::Shader successful load %s\n", fileName.c_str());
}

Shader::~Shader() {
	for(unsigned int i = 0; i < NUM_SHADERS; i++) {
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }

	glDeleteProgram(m_program);
}

void Shader::EnableVertexArray( int i) {
    // Nur ein bestimmtes nutzen
    glEnableVertexAttribArray(m_attribute[i]);
    m_attribute_flag[ i] = 1;
    // GL error
    GLenum error =  glGetError(); if(error) {
        printf( "Shader::EnableVertexArray Error %d\n", (int)error);
    }
}

void Shader::DisableVertexAllArray() {
    // Vertex Array einschalten
    for(int i = 0; i < NUM_ATTRIBUTE; i++)
        if( m_attribute_flag[i] == 1)
            glDisableVertexAttribArray(m_attribute[i]);
    // GL error
    GLenum error =  glGetError(); if(error) {
        printf( "Shader::EnableVertexAllArray Error %d\n", (int)error);
    }
}

void Shader::Bind() {
	glUseProgram(m_program);

	DisableVertexAllArray();
}

void Shader::SetSun( GLfloat x, GLfloat y, GLfloat z, GLfloat f_strength) {
    glUniform4f( m_uniforms[6], x, y, z, f_strength);
}

void Shader::SetSize( GLfloat x, GLfloat y) {
    glUniform2f( m_uniforms[2], x, y);
}

void Shader::SetAlpha_cutoff( GLfloat x) {
    glUniform1f( m_uniforms[5], x);
}

void Shader::SetTextureUnit(unsigned int TextureUnit) {
    glUniform1i( m_uniforms[3], TextureUnit);
}

void Shader::SetBackgroundcolor(  GLfloat r, GLfloat g,  GLfloat b, GLfloat a) {
    glUniform4f( m_uniforms[4], r, g, b, a);
}

void Shader::Update(const Transform& transform, Camera *camera, Camera *shadow, glm::mat4 aa) {
    glm::mat4 MVP = aa * transform.GetMVP(*camera);

    /*if( camera == shadow)
        MVP = transform.GetMVPOrtho(*camera);*/

    glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
    glm::mat4 depthBiasMVP = biasMatrix * transform.GetMVPOrtho( *shadow);
    /*for(int i = 0; i < 4; i++) {
        glm::vec3 shift = glm::vec3((i % 2) * 0.5 / w, (i / 2) * 0.5 / h, 0);
        glm::mat4 aa = glm::translate(glm::mat4(1.0f), shift);
        glm::mat4 mvp = aa * projection * modelview;
        glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
        draw_scene();
        glAccum(i ? GL_ACCUM : GL_LOAD, 0.25);
    }*/

    glUniformMatrix4fv(m_uniforms[0], 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(m_uniforms[1], 1, GL_FALSE, &depthBiasMVP[0][0]);
}

void Shader::UpdateWithout( glm::mat4 mvp, Camera* t_camera, Transform t_transform) {
    Update( t_transform, t_camera, t_camera);

    //GetMVPOrtho
    float x = t_transform.GetPos().x;
    float y = t_transform.GetPos().y;
    float z = t_transform.GetPos().z;


    //mvp = mvp * glm::vec4( x, y, z, 1.0);

    mvp = glm::translate( mvp, glm::vec3(x, y, z));

    glUniformMatrix4fv(m_uniforms[0], 1, GL_FALSE, &mvp[0][0]);
}

void Shader::BindArray( GLuint Data, int Type, GLenum Type_Attrib, int Attrib_size ) {
    glBindBuffer( GL_ARRAY_BUFFER, Data); //
    switch( Type) {
        case 0: glVertexAttribPointer( m_attribute[0], Attrib_size, Type_Attrib, GL_FALSE, 0, (void*)0); break;
        case 1: glVertexAttribPointer( m_attribute[1], Attrib_size, Type_Attrib, GL_FALSE, 0, (void*)0); break;
        case 2: glVertexAttribPointer( m_attribute[2], Attrib_size, Type_Attrib, GL_FALSE, 0, (void*)0); break;
        default: break;
    }
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
