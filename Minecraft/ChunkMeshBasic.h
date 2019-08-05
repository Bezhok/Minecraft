#pragma once
#include "pch.h"
#include "Buffers.h"

using VertexType = GLfloat;


namespace World {
	enum class block_id : uint8_t;
	class Chunk;
	class Map;

	class ChunkMeshBasic {
	private:
		const VertexType BS = 1;

		bool m_is_vertices_created = false;
		VertexType* m_vertices = nullptr;

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

		void add_byte4(VertexType x, VertexType y, VertexType z, VertexType w);
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
		void generate_verticies4positive_x(VertexType x, VertexType y, VertexType z, block_id id, VertexType side);
		void generate_verticies4negative_x(VertexType x, VertexType y, VertexType z, block_id id, VertexType side);
		void generate_verticies4negative_y(VertexType x, VertexType y, VertexType z, block_id id, VertexType side);
		void generate_verticies4positive_y(VertexType x, VertexType y, VertexType z, block_id id, VertexType side);
		void generate_verticies4negative_z(VertexType x, VertexType y, VertexType z, block_id id, VertexType side);
		void generate_verticies4positive_z(VertexType x, VertexType y, VertexType z, block_id id, VertexType side);
	};
}