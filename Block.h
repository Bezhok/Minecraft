#pragma once
#include "Entity.h"
#include <vector>
#include <SFML/OpenGL.hpp>

using std::vector;

class Block :
	public Entity
{
public:
	Block();
	~Block();

	void bind_textures(float size, const sf::Vector3f &pos, const sf::Vector3f &shift);
protected:
	vector<GLuint> m_box;
};

