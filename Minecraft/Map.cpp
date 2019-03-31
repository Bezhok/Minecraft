#include "pch.h"
#include "Map.h"
#include "Chunk.h"
#include "block_db.h"

using std::unordered_map;
using namespace World;

Map::Map()
{
	for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
		for (int j = 0; j < SUPER_CHUNK_SIZE; ++j) {
			for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
				m_world[i][j][k] = Chunk();
			}
		}
	}
}

Map::~Map()
{
}

bool Map::is_block(int x, int y, int z)//block x,y,z in chunk
{
	if (x < 0 || y < 0 || z < 0
		|| x / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| y / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| z / CHUNK_SIZE >= SUPER_CHUNK_SIZE) return false;

	Chunk &e = m_world[x / CHUNK_SIZE][y / CHUNK_SIZE][z / CHUNK_SIZE];
	auto it = e.chunk().find(e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE));

	if (e.chunk().end() == it)
		return false;
	else
		return true;
}

bool Map::create_block(int x, int y, int z, DB::block_id id)
{
	if (x < 0 || y < 0 || z < 0
		|| x / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| y / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| z / CHUNK_SIZE >= SUPER_CHUNK_SIZE) return false;

	Chunk &e = m_world[x / CHUNK_SIZE][y / CHUNK_SIZE][z / CHUNK_SIZE];
	auto it = e.chunk().find(e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE));

	if (e.chunk().end() == it) {
		e.chunk()[e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE)] = { x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE, id };
		m_edited_chunk_coord = { x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE };
		m_redraw_chunk = true;
		return true;
	}
	else {
		return false;
	}
}

bool Map::delete_block(int x, int y, int z)
{
	if (x < 0 || y < 0 || z < 0
		|| x / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| y / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| z / CHUNK_SIZE >= SUPER_CHUNK_SIZE) return false;

	Chunk &e = m_world[x / CHUNK_SIZE][y / CHUNK_SIZE][z / CHUNK_SIZE];
	auto it = e.chunk().find(e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE));

	if (e.chunk().end() == it)
		return false;
	else {
		m_edited_chunk_coord = { x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE };
		e.chunk().erase(it);
		m_redraw_chunk = true;
		return true;
	}
}
