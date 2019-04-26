#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"

namespace World {
	class Chunk
	{
	public:
		Chunk();
		~Chunk();
		std::unordered_map<int, DB::block_data>& chunk() { return m_chunk; };

		int block_hash(int x, int y, int z);
	private:
		std::unordered_map<int, DB::block_data> m_chunk;
	};
}