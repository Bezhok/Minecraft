#include "pch.h"
#include "Block.h"
#include "Map.h"
#include "game_constants.h"
#include "block_db.h"

using std::array;
using namespace World;



Block::Block(World::Map *map) 
{
	m_map = map;



}

void Block::bind_textures(DB::block_id id, const sf::Vector3f &pos)
{

}
