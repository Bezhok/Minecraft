#pragma once
#include "pch.h"

namespace World {
	struct DB {
	public:
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

		static std::unordered_map<enum block_id, std::array<sf::Texture, 6>> s_blocks_db;
		static std::unordered_map<enum block_id, sf::Texture> s_side_textures;

	public:
		/* eponymous */
		void load_blocks();
	private:
		/* eponymous */
		void load_block_side(int i, block_id id, std::string name);
		void load_block(block_id id, std::string name);
	};
}
