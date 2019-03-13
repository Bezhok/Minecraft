#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "World.h"
#include "Chunk.h"

using std::unordered_map;

World::World()
{
	for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
		for (int j = 0; j < SUPER_CHUNK_SIZE; ++j) {
			for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
				m_world[i][j][k] = Chunk();
			}
		}
	}
}

World::~World()
{
}

bool World::is_block(int x, int y, int z)//block x,y,z in chunk
{
	if (x < 0 || y < 0 || z < 0
		|| x / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| y / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| z / CHUNK_SIZE >= SUPER_CHUNK_SIZE) return false;

	Chunk &e = m_world[x / CHUNK_SIZE][y / CHUNK_SIZE][z / CHUNK_SIZE];
	unordered_map<int, sf::Vector3f>::iterator it = e.get_chunk().find(e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE));

	if (e.get_chunk().end() == it)
		return false;
	else
		return true;
}
