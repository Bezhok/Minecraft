#include "pch.h"
#include "block_db.h"


using std::vector;
using namespace World;


DB::AtlasDb DB::s_atlas_db;
std::vector<block_id> DB::s_loaded_blocks;


void World::DB::init_xyz(block_id id, const sf::Vector2i& pos)
{
	for (int i = 0; i < static_cast<int>(sides::SIDES_COUNT); ++i) {
		s_atlas_db(id, static_cast<sides>(i)) = pos;
	}
}

void DB::init_xz(block_id id, const sf::Vector2i& pos)
{
	s_atlas_db(id, sides::negative_x) =
		s_atlas_db(id, sides::positive_x) =
		s_atlas_db(id, sides::negative_z) =
		s_atlas_db(id, sides::positive_z) = pos;
}

void DB::init_y(block_id id, const sf::Vector2i& pos)
{
	s_atlas_db(id, sides::positive_y) =
		s_atlas_db(id, sides::negative_y) = pos;
}

void DB::init_n_y(block_id id, const sf::Vector2i& pos)
{
	s_atlas_db(id, sides::negative_y) = pos;
}

void DB::init_p_y(block_id id, const sf::Vector2i& pos)
{
	s_atlas_db(id, sides::positive_y) = pos;
}

void DB::load_block(block_id id)
{
	s_loaded_blocks.push_back(id);
}

void DB::load_blocks()
{
	block_id id = block_id::Grass;
	load_block(id); //front
	init_xz(id, { 3, 0 });
	init_n_y(id, { 2, 0 });
	init_p_y(id, { 0, 0 });

	id = block_id::Stone;
	load_block(id);
	init_xyz(id, { 1, 0 });

	id = block_id::Dirt;
	load_block(id);
	init_xyz(id, { 2, 0 });

	id = block_id::Oak;
	load_block(id);
	init_xz(id, { 4, 1 });
	init_y(id, { 5, 1 });

	id = block_id::Oak_leafage;
	load_block(id);
	init_xyz(id, { 5, 3 });//4,3

	id = block_id::Water;
	load_block(id);
	init_xyz(id, { 15, 15 });//{ 13, 12 }

	id = block_id::Sand;
	load_block(id);
	init_xyz(id, { 2, 1 });

	id = block_id::Cactus;
	load_block(id);
	init_xz(id, { 6, 4 });
	init_n_y(id, { 7, 4 });
	init_p_y(id, { 5, 4 });

	id = block_id::Oak_wood;
	load_block(id);
	init_xyz(id, { 4, 0 });

}
