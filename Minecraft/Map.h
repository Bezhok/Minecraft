#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"
#include "Chunk.h"

namespace World {
	class Map
	{
		typedef std::array<std::array<std::array<Chunk, SUPER_CHUNK_SIZE>, SUPER_CHUNK_SIZE_HEIGHT>, SUPER_CHUNK_SIZE> map_type;
		typedef std::shared_ptr<map_type> map_ref;
	public:
		Map();
		~Map();

		bool is_block(int mx, int my, int mz);
		bool create_block(int x, int y, int z, DB::block_id type);
		bool delete_block(int x, int y, int z);
		bool save();
		bool load();

		const sf::Vector3f m_world_size;

		bool m_redraw_chunk = false;
		sf::Vector3i m_edited_chunk_coord;
		// 3-d array of world
		map_ref m_world;

	private:
		int get_int_from_stringstream(std::stringstream &line_stream);
	};
}


