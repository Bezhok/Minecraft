#pragma once
#include "pch.h"

namespace World {
	using std::array;
	using std::unordered_map;
	using std::string;

	struct DB {
		enum block_id : signed int
		{
			Air = 0,
			Grass = 1,
			Dirt = 2,
			Stone = 3,
			concrete_black = 4,
			concrete_blue = 5,
			concrete_brown = 6,
			concrete_cyan = 7,
			concrete_gray = 8,
			concrete_green = 9
		};

		struct block_data {
			int x, y, z;
			enum block_id id;
		};

		static unordered_map<enum block_id, array<GLuint, 6>> blocks_db;
		static unordered_map<enum block_id, sf::Texture> side_textures;

		void load_blocks();
	private:
		void load_block(block_id id, string name);
		GLuint load_texture(std::string name);
		GLuint load_block_texture(string name);
	};
}
