#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "Chunk.h"

using std::unordered_map;

Chunk::Chunk()
{
	for (int i = 1; i < CHUNK_SIZE; ++i) {
		for (int k = 1; k < CHUNK_SIZE; ++k) {
			m_chunk[block_hash(i, 1, k)] = sf::Vector3f(i, 1, k);
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
