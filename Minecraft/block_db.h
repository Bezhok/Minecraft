#pragma once
#include "pch.h"

namespace World {
	struct DB {
		enum block_id : unsigned char
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
			unsigned char x, y, z;
			enum block_id id;
		};

		static std::unordered_map<enum block_id, std::array<GLuint, 6>> blocks_db;
		static std::unordered_map<enum block_id, sf::Texture> side_textures;

		void load_blocks();
	private:
		void load_block(block_id id, std::string name);
		GLuint load_texture(std::string name);
		GLuint load_block_texture(std::string name);
	};
}
