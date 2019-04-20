#include "pch.h"
#include "Chunk.h"
#include "block_db.h"

using std::unordered_map;
using namespace World;

Chunk::Chunk()
{
}

Chunk::~Chunk()
{
}

int Chunk::block_hash(int x, int y, int z)
{
	return int(x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE);
}
