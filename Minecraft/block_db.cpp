#include "pch.h"
#include "block_db.h"

using std::array;
using std::unordered_map;
using std::string;
using namespace World;

unordered_map<enum DB::block_id, array<GLuint, 6>> DB::blocks_db;
unordered_map<enum DB::block_id, sf::Texture> DB::side_textures;
static const string PATH2BLOCKS_TEXTURES = "resources/textures/blocks/";


GLuint DB::load_texture(string name)
{
	GLuint texture = 0;

	sf::Image image;
	image.loadFromFile(name);
	image.flipVertically();

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(
		GL_TEXTURE_2D,
		GL_RGBA,
		image.getSize().x,
		image.getSize().y,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image.getPixelsPtr()
	);

	// scope texture view
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);

	// delete "borders"
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
}

GLuint DB::load_block_texture(string name)
{
	return load_texture(PATH2BLOCKS_TEXTURES + name);
}

void DB::load_block(block_id id, string name)
{
	for (auto &e : blocks_db[id]) {
		e = load_block_texture(name);
	}
	side_textures[id].loadFromFile(PATH2BLOCKS_TEXTURES + name);
}

void DB::load_blocks()
{

	blocks_db[Grass] = {
		load_block_texture("grass_side_carried.png"), //front
		load_block_texture("grass_side_carried.png"), //back
		load_block_texture("grass_side_carried.png"), //left
		load_block_texture("grass_side_carried.png"), //right
		load_block_texture("dirt.png"), //bottom
		load_block_texture("grass_carried.png") //top
	};
	side_textures[Grass].loadFromFile(PATH2BLOCKS_TEXTURES + "grass_side_carried.png");

	load_block(Stone, "stone.png");
	load_block(Dirt, "dirt.png");
	load_block(concrete_black, "concrete_black.png");
	load_block(concrete_blue, "concrete_blue.png");
	load_block(concrete_brown, "concrete_brown.png");
	load_block(concrete_cyan, "concrete_cyan.png");
	load_block(concrete_gray, "concrete_gray.png");
	load_block(concrete_green, "concrete_green.png");
}
