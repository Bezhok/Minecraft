#include "pch.h"
#include "block_db.h"

using std::array;
using std::unordered_map;
using std::string;
using std::vector;
using namespace World;




unordered_map<block_id, array<sf::Texture, 6>> DB::s_blocks_db;
unordered_map<block_id, sf::Texture> DB::s_side_textures;


std::unordered_map<block_id, std::unordered_map<sides, sf::Vector2i>> DB::s_atlas_db;
//std::vector<std::vector<sf::Vector2i>> DB::s_atlas_db(static_cast<uint8_t>(block_id::BLOCK_ID_COUNT), std::vector<sf::Vector2i>(6));

static const string PATH2BLOCKS_TEXTURES = "resources/textures/blocks/";

void World::DB::init_same(block_id id, const sf::Vector2i& pos)
{
	for (int i = 0; i < static_cast<int>(sides::SIDES_COUNT); ++i) {
		s_atlas_db[id][static_cast<sides>(i)] = pos;
	}
}

void DB::load_block(block_id id, string name)
{
	s_side_textures[id].loadFromFile(PATH2BLOCKS_TEXTURES + name);
}



void DB::load_blocks()
{

	load_block(block_id::Grass, "grass_carried.png"); //front
	s_atlas_db[block_id::Grass][sides::negative_x] =
		s_atlas_db[block_id::Grass][sides::positive_x] =
		s_atlas_db[block_id::Grass][sides::negative_z] =
		s_atlas_db[block_id::Grass][sides::positive_z] = {3, 0};

	s_atlas_db[block_id::Grass][sides::positive_y] = { 0, 0 };
	s_atlas_db[block_id::Grass][sides::negative_y] = { 2, 0 };

	load_block(block_id::Stone, "stone.png");
	init_same(block_id::Stone, { 1, 0 });
	load_block(block_id::Dirt, "dirt.png");
	init_same(block_id::Dirt, { 2, 0 });
	//load_block(concrete_black, "concrete_black.png");
	//load_block(concrete_blue, "concrete_blue.png");
	//load_block(concrete_brown, "concrete_brown.png");
	//load_block(concrete_cyan, "concrete_cyan.png");
	//load_block(concrete_gray, "concrete_gray.png");
	//load_block(concrete_green, "concrete_green.png");
}
