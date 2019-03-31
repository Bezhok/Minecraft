#include "pch.h"
#include "block_db.h"

using std::array;
using std::unordered_map;
using std::string;
using namespace World;

unordered_map<enum DB::block_id, array<GLuint, 6>> DB::blocks_db;

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
	static const string PATH2BLOCKS_TEXTURES = "resources/textures/blocks/";
	return load_texture(PATH2BLOCKS_TEXTURES + name);
}

void DB::load_blocks()
{

	DB::blocks_db[DB::block_id::Grass] = {
		load_block_texture("grass_side_carried.png"), //front
		load_block_texture("grass_side_carried.png"), //back
		load_block_texture("grass_side_carried.png"), //left
		load_block_texture("grass_side_carried.png"), //right
		load_block_texture("dirt.png"), //bottom
		load_block_texture("grass_carried.png") //top
	};

	for (auto &e : DB::blocks_db[DB::block_id::Stone]) {
		e = load_block_texture("stone.png");
	}

	for (auto &e : DB::blocks_db[DB::block_id::Dirt]) {
		e = load_block_texture("dirt.png");
	}
}
