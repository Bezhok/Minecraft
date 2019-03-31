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
			Crafting_table = 4
		};

		struct block_data {
			int x, y, z;
			enum block_id id;
		};

		static unordered_map<enum block_id, array<GLuint, 6>> blocks_db;
		void load_blocks();
	private:
		GLuint load_texture(std::string name);
		GLuint load_block_texture(string name);
	};
}
