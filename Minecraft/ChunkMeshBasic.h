#pragma once
#include "pch.h"
#include "Buffers.h"

const uint8_t divider = 15;
const uint8_t side = divider;

namespace World {
	enum class block_id :uint8_t;
	class Chunk;
	class Map;

	class ChunkMeshBasic {
	private:
		const uint8_t BS = divider;

		bool m_is_vertices_created = false;
		GLbyte* m_vertices = nullptr;

		//verticies count in current time
		int m_i = 0;

		// verticies count before updating 
		int m_old_i = 0;

		Buffers m_buffers;
		Chunk* chunk = nullptr;

	private:
		void bind_texture_second_order(block_id id, const sf::Vector2i& p);
		void bind_texture_first_order(block_id id, const sf::Vector2i& p);
		void bind_texture2positive_x(block_id id);
		void bind_texture2negative_x(block_id id);
		void bind_texture2negative_y(block_id id);
		void bind_texture2positive_y(block_id id);
		void bind_texture2negative_z(block_id id);
		void bind_texture2positive_z(block_id id);

		void add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w);
		ChunkMeshBasic();

	public:
		/* Min */
		ChunkMeshBasic(Chunk* c) : chunk(c) {};
		~ChunkMeshBasic();

		void upate_vao();
		void update_vertices(sf::Mutex& mutex__for_vbo_generation, Map* map);
		void update_vertices_using_old_buffers();
		void free_buffers(Map* map);
		int get_final_points_count() { return m_old_i / 6; };
		int get_current_faces_count() { return m_i / 36; };

		Buffers get_buffers() { return m_buffers; };
		GLuint get_VBO() { return m_buffers.VBO; };
		GLuint get_VAO() { return m_buffers.VAO; };

		/* only for Chunk */
		void generate_verticies4positive_x(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side);
		void generate_verticies4negative_x(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side);
		void generate_verticies4negative_y(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side);
		void generate_verticies4positive_y(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side);
		void generate_verticies4negative_z(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side);
		void generate_verticies4positive_z(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side);
	};
}