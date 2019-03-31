#pragma once
#include "pch.h"
#include "Map.h"

namespace Base {
	using namespace World;

	class Entity
	{
	public:
		Entity();
		sf::Vector3f get_position() const { return m_pos; }
		~Entity();
	protected:
		GLuint load_texture(sf::String name);
		sf::Vector3f m_pos;
		sf::Vector3f m_rotation;
		sf::Vector3f m_size;
		World::Map *m_world = nullptr;
	};
}