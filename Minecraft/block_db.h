#pragma once
#include "pch.h"

namespace World {
	struct DB {
	public:
		enum block_id : uint8_t
		{
			Air = 0,
			Grass,
			Dirt,
			Stone,
			concrete_black,
			concrete_blue,
			concrete_brown,
			concrete_cyan,
			concrete_gray,
			concrete_green,

			BLOCK_ID_COUNT
		};

		enum sides : int
		{
			negative_x = 0,
			positive_x,
			negative_y,
			positive_y,
			negative_z,
			positive_z,

			SIDES_COUNT
		};

		struct block_data {
			unsigned char x, y, z;
			enum block_id id;
		};

		
		static std::unordered_map<enum block_id, std::array<sf::Texture, 6>> s_blocks_db;
		static std::unordered_map<enum block_id, sf::Texture> s_side_textures;
		static std::vector<std::vector<sf::Vector2i>> s_atlas_db;
	public:
		/* eponymous */
		void load_blocks();
	private:
		/* eponymous */
		void init_same(block_id id, const sf::Vector2i& pos);
		void load_block(block_id id, std::string name);
	};


}
