#include "pch.h"
#include "Chunk.h"
#include "block_db.h"

using namespace World;

int Chunk::block_hash(int x, int y, int z)
{
	return int(x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE);
}
