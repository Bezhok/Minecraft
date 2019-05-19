#pragma once
#include "pch.h"
#include "Box.h"
//#include "Map.h"
#include "block_db.h"

//namespace Base { class Box; }
namespace World { class Map; }

namespace World {
	class Block :
		public Base::Box
	{
	public:
		/* init var */
		Block(Map *map);

		/* eponymous. pos in blocks coordinates */
		void bind_textures(DB::block_id id, const sf::Vector3f &pos);
	};
}