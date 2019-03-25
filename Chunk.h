#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "game_constants.h"
using std::unordered_map;

class Chunk
{
public:
	Chunk();
	~Chunk();
	unordered_map<int, sf::Vector3f>& chunk() { return m_chunk; };

	int block_hash(int x, int y, int z);
private:
	unordered_map<int, sf::Vector3f> m_chunk;


};
