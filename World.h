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
	bool create_block(int x, int y, int z);
	bool delete_block(int x, int y, int z);

	const sf::Vector3f m_world_size;

	bool m_redraw_chunk = false;
	sf::Vector3i m_edited_chunk_coord;
	Chunk m_world[SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE];
private:


};

