#include "pch.h"
#include "block_db.h"

using std::array;
using std::unordered_map;
using std::string;
using std::vector;
using namespace World;

unordered_map<enum DB::block_id, array<sf::Texture, 6>> DB::s_blocks_db;
unordered_map<enum DB::block_id, sf::Texture> DB::s_side_textures;
static const string PATH2BLOCKS_TEXTURES = "resources/textures/blocks/";

void DB::load_block(block_id id, string name)
{
	for (auto &e : s_blocks_db[id]) {
		sf::Image image;

		image.loadFromFile(PATH2BLOCKS_TEXTURES + name);
		image.flipVertically();

		e.loadFromImage(image);
		e.generateMipmap();
	}
	s_side_textures[id].loadFromFile(PATH2BLOCKS_TEXTURES + name);
}

void DB::load_block_side(int i, block_id id, string name)
{
	sf::Image image;

	image.loadFromFile(PATH2BLOCKS_TEXTURES + name);
	image.flipVertically();

	DB::s_blocks_db[id][i].loadFromImage(image);
	DB::s_blocks_db[id][i].generateMipmap();
}

void DB::load_blocks()
{

	load_block_side(0, Grass, "grass_side_carried.png"); //front
	load_block_side(1, Grass, "grass_side_carried.png"); //back
	load_block_side(2, Grass, "grass_side_carried.png"); //left
	load_block_side(3, Grass, "grass_side_carried.png"); //right
	load_block_side(4, Grass, "dirt.png"); //bottom
	load_block_side(5, Grass, "grass_carried.png"); //top
	s_side_textures[Grass].loadFromFile(PATH2BLOCKS_TEXTURES + "grass_side_carried.png");

	load_block(Stone, "stone.png");
	load_block(Dirt, "dirt.png");
	load_block(concrete_black, "concrete_black.png");
	load_block(concrete_blue, "concrete_blue.png");
	load_block(concrete_brown, "concrete_brown.png");
	load_block(concrete_cyan, "concrete_cyan.png");
	load_block(concrete_gray, "concrete_gray.png");
	load_block(concrete_green, "concrete_green.png");
}
