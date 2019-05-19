#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"

namespace World {
	class Map;
	class Chunk
	{
	private:
		sf::Vector3i m_pos = {0,0,0};
		std::unordered_map<int, DB::block_data> m_chunk;
		int i = 0;

		void bind_texture2positive_x(DB::block_id id);
		void bind_texture2negative_x(DB::block_id id);
		void bind_texture2negative_y(DB::block_id id);
		void bind_texture2positive_y(DB::block_id id);
		void bind_texture2negative_z(DB::block_id id);
		void bind_texture2positive_z(DB::block_id id);

		void add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w);

		void generate_vertexs(World::Map &map);
	public:
		bool is_vertex_created;
		GLbyte *vertex;
		GLuint VBO, VAO;
		Chunk();
		std::unordered_map<int, DB::block_data>& chunk() { return m_chunk; };

		/* return hash of block. x,y,z coordinates of block relative to chunk */
		static int block_hash(int x, int y, int z);

		void upate_vao();
		void update(Map &map);
		int get_points_count() { return std::floorf(i/6.F); };
		void set_pos(const sf::Vector3i &pos) { m_pos = pos; };
		sf::Vector3i get_pos() { return m_pos; };
	};
}