#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "game_constants.h"
#include "Chunk.h"
using std::unordered_map;


class World
{
public:
	World();
	~World();

	bool is_block(int mx, int my, int mz);

	const sf::Vector3f m_world_size;
	Chunk m_world[SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE];
private:


};

