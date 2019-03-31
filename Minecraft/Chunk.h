#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"

namespace World {
	using std::unordered_map;

	class Chunk
	{
	public:
		Chunk();
		~Chunk();
		unordered_map<int, DB::block_data>& chunk() { return m_chunk; };

		int block_hash(int x, int y, int z);
	private:
		unordered_map<int, DB::block_data> m_chunk;
	};
}