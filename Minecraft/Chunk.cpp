#include "pch.h"
#include "Chunk.h"
#include "block_db.h"

using std::unordered_map;
using namespace World;

Chunk::Chunk()
{
	for (int i = 1; i < CHUNK_SIZE; ++i) {
		for (int k = 1; k < CHUNK_SIZE; ++k) {
			DB::block_data block = { i, 1, k, DB::block_id::Grass };

			block.id = (DB::block_id)((signed int)(rand() % 2 + 1));
			m_chunk[block_hash(i, 1, k)] = block;
		}
	}
}

Chunk::~Chunk()
{
}

int Chunk::block_hash(int x, int y, int z)
{
	return int(x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE);
}
