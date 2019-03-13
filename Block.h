#pragma once
#include <SFML/OpenGL.hpp>
#include "Box.h"

class World;
class Block :
	public Box
{
public:
	Block(World *world);
	~Block();
	void bind_textures(const sf::Vector3f &pos);
private:
	GLuint m_buf[2];
};

