#include "pch.h"
#include "block_db.h"

using std::vector;
using namespace World;

DB::AtlasDb DB::s_atlas_db;
std::vector<BlockType> DB::s_loaded_blocks;

void World::DB::init_xyz(BlockType id, const sf::Vector2i &pos) {
    for (int i = 0; i < static_cast<int>(Side::SIDES_COUNT); ++i) {
        s_atlas_db(id, static_cast<Side>(i)) = pos;
    }
}

void DB::init_xz(BlockType id, const sf::Vector2i &pos) {
    s_atlas_db(id, Side::negative_x) =
    s_atlas_db(id, Side::positive_x) =
    s_atlas_db(id, Side::negative_z) =
    s_atlas_db(id, Side::positive_z) = pos;
}

void DB::init_y(BlockType id, const sf::Vector2i &pos) {
    s_atlas_db(id, Side::positive_y) =
    s_atlas_db(id, Side::negative_y) = pos;
}

void DB::init_n_y(BlockType id, const sf::Vector2i &pos) {
    s_atlas_db(id, Side::negative_y) = pos;
}

void DB::init_p_y(BlockType id, const sf::Vector2i &pos) {
    s_atlas_db(id, Side::positive_y) = pos;
}

void DB::load_block(BlockType id) {
    s_loaded_blocks.push_back(id);
}

void DB::load_blocks() {
    BlockType id = BlockType::Grass;
    load_block(id); //front
    init_xz(id, {3, 0});
    init_n_y(id, {2, 0});
    init_p_y(id, {0, 0});

    id = BlockType::Stone;
    load_block(id);
    init_xyz(id, {1, 0});

    id = BlockType::Dirt;
    load_block(id);
    init_xyz(id, {2, 0});

    id = BlockType::Oak;
    load_block(id);
    init_xz(id, {4, 1});
    init_y(id, {5, 1});

    id = BlockType::Oak_leafage;
    load_block(id);
    init_xyz(id, {5, 3});//4,3

    id = BlockType::Water;
    load_block(id);
    init_xyz(id, {15, 15});//{ 13, 12 }

    id = BlockType::Sand;
    load_block(id);
    init_xyz(id, {2, 1});

    id = BlockType::Cactus;
    load_block(id);
    init_xz(id, {6, 4});
    init_n_y(id, {7, 4});
    init_p_y(id, {5, 4});

    id = BlockType::Oak_wood;
    load_block(id);
    init_xyz(id, {4, 0});
}
