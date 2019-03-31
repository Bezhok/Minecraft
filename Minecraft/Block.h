#pragma once
#include "pch.h"
#include "Box.h"
#include "block_db.h"

namespace World {
	using std::array;
	using namespace Base;

	class Map;

	class Block :
		public Box
	{
	public:
		Block(World::Map *world);
		~Block();
		void bind_textures(DB::block_id id, const sf::Vector3f &pos);
	private:
		GLuint m_buf[2];
	};
}