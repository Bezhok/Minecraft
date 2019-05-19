#include "pch.h"
#include "Chunk.h"
#include "block_db.h"
#include "Map.h"
using namespace World;
//GLbyte vertex[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE * 36 * 6];

int Chunk::block_hash(int x, int y, int z)
{
	return int(x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE);
}

void Chunk::bind_texture2negative_x(DB::block_id id)
{
	int x = 2; int y = 1;
	vertex[i+4] = x; vertex[i+5] = (y + 1);
	vertex[i+10] = (x + 1); vertex[i+11] = (y + 1);
	vertex[i+16] = x; vertex[i+17] = y;
	vertex[i+22] = x; vertex[i+23] = y;
	vertex[i+28] = (x + 1); vertex[i+29] = (y + 1);
	vertex[i+34] = (x + 1); vertex[i+35] = y;
}

void Chunk::bind_texture2positive_x(DB::block_id id) {
	int x = 2; int y = 1;
	vertex[i+4] = x; vertex[i+5] = (y + 1);
	vertex[i+10] = x; vertex[i+11] = y;
	vertex[i+16] = (x + 1); vertex[i+17] = (y + 1);
	vertex[i+22] = x; vertex[i+23] = y;
	vertex[i+28] = (x + 1); vertex[i+29] = y;
	vertex[i+34] = (x + 1); vertex[i+35] = (y + 1);
}

void Chunk::bind_texture2negative_y(DB::block_id id) {
	bind_texture2positive_x(id);
}

void Chunk::bind_texture2positive_y(DB::block_id id) {
	bind_texture2negative_x(id);
}

void Chunk::bind_texture2negative_z(DB::block_id id) {
	bind_texture2positive_x(id);
}

void Chunk::bind_texture2positive_z(DB::block_id id) {
	bind_texture2negative_x(id);
}


void Chunk::add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w) 
{
	vertex[i++] = x; vertex[i++] = y; vertex[i++] = z; vertex[i++] = w;
	i += 2;
}

void Chunk::generate_vertexs(World::Map &map)
{
	for (const auto &e : m_chunk) {

		// global pos
		int X = (e.second.x + m_pos.x * CHUNK_SIZE);
		int Y = (e.second.y + m_pos.y * CHUNK_SIZE);
		int Z = (e.second.z + m_pos.z * CHUNK_SIZE);

		// local(in chunk) pos
		int	x = (e.second.x) * BLOCK_SIZE;
		int	y = (e.second.y) * BLOCK_SIZE;
		int	z = (e.second.z) * BLOCK_SIZE;

		float BS = BLOCK_SIZE;

		uint8_t side = 1;
		auto id = e.second.id;

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
			add_byte4(x, y, z, side);
			add_byte4(x + BS, y, z, side);
			add_byte4(x, y, z + BS, side);
			add_byte4(x + BS, y, z, side);
			add_byte4(x + BS, y, z + BS, side);
			add_byte4(x, y, z + BS, side);

		}

		if (!map.is_block(X, Y + 1, Z)) {
			bind_texture2positive_y(id);
			add_byte4(x, y + BS, z, -BS);
			add_byte4(x, y + BS, z + BS, -BS);
			add_byte4(x + BS, y + BS, z, -BS);
			add_byte4(x + BS, y + BS, z, -BS);
			add_byte4(x, y + BS, z + BS, -BS);
			add_byte4(x + BS, y + BS, z + BS, -BS);

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

Chunk::Chunk()
{

}


void World::Chunk::upate_vao()
{
	if (is_vertex_created) {
		glBindVertexArray(VAO);
		/**/

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, i * sizeof(GLbyte), vertex, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW

		// Position attribute
		glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// TexCoord attribute
		glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)(4 * sizeof(GLbyte)));
		glEnableVertexAttribArray(1);

		/**/
		glBindVertexArray(0); // Unbind VAO

		delete[] vertex;
		is_vertex_created = false;
	}
}

void Chunk::update(World::Map &map) {

	is_vertex_created = false;
	auto this_chunk_iter = map.m_free_vbo_chunks.find(this);
	bool is_finded = this_chunk_iter != map.m_free_vbo_chunks.end();
	if (is_finded && !map.is_chunk_edited()) {
		map.m_free_vbo_chunks.erase(this_chunk_iter);
	}
	else {
		i = 0;
		//GLbyte vvvv[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE * 36 * 6];
		//vertex = vvvv;
		is_vertex_created = true;
		vertex = new GLbyte[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE * 36 * 6];
		generate_vertexs(map);

		if (map.m_free_vbo_chunks.size()) {

			if (is_finded && map.is_chunk_edited()) {
				map.m_free_vbo_chunks.erase(this_chunk_iter);			
			}
			else {
				auto this_chunk_iter = map.m_free_vbo_chunks.begin();

				VAO = (*this_chunk_iter)->VAO;
				VBO = (*this_chunk_iter)->VBO;
				map.m_free_vbo_chunks.erase(this_chunk_iter);
			}
		}
		else {
			assert(!map.m_global_vao_vbo_buffers.empty());
			VAO = map.m_global_vao_vbo_buffers.back().first;
			VBO = map.m_global_vao_vbo_buffers.back().second;

			map.m_global_vao_vbo_buffers.pop_back();
			//glGenVertexArrays(1, &VAO);
			//glGenBuffers(1, &VBO);
		}



		//glBindVertexArray(VAO);
		///**/

		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, i * sizeof(GLbyte), vertex, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW

		//// Position attribute
		//glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)0);
		//glEnableVertexAttribArray(0);

		//// TexCoord attribute
		//glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, 6 * sizeof(GLbyte), (GLvoid*)(4 * sizeof(GLbyte)));
		//glEnableVertexAttribArray(1);

		///**/
		//glBindVertexArray(0); // Unbind VAO


		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, i * sizeof(GLbyte), vertex, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW




	}
}
