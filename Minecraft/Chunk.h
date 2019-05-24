#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"

namespace World {
	class Map;
	class Chunk
	{
		class array3D {
			
		public:
			std::vector<enum DB::block_id> m_data;
			array3D(enum DB::block_id init = DB::block_id::Air) :
				m_data(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE, init)
			{}
			enum DB::block_id& operator()(int x, int y, int z) {
				return m_data.at(x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE);
			}
		};


	private:
		sf::Vector3i m_pos = {0,0,0};
		//std::unordered_map<int, DB::block_data> m_chunk;
		array3D m_chunk;



		int m_i = 0;
		bool m_is_vertices_created = false;
		GLbyte *m_vertices;
		GLuint m_VBO, m_VAO;


		void bind_texture2positive_x(DB::block_id id);
		void bind_texture2negative_x(DB::block_id id);
		void bind_texture2negative_y(DB::block_id id);
		void bind_texture2positive_y(DB::block_id id);
		void bind_texture2negative_z(DB::block_id id);
		void bind_texture2positive_z(DB::block_id id);

		void add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w);

		void generate_vertices(World::Map& map);
	public:

		bool is_empty()
		{
			for (const auto &e : m_chunk.m_data)
			{
				if (e) {
					return false;
				}
			}

			return true;
		};
		Chunk();
		auto& chunk() { return m_chunk; };

		/* return hash of block. x,y,z coordinates of block relative to chunk */
		static int block_hash(int x, int y, int z);

		void upate_vao();
		void update_vertices(Map& map);
		int get_points_count() { return std::floorf(m_i/6.F); };
		void set_pos(const sf::Vector3i& pos) { m_pos = pos; };
		const sf::Vector3i& get_pos() { return m_pos; };

		GLuint get_VBO() { return m_VBO; };
		GLuint get_VAO() { return m_VAO; };
	};
}