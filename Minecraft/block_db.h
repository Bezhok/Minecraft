#pragma once
#include "pch.h"

namespace World {
	enum class block_id : uint8_t
	{
		Air = 0,
		Glass,
		Water,

		Sand,
		Grass,
		Dirt,
		Stone,
		Oak,
		Oak_leafage,
		Cactus,

		concrete_black,
		concrete_blue,
		concrete_brown,
		concrete_cyan,
		concrete_gray,
		concrete_green,

		BLOCK_ID_COUNT
	};

	enum class sides : int
	{
		negative_x = 0,
		positive_x,
		negative_y,
		positive_y,
		negative_z,
		positive_z,

		SIDES_COUNT
	};


	struct DB {
	public:		
		static std::unordered_map<block_id, std::array<sf::Texture, 6>> s_blocks_db;
		static std::unordered_map<block_id, sf::Texture> s_side_textures;
		static std::unordered_map<block_id, std::unordered_map<sides, sf::Vector2i>> s_atlas_db;
	public:
		/* eponymous */
		void load_blocks();
	private:
		/* eponymous */
		void init_xyz(block_id id, const sf::Vector2i& pos);
		void load_block(block_id id, std::string name);

		void init_xz(block_id id, const sf::Vector2i& pos);
		void init_y(block_id id, const sf::Vector2i& pos);
		void init_n_y(block_id id, const sf::Vector2i& pos);
		void init_p_y(block_id id, const sf::Vector2i& pos);
	};


}
