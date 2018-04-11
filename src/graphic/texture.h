#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class texture {
public:
	texture(const std::string& fileName);

	void Bind();

	virtual ~texture();

	GLuint getId() { return p_texture; }
protected:
private:
	texture(const texture& texture) {}
	void operator=(const texture& texture) {}

	GLuint p_texture;
};

#endif

