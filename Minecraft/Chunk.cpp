#include "pch.h"

#include "block_db.h"
#include "Map.h"
#include "Chunk.h"
using namespace World;


void World::Chunk::init()
{
	if (m_data.empty()) {
		m_data.resize(BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK, block_id::Air);

		//m_layers.resize(BLOCKS_IN_CHUNK);
	}
}

block_id World::Chunk::get_type(int x, int y, int z)
{
	if (m_data.empty()) {
		return block_id::Air;
	}

	return m_data.at(x + y * BLOCKS_IN_CHUNK + z * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK);
}

void World::Chunk::set_type(int x, int y, int z, block_id type)
{
	m_data.at(x + y * BLOCKS_IN_CHUNK + z * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK) = type;
	m_layers[y].update(type);
}


bool Chunk::is_empty()
{
	// air
	for (block_id id : m_data)
	{
		if (id != block_id::Air) {
			return false;
		}
	}

	//empty
	return m_data.empty();
};

void Chunk::add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w) 
{
	m_vertices[m_i++] = x; m_vertices[m_i++] = y; m_vertices[m_i++] = z; m_vertices[m_i++] = w;
	m_i += 2;
}

void Chunk::generate_vertices(World::Map& map)
{
	for (int j = 0; j < BLOCKS_IN_CHUNK; ++j) {
		if (!should_make_layer(j))
			continue;
			for (int i = 0; i < BLOCKS_IN_CHUNK; ++i) {
			for (int k = 0; k < BLOCKS_IN_CHUNK; ++k) {

				// global pos
				int X = (i + Map::chunk_coord2block_coord(m_pos.x));
				int Z = (k + Map::chunk_coord2block_coord(m_pos.z));
				int Y = (j + Map::chunk_coord2block_coord(m_pos.y));

				if (map.is_block(X, Y, Z)) {
					// local(in chunk) pos
					int	x = Map::block_coord2coord(i);
					int	y = Map::block_coord2coord(j);
					int	z = Map::block_coord2coord(k);

					int BS = static_cast<int>(COORDS_IN_BLOCK);

					uint8_t side = 1;
					block_id id = get_type(i, j, k);

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
}

World::Chunk::~Chunk()
{
	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}
}

bool World::Chunk::should_make_layer(int y)
{
	auto is_all_solid_y_check = [&](sf::Vector3i& pos, int y) {
		//check error value
		if (y == -1) {
			return is_layer_solid(pos + sf::Vector3i{ 0,-1,0 }, BLOCKS_IN_CHUNK-1);
		}
		else if (y == BLOCKS_IN_CHUNK) {
			return is_layer_solid(pos + sf::Vector3i{ 0, 1,0 }, 0);
		}
		else {
			return is_layer_solid(pos, y);
		}
	};

	return !is_all_solid_y_check(m_pos, y) ||
		!is_all_solid_y_check(m_pos, y - 1) ||
		!is_all_solid_y_check(m_pos, y + 1) ||
		!is_layer_solid(m_pos + sf::Vector3i{ 1,0,0 }, y) ||
		!is_layer_solid(m_pos + sf::Vector3i{ -1,0,0 }, y) ||
		!is_layer_solid(m_pos + sf::Vector3i{ 0,0,1 }, y) ||
		!is_layer_solid(m_pos + sf::Vector3i{ 0,0,-1 }, y);

}

Chunk::ChunkLayer& World::Chunk::get_layer(sf::Vector3i pos, int y)
{
	return m_map->get_chunk(pos.x, pos.y, pos.z).m_layers[y];
}

bool World::Chunk::is_layer_solid(sf::Vector3i pos, int y)
{
	if (m_map->can_get_chunk(pos.x, pos.y, pos.z)) {
		return m_map->get_chunk(pos.x, pos.y, pos.z).m_layers[y].is_all_solid();
	}
	else {
		return false;
	}
}


void World::Chunk::upate_vao()
{
	if (m_is_vertices_created) {
		glBindVertexArray(m_VAO);
		/**/

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_i * sizeof(GLbyte), m_vertices, GL_DYNAMIC_DRAW); //GL_DYNAMIC_DRAW GL_STATIC_DRAW

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

void Chunk::update_vertices(World::Map& map)
{
	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}

	m_i = 0;
	m_vertices = new GLbyte[BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK * 6];
	generate_vertices(map);
	m_is_vertices_created = true;
	++verticies_wasnt_free;

	//TODO mutex
	map.m_mutex__chunks4vbo_generation->lock();
	auto this_chunk_iter = map.m_free_vbo_chunks.find(this);
	bool is_chunk_in_buffer = this_chunk_iter != map.m_free_vbo_chunks.end();

	if (map.m_free_vbo_chunks.size()) {
		if (is_chunk_in_buffer) {
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
	map.m_mutex__chunks4vbo_generation->unlock();
}


void Chunk::bind_texture_first_order(block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4] = p.x; m_vertices[m_i + 5] = (p.y + 1);
	m_vertices[m_i + 10] = (p.x + 1); m_vertices[m_i + 11] = (p.y + 1);
	m_vertices[m_i + 16] = p.x; m_vertices[m_i + 17] = p.y;
	m_vertices[m_i + 22] = p.x; m_vertices[m_i + 23] = p.y;
	m_vertices[m_i + 28] = (p.x + 1); m_vertices[m_i + 29] = (p.y + 1);
	m_vertices[m_i + 34] = (p.x + 1); m_vertices[m_i + 35] = p.y;
}

void Chunk::bind_texture_second_order(block_id id, const sf::Vector2i& p)
{
	m_vertices[m_i + 4] = p.x; m_vertices[m_i + 5] = (p.y + 1);
	m_vertices[m_i + 10] = p.x; m_vertices[m_i + 11] = p.y;
	m_vertices[m_i + 16] = (p.x + 1); m_vertices[m_i + 17] = (p.y + 1);
	m_vertices[m_i + 22] = p.x; m_vertices[m_i + 23] = p.y;
	m_vertices[m_i + 28] = (p.x + 1); m_vertices[m_i + 29] = p.y;
	m_vertices[m_i + 34] = (p.x + 1); m_vertices[m_i + 35] = (p.y + 1);
}

void Chunk::bind_texture2negative_x(block_id id)
{
	bind_texture_first_order(id, DB::s_atlas_db[id][sides::negative_x]);
}

void Chunk::bind_texture2positive_x(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db[id][sides::positive_x]);
}

void Chunk::bind_texture2negative_y(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db[id][sides::negative_y]);
}

void Chunk::bind_texture2positive_y(block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db[id][sides::positive_y]);
}

void Chunk::bind_texture2negative_z(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db[id][sides::negative_z]);
}

void Chunk::bind_texture2positive_z(block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db[id][sides::positive_z]);
}

//int x = 1;
//int y = 0;
//
//void Chunk::bind_texture_first_order(block_id id, int x, int y)
//{
//	m_vertices[m_i + 4] = x; m_vertices[m_i + 5] = (y + 1);
//	m_vertices[m_i + 10] = (x + 1); m_vertices[m_i + 11] = (y + 1);
//	m_vertices[m_i + 16] = x; m_vertices[m_i + 17] = y;
//	m_vertices[m_i + 22] = x; m_vertices[m_i + 23] = y;
//	m_vertices[m_i + 28] = (x + 1); m_vertices[m_i + 29] = (y + 1);
//	m_vertices[m_i + 34] = (x + 1); m_vertices[m_i + 35] = y;
//}
//
//void Chunk::bind_texture_second_order(block_id id, int x, int y)
//{
//	m_vertices[m_i + 4] = x; m_vertices[m_i + 5] = (y + 1);
//	m_vertices[m_i + 10] = x; m_vertices[m_i + 11] = y;
//	m_vertices[m_i + 16] = (x + 1); m_vertices[m_i + 17] = (y + 1);
//	m_vertices[m_i + 22] = x; m_vertices[m_i + 23] = y;
//	m_vertices[m_i + 28] = (x + 1); m_vertices[m_i + 29] = y;
//	m_vertices[m_i + 34] = (x + 1); m_vertices[m_i + 35] = (y + 1);
//}
//const sf::Vector2i p = { 1,0 };
//void Chunk::bind_texture2negative_x(block_id id)
//{
//	bind_texture_first_order(id, 1, 0);
//}
//
//void Chunk::bind_texture2positive_x(block_id id) {
//	bind_texture_second_order(id, 1, 0);
//}
//
//void Chunk::bind_texture2negative_y(block_id id) {
//	bind_texture_second_order(id, 1, 0);
//}
//
//void Chunk::bind_texture2positive_y(block_id id) {
//	bind_texture_first_order(id, 1, 0);
//}
//
//void Chunk::bind_texture2negative_z(block_id id) {
//	bind_texture_second_order(id, 1, 0);
//}
//
//void Chunk::bind_texture2positive_z(block_id id) {
//	bind_texture_first_order(id, 1, 0);
//}
//

void World::Chunk::ChunkLayer::update(block_id type)
{
	//TODO only for initializiation
	if (type == block_id::Air) /*or water*/
	{
		--solid_block_count;
	}
	else {
		++solid_block_count;
	}
}
