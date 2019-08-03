#include "pch.h"
#include "block_db.h"
#include "Map.h"
#include "ChunkMeshBasic.h"
#include "Chunk.h"
#include "Map.h"

using namespace World;


inline void ChunkMeshBasic::add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w)
{
	m_vertices[m_i++] = x; m_vertices[m_i++] = y; m_vertices[m_i++] = z; m_vertices[m_i++] = w;
	m_i += 2;
}

ChunkMeshBasic::~ChunkMeshBasic()
{
	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}
}

void ChunkMeshBasic::upate_vao()
{
	if (m_is_vertices_created) {
		m_old_i = m_i;

		if (m_i > 0) {
			glBindVertexArray(m_buffers.VAO);
			/**/

			glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
			glBufferData(GL_ARRAY_BUFFER, m_i * sizeof(GLbyte), m_vertices, GL_DYNAMIC_DRAW); //GL_DYNAMIC_DRAW GL_STATIC_DRAW

			// Position attribute
			glVertexAttribPointer(0, 4, GL_UNSIGNED_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// TexCoord attribute
			glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)(4 * sizeof(GLbyte)));
			glEnableVertexAttribArray(1);

			/**/
			glBindVertexArray(0); // Unbind VAO
		}

		delete[] m_vertices;
		--verticies_wasnt_free;
		m_is_vertices_created = false;
	}
}

void ChunkMeshBasic::update_vertices(sf::Mutex& mutex__for_vbo_generation, Map* map)
{
	update_vertices_using_old_buffers();

	//mutex__for_vbo_generation.lock();

	assert(!map->m_global_vao_vbo_buffers.empty());
	m_buffers = map->m_global_vao_vbo_buffers.back();
	map->m_global_vao_vbo_buffers.pop_back();

	//mutex__for_vbo_generation.unlock();
}

void ChunkMeshBasic::update_vertices_using_old_buffers()
{
	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}

	m_i = 0;
	m_vertices = new GLbyte[BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK * 36 * 6];
	m_is_vertices_created = true;
	++verticies_wasnt_free;
}

void World::ChunkMeshBasic::free_buffers(Map* map)
{
	if (get_VAO()) {
		map->m_global_vao_vbo_buffers.push_back(get_buffers());
		m_buffers = Buffers();
	}
}

void ChunkMeshBasic::bind_texture_first_order(block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4] = p.x; m_vertices[m_i + 5] = (p.y + 1);
	m_vertices[m_i + 10] = (p.x + 1); m_vertices[m_i + 11] = (p.y + 1);
	m_vertices[m_i + 16] = p.x; m_vertices[m_i + 17] = p.y;
	m_vertices[m_i + 22] = p.x; m_vertices[m_i + 23] = p.y;
	m_vertices[m_i + 28] = (p.x + 1); m_vertices[m_i + 29] = (p.y + 1);
	m_vertices[m_i + 34] = (p.x + 1); m_vertices[m_i + 35] = p.y;
}

void ChunkMeshBasic::bind_texture_second_order(block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4] = p.x; m_vertices[m_i + 5] = (p.y + 1);
	m_vertices[m_i + 10] = p.x; m_vertices[m_i + 11] = p.y;
	m_vertices[m_i + 16] = (p.x + 1); m_vertices[m_i + 17] = (p.y + 1);
	m_vertices[m_i + 22] = p.x; m_vertices[m_i + 23] = p.y;
	m_vertices[m_i + 28] = (p.x + 1); m_vertices[m_i + 29] = p.y;
	m_vertices[m_i + 34] = (p.x + 1); m_vertices[m_i + 35] = (p.y + 1);
}

inline void ChunkMeshBasic::bind_texture2negative_x(block_id id)
{
	bind_texture_first_order(id, DB::s_atlas_db(id, sides::negative_x));
}

inline void ChunkMeshBasic::bind_texture2positive_x(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db(id, sides::positive_x));
}

inline void ChunkMeshBasic::bind_texture2negative_y(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db(id, sides::negative_y));
}

inline void ChunkMeshBasic::bind_texture2positive_y(block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db(id, sides::positive_y));
}

inline void ChunkMeshBasic::bind_texture2negative_z(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db(id, sides::negative_z));
}

inline void ChunkMeshBasic::bind_texture2positive_z(block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db(id, sides::positive_z));
}

void World::ChunkMeshBasic::generate_verticies4positive_x(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side)
{
	bind_texture2positive_x(id);
	add_byte4(x + BS, y, z, side);
	add_byte4(x + BS, y + BS, z, side);
	add_byte4(x + BS, y, z + BS, side);
	add_byte4(x + BS, y + BS, z, side);
	add_byte4(x + BS, y + BS, z + BS, side);
	add_byte4(x + BS, y, z + BS, side);
}

void World::ChunkMeshBasic::generate_verticies4negative_x(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side)
{
	bind_texture2negative_x(id);
	add_byte4(x, y, z, side);
	add_byte4(x, y, z + BS, side);
	add_byte4(x, y + BS, z, side);
	add_byte4(x, y + BS, z, side);
	add_byte4(x, y, z + BS, side);
	add_byte4(x, y + BS, z + BS, side);
}

void World::ChunkMeshBasic::generate_verticies4negative_y(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side)
{
	bind_texture2negative_y(id);
	add_byte4(x, y, z, side);
	add_byte4(x + BS, y, z, side);
	add_byte4(x, y, z + BS, side);
	add_byte4(x + BS, y, z, side);
	add_byte4(x + BS, y, z + BS, side);
	add_byte4(x, y, z + BS, side);
}

void World::ChunkMeshBasic::generate_verticies4positive_y(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side)
{
	bind_texture2positive_y(id);
	add_byte4(x, y + BS, z, 0);
	add_byte4(x, y + BS, z + BS, 0);
	add_byte4(x + BS, y + BS, z, 0);
	add_byte4(x + BS, y + BS, z, 0);
	add_byte4(x, y + BS, z + BS, 0);
	add_byte4(x + BS, y + BS, z + BS, 0);
}

void World::ChunkMeshBasic::generate_verticies4negative_z(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side)
{
	bind_texture2negative_z(id);
	add_byte4(x, y, z, side);
	add_byte4(x, y + BS, z, side);
	add_byte4(x + BS, y, z, side);
	add_byte4(x, y + BS, z, side);
	add_byte4(x + BS, y + BS, z, side);
	add_byte4(x + BS, y, z, side);
}

 void World::ChunkMeshBasic::generate_verticies4positive_z(uint8_t x, uint8_t y, uint8_t z, block_id id, uint8_t side)
{
	bind_texture2positive_z(id);
	add_byte4(x, y, z + BS, side);
	add_byte4(x + BS, y, z + BS, side);
	add_byte4(x, y + BS, z + BS, side);
	add_byte4(x, y + BS, z + BS, side);
	add_byte4(x + BS, y, z + BS, side);
	add_byte4(x + BS, y + BS, z + BS, side);
}
