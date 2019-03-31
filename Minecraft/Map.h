#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"
#include "Chunk.h"

namespace World {
	using std::unordered_map;
	class Map
	{
	public:
		Map();
		~Map();

		bool is_block(int mx, int my, int mz);
		bool create_block(int x, int y, int z, DB::block_id type);
		bool delete_block(int x, int y, int z);

		const sf::Vector3f m_world_size;

		bool m_redraw_chunk = false;
		sf::Vector3i m_edited_chunk_coord;
		Chunk m_world[SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE];
	};
}


