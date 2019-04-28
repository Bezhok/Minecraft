#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"

namespace World {
	class Chunk
	{
	private:
		std::unordered_map<int, DB::block_data> m_chunk;

	public:
		std::unordered_map<int, DB::block_data>& chunk() { return m_chunk; };

		/* return hash of block. x,y,z coordinates of block relative to chunk */
		static int block_hash(int x, int y, int z);
	};
}