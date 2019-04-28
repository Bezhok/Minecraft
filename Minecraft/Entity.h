#pragma once
#include "pch.h"
#include "Map.h"

namespace Base {
	class Entity
	{
	protected:
		sf::Vector3f m_pos;
		sf::Vector3f m_rotation;
		sf::Vector3f m_size;
		World::Map *m_map = nullptr;

	public:
		/* eponymous */
		const sf::Vector3f &get_position() const { return m_pos; }
	};
}