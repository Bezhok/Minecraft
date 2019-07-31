#include "pch.h"
#include "block_db.h"
#include "Map.h"
#include "Chunk.h"

using namespace World;


void Chunk::init(const sf::Vector3i& pos, World::Map* map)
{
	m_is_init = true;
	m_pos = pos;
	m_map = map;
}

block_id Chunk::get_type(int x, int y, int z)
{
	if (m_data.empty()) {
		return block_id::Air;
	}

	return m_data.at(x + y * BLOCKS_IN_CHUNK + z * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK);
}

void Chunk::set_type(int x, int y, int z, block_id type)
{
	m_data.at(x + y * BLOCKS_IN_CHUNK + z * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK) = type;
	m_layers[y].update(type);
}


bool Chunk::is_empty()
{
	for (block_id id : m_data)
	{
		if (id != block_id::Air) {
			return false;
		}
	}

	return true;
};

inline void Chunk::add_byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w)
{
	m_vertices[m_i++] = x; m_vertices[m_i++] = y; m_vertices[m_i++] = z; m_vertices[m_i++] = w;
	m_i += 2;
}

bool Chunk::is_air__in_chunk(int x, int y, int z)
{
	if (y < 0 || x < 0 || z < 0 || y >= BLOCKS_IN_CHUNK || x >= BLOCKS_IN_CHUNK || z >= BLOCKS_IN_CHUNK) {
		return m_map->is_air(
			x + Map::chunk_coord2block_coord(m_pos.x),
			y + Map::chunk_coord2block_coord(m_pos.y),
			z + Map::chunk_coord2block_coord(m_pos.z)
		);
	}

	return get_type(x, y, z) == block_id::Air;
}

bool Chunk::is_opaque__in_chunk(int x, int y, int z)
{
	if (y < 0 || x < 0 || z < 0 || y >= BLOCKS_IN_CHUNK || x >= BLOCKS_IN_CHUNK || z >= BLOCKS_IN_CHUNK) {
		return m_map->is_opaque(
			x + Map::chunk_coord2block_coord(m_pos.x),
			y + Map::chunk_coord2block_coord(m_pos.y),
			z + Map::chunk_coord2block_coord(m_pos.z)
			);
	}

	return !is_block_type_transperent(get_type(x, y, z));
}

void Chunk::generate_vertices()
{
	static const uint8_t m = 15;
	static const uint8_t BS = static_cast<uint8_t>(COORDS_IN_BLOCK)*m;
	static const uint8_t side = 1 * m;

	for (int j = 0; j < BLOCKS_IN_CHUNK; ++j) {
		if (should_make_layer(j))
			for (int i = 0; i < BLOCKS_IN_CHUNK; ++i)
			for (int k = 0; k < BLOCKS_IN_CHUNK; ++k)

				if (!is_air__in_chunk(i, j, k)) {
					// local(in chunk) pos
					//TODO dirty hack))
					
					uint8_t x = Map::block_coord2coord(i)*m;
					uint8_t y = Map::block_coord2coord(j)*m;
					uint8_t z = Map::block_coord2coord(k)*m;

					block_id id = get_type(i, j, k);

					
					if (id == block_id::Cactus) {

							uint8_t xx = x + 1;

							bind_texture2negative_x(id);
							add_byte4(xx, y,      z, side);
							add_byte4(xx, y,      z + BS, side);
							add_byte4(xx, y + BS, z, side);
							add_byte4(xx, y + BS, z, side);
							add_byte4(xx, y,      z + BS, side);
							add_byte4(xx, y + BS, z + BS, side);


							xx = x - 1;
							bind_texture2positive_x(id);
							add_byte4(xx + BS, y,      z, side);
							add_byte4(xx + BS, y + BS, z, side);
							add_byte4(xx + BS, y,      z + BS, side);
							add_byte4(xx + BS, y + BS, z, side);
							add_byte4(xx + BS, y + BS, z + BS, side);
							add_byte4(xx + BS, y,      z + BS, side);



							bind_texture2negative_y(id);
							add_byte4(x, y,      z, 0);
							add_byte4(x + BS, y, z, 0);
							add_byte4(x, y,      z + BS, 0);
							add_byte4(x + BS, y, z, 0);
							add_byte4(x + BS, y, z + BS, 0);
							add_byte4(x, y,      z + BS, 0);


							bind_texture2positive_y(id);
							add_byte4(x, y + BS, z, 0);
							add_byte4(x, y + BS, z + BS, 0);
							add_byte4(x + BS, y + BS, z, 0);
							add_byte4(x + BS, y + BS, z, 0);
							add_byte4(x, y + BS, z + BS, 0);
							add_byte4(x + BS, y + BS, z + BS, 0);


							uint8_t zz = z + 1;
							bind_texture2negative_z(id);
							add_byte4(x, y,           zz, side);
							add_byte4(x, y + BS,      zz, side);
							add_byte4(x + BS, y,      zz, side);
							add_byte4(x, y + BS,      zz, side);
							add_byte4(x + BS, y + BS, zz, side);
							add_byte4(x + BS, y,      zz, side);


							zz = z - 1;
							bind_texture2positive_z(id);
							add_byte4(x, y,           zz + BS, side);
							add_byte4(x + BS, y,      zz + BS, side);
							add_byte4(x, y + BS,      zz + BS, side);
							add_byte4(x, y + BS,      zz + BS, side);
							add_byte4(x + BS, y,      zz + BS, side);
							add_byte4(x + BS, y + BS, zz + BS, side);

					}
					else {

						if (!is_opaque__in_chunk(i - 1, j, k)) {

							bind_texture2negative_x(id);
							add_byte4(x, y, z, side);
							add_byte4(x, y, z + BS, side);
							add_byte4(x, y + BS, z, side);
							add_byte4(x, y + BS, z, side);
							add_byte4(x, y, z + BS, side);
							add_byte4(x, y + BS, z + BS, side);
						}

						if (!is_opaque__in_chunk(i + 1, j, k)) {
							bind_texture2positive_x(id);
							add_byte4(x + BS, y, z, side);
							add_byte4(x + BS, y + BS, z, side);
							add_byte4(x + BS, y, z + BS, side);
							add_byte4(x + BS, y + BS, z, side);
							add_byte4(x + BS, y + BS, z + BS, side);
							add_byte4(x + BS, y, z + BS, side);
						}

						if (!is_opaque__in_chunk(i, j - 1, k)) {
							bind_texture2negative_y(id);
							add_byte4(x, y, z, 0);
							add_byte4(x + BS, y, z, 0);
							add_byte4(x, y, z + BS, 0);
							add_byte4(x + BS, y, z, 0);
							add_byte4(x + BS, y, z + BS, 0);
							add_byte4(x, y, z + BS, 0);
						}

						if (!is_opaque__in_chunk(i, j + 1, k)) {
							bind_texture2positive_y(id);
							add_byte4(x, y + BS, z, 0);
							add_byte4(x, y + BS, z + BS, 0);
							add_byte4(x + BS, y + BS, z, 0);
							add_byte4(x + BS, y + BS, z, 0);
							add_byte4(x, y + BS, z + BS, 0);
							add_byte4(x + BS, y + BS, z + BS, 0);
						}

						if (!is_opaque__in_chunk(i, j, k - 1)) {
							bind_texture2negative_z(id);
							add_byte4(x, y, z, side);
							add_byte4(x, y + BS, z, side);
							add_byte4(x + BS, y, z, side);
							add_byte4(x, y + BS, z, side);
							add_byte4(x + BS, y + BS, z, side);
							add_byte4(x + BS, y, z, side);
						}

						if (!is_opaque__in_chunk(i, j, k + 1)) {
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

Chunk::Chunk()
{
	m_data.fill(block_id::Air);
}

Chunk::~Chunk()
{
	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}
}

bool Chunk::should_make_layer(int y)
{
	auto is_all_solid_check_range = [&](sf::Vector3i& pos, int y) {
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

	return !is_all_solid_check_range(m_pos, y) ||
		!is_all_solid_check_range(m_pos, y - 1) ||
		!is_all_solid_check_range(m_pos, y + 1) ||
		!is_layer_solid(m_pos + sf::Vector3i{ 1,0,0 }, y) ||
		!is_layer_solid(m_pos + sf::Vector3i{ -1,0,0 }, y) ||
		!is_layer_solid(m_pos + sf::Vector3i{ 0,0,1 }, y) ||
		!is_layer_solid(m_pos + sf::Vector3i{ 0,0,-1 }, y);

}

bool Chunk::is_layer_solid(sf::Vector3i pos, int y)
{
	if (pos.y < 0 || pos.y >= CHUNKS_IN_WORLD_HEIGHT)
		return false;

	Chunk& chunk = m_map->get_chunk_or_generate(pos.x, pos.y, pos.z);

	if (chunk.is_init()) {
		return chunk.m_layers[y].is_all_solid();
	}
	else {
		return false;
	}
}

void Chunk::upate_vao()
{
	if (m_is_vertices_created) {
		m_old_i = m_i;

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
		

		delete[] m_vertices;
		--verticies_wasnt_free;
		m_is_vertices_created = false;
	}
}

void Chunk::update_vertices(sf::Mutex& mutex__for_vbo_generation)
{
	update_vertices_use_old_buffers();

	mutex__for_vbo_generation.lock();

	assert(!m_map->m_global_vao_vbo_buffers.empty());
	m_buffers = m_map->m_global_vao_vbo_buffers.back();
	m_map->m_global_vao_vbo_buffers.pop_back();

	mutex__for_vbo_generation.unlock();
}

void Chunk::update_vertices_use_old_buffers()
{
	if (m_is_vertices_created) {
		delete[] m_vertices;
		--verticies_wasnt_free;
	}

	m_i = 0;
	m_vertices = new GLbyte[BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK * 36 * 6];
	generate_vertices();
	m_is_vertices_created = true;
	++verticies_wasnt_free;
}

void World::Chunk::free_buffers()
{
	if (get_VAO()) {
		m_map->m_global_vao_vbo_buffers.push_back(get_buffers());
		m_buffers = Buffers();
	}
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

inline void Chunk::bind_texture2negative_x(block_id id)
{
	bind_texture_first_order(id, DB::s_atlas_db(id, sides::negative_x));
}

inline void Chunk::bind_texture2positive_x(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db(id, sides::positive_x));
}

inline void Chunk::bind_texture2negative_y(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db(id, sides::negative_y));
}

inline void Chunk::bind_texture2positive_y(block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db(id, sides::positive_y));
}

inline void Chunk::bind_texture2negative_z(block_id id) {
	bind_texture_second_order(id, DB::s_atlas_db(id, sides::negative_z));
}

inline void Chunk::bind_texture2positive_z(block_id id) {
	bind_texture_first_order(id, DB::s_atlas_db(id, sides::positive_z));
}

void Chunk::ChunkLayer::update(block_id type)
{
	if (is_block_type_transperent(type))
	{
		--solid_block_count;
	}
	else {
		++solid_block_count;
	}
}

bool World::Chunk::is_block_type_transperent(block_id type)
{
	return type == block_id::Air || type == block_id::Cactus;// || type == block_id::Oak_leafage; //|| type == block_id::Water || type == block_id::Glass; //...
}

bool World::Chunk::is_block_type_solid(block_id type)
{
	return type != block_id::Air && type != block_id::Water; //...
}