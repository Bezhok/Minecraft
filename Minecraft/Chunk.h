#pragma once
#include "pch.h"
#include "game_constants.h"


namespace World {
	class Map;
	enum class block_id :uint8_t;

	class Chunk
	{
	private:
		class ChunkLayer {
		public:
			bool is_all_solid() { return solid_block_count == BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK; };
			void update(block_id type);
		private:
			int solid_block_count = 0;
		};

		sf::Vector3i m_pos = {-1, -1, -1};
		std::array<block_id, BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK> m_data;
		std::array<ChunkLayer, BLOCKS_IN_CHUNK> m_layers;

		int m_i = 0;
		bool m_is_vertices_created = false;
		GLbyte* m_vertices = nullptr;
		GLuint m_VBO=0, m_VAO=0;

		//void bind_texture_second_order(block_id id, int x, int y);
		//void bind_texture_first_order(block_id id, int x, int y);

		void bind_texture_second_order(block_id id, const sf::Vector2i& p);
		void bind_texture_first_order(block_id id,  const sf::Vector2i& p);

		void bind_texture2positive_x(block_id id);
		void bind_texture2negative_x(block_id id);
		void bind_texture2negative_y(block_id id);
		void bind_texture2positive_y(block_id id);
		void bind_texture2negative_z(block_id id);
		void bind_texture2positive_z(block_id id);

		void add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w);

		void generate_vertices();
		bool m_is_init = false;
		World::Map* m_map = nullptr;

		bool should_make_layer(int y);
		ChunkLayer& get_layer(sf::Vector3i pos, int y);

		bool is_layer_solid(sf::Vector3i pos, int y);

		bool m_is_rendering = false;
	public:
		bool is_init() { return m_is_init; };

		bool is_vertices_created() { return m_is_vertices_created; };

		bool is_rendering() { return m_is_rendering; };
		void set_is_rendering(bool flag) { m_is_rendering = flag; };

		bool is_empty();
		Chunk();
		~Chunk();



		void init(const sf::Vector3i& pos, World::Map* map);
		enum block_id get_type(int x, int y, int z);
		void set_type(int x, int y, int z, enum block_id type);

		void upate_vao();
		void update_vertices();

		int get_points_count() { return m_i/6; };
		void set_pos(const sf::Vector3i& pos) { m_pos = pos; m_is_init = true;};
		const sf::Vector3i& get_pos() { return m_pos; };

		GLuint get_VBO() { return m_VBO; };
		GLuint get_VAO() { return m_VAO; };
	};
}