#pragma once
#include "pch.h"
#include "Box.h"
#include "Map.h"
#include "block_db.h"

namespace World {
	class Block :
		public Base::Box
	{
	public:
		Block(Map *world);
		~Block();
		void bind_textures(DB::block_id id, const sf::Vector3f &pos);
	private:
		GLuint m_buf[2];
	};
}