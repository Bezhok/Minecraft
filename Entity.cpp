#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>

#define GL_CLAMP_TO_EDGE 0x812F

Entity::Entity()
{
}

Entity::~Entity()
{
}

GLuint& Entity::load_texture(sf::String name)
{
	GLuint texture = 0;
	sf::Image image;
	image.loadFromFile(name);
	image.flipVertically();

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(
		GL_TEXTURE_2D,
		GL_RGBA,
		image.getSize().x,
		image.getSize().y,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image.getPixelsPtr()
	);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	//	GL_LINEAR_MIPMAP_LINEAR);

	// scope texture view
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);

	// delete "borders"
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
}