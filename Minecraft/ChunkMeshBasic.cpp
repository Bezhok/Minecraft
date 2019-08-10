#include "pch.h"
#include "block_db.h"
#include "Map.h"
#include "ChunkMeshBasic.h"
#include "Chunk.h"
#include "Map.h"

using namespace World;


inline void ChunkMeshBasic::add_byte4(VertexType x, VertexType y, VertexType z, VertexType w)
{
	m_vertices[m_i++] = x; m_vertices[m_i++] = y; m_vertices[m_i++] = z; m_vertices[m_i++] = w;
	m_i += 5;
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
		m_old_i = m_i;// = 0;

		if (m_i > 0) {

			if (get_VAO() == 0) {
				glGenVertexArrays(1, &m_buffers.VAO);
				glGenBuffers(1, &m_buffers.VBO);
			}

			glBindVertexArray(m_buffers.VAO);
			/**/

			glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
			glBufferData(GL_ARRAY_BUFFER, m_i * sizeof(VertexType), m_vertices, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW GL_STATIC_DRAW

			// Position attribute
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(VertexType), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// TexCoord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(VertexType), (GLvoid*)(4 * sizeof(VertexType)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(VertexType), (GLvoid*)(6 * sizeof(VertexType)));
			glEnableVertexAttribArray(2);

			/**/
			glBindVertexArray(0); // Unbind VAO
		}

		delete[] m_vertices;
		--verticies_wasnt_free;
		m_is_vertices_created = false;
	}
}

void ChunkMeshBasic::update_vertices()
{
	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}

	m_i = 0;
	m_vertices = new VertexType[BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK * 36 * 9];
	m_is_vertices_created = true;
	++verticies_wasnt_free;
}

void World::ChunkMeshBasic::free_buffers(Map* map)
{
	if (get_VAO()!=0) {
		map->m_should_be_freed_buffers.push_back(get_buffers());
		m_buffers = Buffers();
	}
}

//TODO divide by BLOCK_RESOLUTION
void ChunkMeshBasic::bind_texture_first_order(block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4]     =  p.x;      m_vertices[m_i + 5]     = (p.y + 1);
	m_vertices[m_i + 10+3]  = (p.x + 1); m_vertices[m_i + 11+3]  = (p.y + 1);
	m_vertices[m_i + 16+6]  =  p.x;      m_vertices[m_i + 17+6]  =  p.y;
	m_vertices[m_i + 22+9]  =  p.x;      m_vertices[m_i + 23+9]  =  p.y;
	m_vertices[m_i + 28+12] = (p.x + 1); m_vertices[m_i + 29+12] = (p.y + 1);
	m_vertices[m_i + 34+15] = (p.x + 1); m_vertices[m_i + 35+15] =  p.y;
}

void ChunkMeshBasic::bind_texture_second_order(block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4]    = p.x;        m_vertices[m_i + 5]     = (p.y + 1);
	m_vertices[m_i + 10+3] = p.x;        m_vertices[m_i + 11+3]  = p.y;
	m_vertices[m_i + 16+6] = (p.x + 1);  m_vertices[m_i + 17+6]  = (p.y + 1);
	m_vertices[m_i + 22+9] = p.x;        m_vertices[m_i + 23+9]  = p.y;
	m_vertices[m_i + 28+12] = (p.x + 1); m_vertices[m_i + 29+12] = p.y;
	m_vertices[m_i + 34+15] = (p.x + 1); m_vertices[m_i + 35+15] = (p.y + 1);
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


void World::ChunkMeshBasic::add_normal(VertexType x, VertexType y, VertexType z)
{
	for (int i = 6; i <= 51; i+=9) {
		m_vertices[m_i + i] = x; m_vertices[m_i + i+1] = y; m_vertices[m_i + i+2] = z;
	}
}


void World::ChunkMeshBasic::generate_verticies4positive_x(VertexType x, VertexType y, VertexType z, block_id id, VertexType side)
{
	add_normal(1, 0, 0);
	bind_texture2positive_x(id);
	add_byte4(x + BS, y,      z, side);
	add_byte4(x + BS, y + BS, z, side);
	add_byte4(x + BS, y,      z + BS, side);
	add_byte4(x + BS, y + BS, z, side);
	add_byte4(x + BS, y + BS, z + BS, side);
	add_byte4(x + BS, y,      z + BS, side);
}

void World::ChunkMeshBasic::generate_verticies4negative_x(VertexType x, VertexType y, VertexType z, block_id id, VertexType side)
{
	add_normal(-1, 0, 0);
	bind_texture2negative_x(id);
	add_byte4(x, y,      z, side);
	add_byte4(x, y,      z + BS, side);
	add_byte4(x, y + BS, z, side);
	add_byte4(x, y + BS, z, side);
	add_byte4(x, y,      z + BS, side);
	add_byte4(x, y + BS, z + BS, side);
}

void World::ChunkMeshBasic::generate_verticies4negative_y(VertexType x, VertexType y, VertexType z, block_id id, VertexType side)
{
	add_normal(0, -1, 0);
	bind_texture2negative_y(id);
	add_byte4(x,      y, z, 1);
	add_byte4(x + BS, y, z, 1);
	add_byte4(x,      y, z + BS, 1);
	add_byte4(x + BS, y, z, 1);
	add_byte4(x + BS, y, z + BS, 1);
	add_byte4(x,      y, z + BS, 1);
}

void World::ChunkMeshBasic::generate_verticies4positive_y(VertexType x, VertexType y, VertexType z, block_id id, VertexType side)
{
	add_normal(0, 1, 0);
	bind_texture2positive_y(id);
	add_byte4(x,      y + BS, z, 0);
	add_byte4(x,      y + BS, z + BS, 0);
	add_byte4(x + BS, y + BS, z, 0);
	add_byte4(x + BS, y + BS, z, 0);
	add_byte4(x,      y + BS, z + BS, 0);
	add_byte4(x + BS, y + BS, z + BS, 0);
}

void World::ChunkMeshBasic::generate_verticies4negative_z(VertexType x, VertexType y, VertexType z, block_id id, VertexType side)
{
	add_normal(0, 0, -1);
	bind_texture2negative_z(id);
	add_byte4(x,      y,      z, side);
	add_byte4(x,      y + BS, z, side);
	add_byte4(x + BS, y,      z, side);
	add_byte4(x,      y + BS, z, side);
	add_byte4(x + BS, y + BS, z, side);
	add_byte4(x + BS, y,      z, side);
}

 void World::ChunkMeshBasic::generate_verticies4positive_z(VertexType x, VertexType y, VertexType z, block_id id, VertexType side)
{
	add_normal(0, 0, 1);
	bind_texture2positive_z(id);
	add_byte4(x,      y,      z + BS, side);
	add_byte4(x + BS, y,      z + BS, side);
	add_byte4(x,      y + BS, z + BS, side);
	add_byte4(x,      y + BS, z + BS, side);
	add_byte4(x + BS, y,      z + BS, side);
	add_byte4(x + BS, y + BS, z + BS, side);
}
