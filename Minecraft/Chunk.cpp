#include "pch.h"
#include "Chunk.h"
#include "block_db.h"
#include "Map.h"
using namespace World;

int Chunk::block_hash(int x, int y, int z)
{
	return int(x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE);
}

void Chunk::bind_texture_first_order(DB::block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4] = p.x; m_vertices[m_i + 5] = (p.y + 1);
	m_vertices[m_i + 10] = (p.x + 1); m_vertices[m_i + 11] = (p.y + 1);
	m_vertices[m_i + 16] = p.x; m_vertices[m_i + 17] = p.y;
	m_vertices[m_i + 22] = p.x; m_vertices[m_i + 23] = p.y;
	m_vertices[m_i + 28] = (p.x + 1); m_vertices[m_i + 29] = (p.y + 1);
	m_vertices[m_i + 34] = (p.x + 1); m_vertices[m_i + 35] = p.y;
}

void Chunk::bind_texture_second_order(DB::block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4] = p.x; m_vertices[m_i + 5] = (p.y + 1);
	m_vertices[m_i + 10] = p.x; m_vertices[m_i + 11] = p.y;
	m_vertices[m_i + 16] = (p.x + 1); m_vertices[m_i + 17] = (p.y + 1);
	m_vertices[m_i + 22] = p.x; m_vertices[m_i + 23] = p.y;
	m_vertices[m_i + 28] = (p.x + 1); m_vertices[m_i + 29] = p.y;
	m_vertices[m_i + 34] = (p.x + 1); m_vertices[m_i + 35] = (p.y + 1);
}

void Chunk::bind_texture2negative_x(DB::block_id id)
{
	bind_texture_first_order(id, DB::s_atlas_db[id][DB::sides::negative_x]);
}

void Chunk::bind_texture2positive_x(DB::block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db[id][DB::sides::positive_x]);
}

void Chunk::bind_texture2negative_y(DB::block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db[id][DB::sides::negative_y]);
}

void Chunk::bind_texture2positive_y(DB::block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db[id][DB::sides::positive_y]);
}

void Chunk::bind_texture2negative_z(DB::block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db[id][DB::sides::negative_z]);
}

void Chunk::bind_texture2positive_z(DB::block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db[id][DB::sides::positive_z]);
}

void Chunk::add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w) 
{
	m_vertices[m_i++] = x; m_vertices[m_i++] = y; m_vertices[m_i++] = z; m_vertices[m_i++] = w;
	m_i += 2;
}

void Chunk::generate_vertices(World::Map& map)
{
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {

				// global pos
				int X = (i + m_pos.x * CHUNK_SIZE);
				int Y = (j + m_pos.y * CHUNK_SIZE);
				int Z = (k + m_pos.z * CHUNK_SIZE);

				if (map.is_block(X, Y, Z)) {
					// local(in chunk) pos
					int	x = i * BLOCK_SIZE;
					int	y = j * BLOCK_SIZE;
					int	z = k * BLOCK_SIZE;

					float BS = BLOCK_SIZE;

					uint8_t side = 1;
					auto id = m_chunk(i, j, k);

					if (!map.is_block(X - 1, Y, Z)) {
						bind_texture2negative_x(id);
						add_byte4(x, y, z, side);
						add_byte4(x, y, z + BS, side);
						add_byte4(x, y + BS, z, side);
						add_byte4(x, y + BS, z, side);
						add_byte4(x, y, z + BS, side);
						add_byte4(x, y + BS, z + BS, side);

					}
					if (!map.is_block(X + 1, Y, Z)) {
						bind_texture2positive_x(id);
						add_byte4(x + BS, y, z, side);
						add_byte4(x + BS, y + BS, z, side);
						add_byte4(x + BS, y, z + BS, side);
						add_byte4(x + BS, y + BS, z, side);
						add_byte4(x + BS, y + BS, z + BS, side);
						add_byte4(x + BS, y, z + BS, side);

					}

					if (!map.is_block(X, Y - 1, Z)) {
						bind_texture2negative_y(id);
						add_byte4(x, y, z, -side);
						add_byte4(x + BS, y, z, -side);
						add_byte4(x, y, z + BS, -side);
						add_byte4(x + BS, y, z, -side);
						add_byte4(x + BS, y, z + BS, -side);
						add_byte4(x, y, z + BS, -side);

					}

					if (!map.is_block(X, Y + 1, Z)) {
						bind_texture2positive_y(id);
						add_byte4(x, y + BS, z, -side);
						add_byte4(x, y + BS, z + BS, -side);
						add_byte4(x + BS, y + BS, z, -side);
						add_byte4(x + BS, y + BS, z, -side);
						add_byte4(x, y + BS, z + BS, -side);
						add_byte4(x + BS, y + BS, z + BS, -side);

					}

					if (!map.is_block(X, Y, Z - 1)) {
						bind_texture2negative_z(id);
						add_byte4(x, y, z, side);
						add_byte4(x, y + BS, z, side);
						add_byte4(x + BS, y, z, side);
						add_byte4(x, y + BS, z, side);
						add_byte4(x + BS, y + BS, z, side);
						add_byte4(x + BS, y, z, side);

					}

					if (!map.is_block(X, Y, Z + 1)) {
						bind_texture2positive_z(id);
						add_byte4(x, y, z + BS, side);
						add_byte4(x + BS, y, z + BS, side);
						add_byte4(x, y + BS, z + BS, side);
						add_byte4(x, y + BS, z + BS, side);
						add_byte4(x + BS, y, z + BS, side);
						add_byte4(x + BS, y + BS, z + BS, side);

					}
				}
			}
		}
	}
}

Chunk::Chunk()
{
	//m_chunk.reserve(16 * 16 * 16);
}


void World::Chunk::upate_vao()
{
	if (m_is_vertices_created) {
		glBindVertexArray(m_VAO);
		/**/

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_i * sizeof(GLbyte), m_vertices, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW

		// Position attribute
		glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// TexCoord attribute
		glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)(4 * sizeof(GLbyte)));
		glEnableVertexAttribArray(1);

		/**/
		glBindVertexArray(0); // Unbind VAO
		

		delete[] m_vertices;
		--verticies_wasnt_free;
		m_is_vertices_created = false;
	}
}

void Chunk::update_vertices(World::Map& map) {

	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}
	m_is_vertices_created = false;

	auto this_chunk_iter = map.m_free_vbo_chunks.find(this);
	bool is_finded = this_chunk_iter != map.m_free_vbo_chunks.end();
	if (is_finded && !map.is_chunk_edited()) {
		map.m_free_vbo_chunks.erase(this_chunk_iter);
	}
	else {
		m_i = 0;
		m_is_vertices_created = true;
		m_vertices = new GLbyte[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE * 36 * 6];
		generate_vertices(map);
		++verticies_wasnt_free;

		if (map.m_free_vbo_chunks.size()) {

			if (is_finded && map.is_chunk_edited()) {
				map.m_free_vbo_chunks.erase(this_chunk_iter);			
			}
			else {
				auto this_chunk_iter = map.m_free_vbo_chunks.begin();

				m_VAO = (*this_chunk_iter)->m_VAO;
				m_VBO = (*this_chunk_iter)->m_VBO;
				map.m_free_vbo_chunks.erase(this_chunk_iter);
			}
		}
		else {
			assert(!map.m_global_vao_vbo_buffers.empty());
			m_VAO = map.m_global_vao_vbo_buffers.back().first;
			m_VBO = map.m_global_vao_vbo_buffers.back().second;

			map.m_global_vao_vbo_buffers.pop_back();
		}
	}
}
