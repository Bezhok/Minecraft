#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "World.h"

class Entity
{
public:
	Entity();
	sf::Vector3f get_position() const { return m_pos; }
	~Entity();
protected:
	GLuint& load_texture(sf::String name);
	sf::Vector3f m_pos;
	sf::Vector3f m_rotation;
	sf::Vector3f m_size;
	World *m_world=nullptr;
};