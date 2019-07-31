#include "pch.h"
#include "Map.h"
#include "Chunk.h"
#include "block_db.h"
#include "TerrainGenerator.h"

using namespace World;


Map::Map() : m_terrain_generator(this)
{
	int chunks_count = RENDER_DISTANCE_CHUNKS * RENDER_DISTANCE_CHUNKS * CHUNKS_IN_WORLD_HEIGHT*2;

	m_should_be_updated_neighbours.reserve(6);

	m_map.reserve(chunks_count/ CHUNKS_IN_WORLD_HEIGHT);
	m_global_vao_vbo_buffers.resize(chunks_count);

	for (auto& e : m_global_vao_vbo_buffers) {
		glGenVertexArrays(1, &e.VAO);
		glGenBuffers(1, &e.VBO);
	}
}

//inline void Map::generate_chunk_terrain(int chunk_x, int chunk_y, int chunk_z)
//{
//	m_terrain_generator.generate_chunk_terrain(chunk_x, chunk_y, chunk_z);
//}

inline void World::Map::generate_chunk_terrain(Column& column, int chunk_x, int chunk_y, int chunk_z)
{
	m_terrain_generator.generate_chunk_terrain(column, chunk_x, chunk_y, chunk_z);
}

void Map::set_block(sf::Vector3i& pos_in_chunk, Column& column, int y, block_id type)
{
	Chunk& chunk = column[y];
	auto& chunk_pos = chunk.get_pos();


	// if same column
	if (pos_in_chunk.x >= 0 && pos_in_chunk.z >= 0 && pos_in_chunk.x < BLOCKS_IN_CHUNK && pos_in_chunk.z < BLOCKS_IN_CHUNK) {
		if (pos_in_chunk.y < BLOCKS_IN_CHUNK && pos_in_chunk.y >= 0)
			chunk.set_type(pos_in_chunk.x, pos_in_chunk.y, pos_in_chunk.z, type);
		else {
			pos_in_chunk.y = pos_in_chunk.y >= 0 ? (pos_in_chunk.y) : (BLOCKS_IN_CHUNK + pos_in_chunk.y);
			int chunk_y = chunk_pos.y + Map::coord2chunk_coord(pos_in_chunk.y);
			if (chunk_y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT)
				return;

			Chunk& temp_chunk = column[chunk_y];
			temp_chunk.set_type(pos_in_chunk.x, Map::coord2block_coord_in_chunk(pos_in_chunk.y), pos_in_chunk.z, type);
		}
	}
	else {
		//pos_in_chunk.y += pos_in_chunk.y >= 0 ? (pos_in_chunk.y) : (BLOCKS_IN_CHUNK + pos_in_chunk.y);
		//pos_in_chunk.x += pos_in_chunk.x >= 0 ? (pos_in_chunk.x) : (BLOCKS_IN_CHUNK + pos_in_chunk.x);
		//pos_in_chunk.z += pos_in_chunk.z >= 0 ? (pos_in_chunk.z) : (BLOCKS_IN_CHUNK + pos_in_chunk.z);

		//int chunk_x = chunk_pos.x + Map::coord2chunk_coord(pos_in_chunk.x);
		//int chunk_z = chunk_pos.z + Map::coord2chunk_coord(pos_in_chunk.z);
		//int chunk_y = chunk_pos.y + Map::coord2chunk_coord(pos_in_chunk.y);

		//if (chunk_x < 0 || chunk_x < 0 || chunk_y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT)
		//	return;

		//Chunk& temp_chunk = get_chunk(chunk_x, chunk_y, chunk_z);

		//temp_chunk.set_type(
		//	Map::coord2block_coord_in_chunk(pos_in_chunk.x),
		//	Map::coord2block_coord_in_chunk(pos_in_chunk.y),
		//	Map::coord2block_coord_in_chunk(pos_in_chunk.z),
		//	type
		//);
	}
}

bool Map::is_solid(int x, int y, int z)
{
	int chunk_x = x / BLOCKS_IN_CHUNK,
		chunk_y = y / BLOCKS_IN_CHUNK,
		chunk_z = z / BLOCKS_IN_CHUNK;

	if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

	return Chunk::is_block_type_solid(get_chunk_or_generate(chunk_x, chunk_y, chunk_z).get_type(x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK, z % BLOCKS_IN_CHUNK));
}

bool Map::is_opaque(int x, int y, int z)
{
	int chunk_x = x / BLOCKS_IN_CHUNK,
		chunk_y = y / BLOCKS_IN_CHUNK,
		chunk_z = z / BLOCKS_IN_CHUNK;

	if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

	return !Chunk::is_block_type_transperent(get_chunk_or_generate(chunk_x, chunk_y, chunk_z).get_type(x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK, z % BLOCKS_IN_CHUNK));
}

bool Map::is_air(int x, int y, int z)
{
	int chunk_x = x / BLOCKS_IN_CHUNK,
		chunk_y = y / BLOCKS_IN_CHUNK,
		chunk_z = z / BLOCKS_IN_CHUNK;

	if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

	return get_chunk_or_generate(chunk_x, chunk_y, chunk_z).get_type(x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK, z % BLOCKS_IN_CHUNK) == block_id::Air;
}

bool Map::create_block(int x, int y, int z, block_id id)
{
	if (!m_should_redraw_chunk) {
		int chunk_x = x / BLOCKS_IN_CHUNK,
			chunk_y = y / BLOCKS_IN_CHUNK,
			chunk_z = z / BLOCKS_IN_CHUNK,
			block_in_chunk_x = x % BLOCKS_IN_CHUNK,
			block_in_chunk_y = y % BLOCKS_IN_CHUNK,
			block_in_chunk_z = z % BLOCKS_IN_CHUNK;

		if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

		Chunk& chunk = get_chunk_or_generate(chunk_x, chunk_y, chunk_z);

		auto old_id = chunk.get_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

		if (old_id == block_id::Air) {
			m_edited_block_type = id;
			m_edited_block_pos = { block_in_chunk_x, block_in_chunk_y, block_in_chunk_z };
			m_edited_chunk = &chunk;
			chunk.set_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z, id);

			m_should_redraw_chunk = true;
			return true;
		}
		else {
			return false;
		}
	}
}

bool Map::delete_block(int x, int y, int z)
{
	if (!m_should_redraw_chunk) {
		int chunk_x = x / BLOCKS_IN_CHUNK,
			chunk_y = y / BLOCKS_IN_CHUNK,
			chunk_z = z / BLOCKS_IN_CHUNK,
			block_in_chunk_x = x % BLOCKS_IN_CHUNK,
			block_in_chunk_y = y % BLOCKS_IN_CHUNK,
			block_in_chunk_z = z % BLOCKS_IN_CHUNK;

		if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

		Chunk& chunk = get_chunk_or_generate(chunk_x, chunk_y, chunk_z);
		auto old_id = chunk.get_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

		if (old_id == block_id::Air) {
			return false;
		}
		else {
			if (block_in_chunk_x == 0 && is_opaque(x - 1, y, z)) {
				m_should_be_updated_neighbours.push_back({ (x - 1) / BLOCKS_IN_CHUNK, chunk_y, chunk_z });
			}

			if (block_in_chunk_x == 15 && is_opaque(x + 1, y, z)) {
				m_should_be_updated_neighbours.push_back({ (x + 1) / BLOCKS_IN_CHUNK, chunk_y, chunk_z });
			}

			if (block_in_chunk_y == 0 && is_opaque(x, y - 1, z)) {
				m_should_be_updated_neighbours.push_back({ chunk_x, (y - 1) / BLOCKS_IN_CHUNK, chunk_z });
			}

			if (block_in_chunk_y == 15 && is_opaque(x, y + 1, z)) {
				m_should_be_updated_neighbours.push_back({ chunk_x, (y + 1) / BLOCKS_IN_CHUNK, chunk_z });
			}

			if (block_in_chunk_z == 0 && is_opaque(x, y, z - 1)) {
				m_should_be_updated_neighbours.push_back({ chunk_x, chunk_y, (z - 1) / BLOCKS_IN_CHUNK });
			}

			if (block_in_chunk_z == 15 && is_opaque(x, y, z + 1)) {
				m_should_be_updated_neighbours.push_back({ chunk_x, chunk_y, (z + 1) / BLOCKS_IN_CHUNK });
			}

			m_edited_block_pos = { block_in_chunk_x, block_in_chunk_y, block_in_chunk_z };
			m_edited_chunk = &chunk;
			m_edited_block_type = block_id::Air;
			chunk.set_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z, block_id::Air);//block_id::transperent_type

			m_should_redraw_chunk = true;
			return true;
		}
	}

	return false;
}

void Map::apply_chunk_changes()
{
	if (m_should_redraw_chunk) {
		m_should_redraw_chunk = false;
		m_should_be_updated_neighbours.clear();
		//m_edited_chunk->set_type(m_edited_block_pos.x, m_edited_block_pos.y, m_edited_block_pos.z, m_edited_block_type);

		m_edited_chunk = nullptr;
	}
};

Map::Column& Map::get_column(int i, int k)
{
	return m_map[hashXZ(i, k)];
}

Chunk& Map::get_chunk(int i, int j, int k)
{
	return  m_map[hashXZ(i, k)][j];
}

void Map::unload_column(int i, int k)
{
	m_map.erase(hashXZ(i, k));
}

Chunk& Map::get_chunk_or_generate(int i, int j, int k)
{
	auto column = m_map.find(hashXZ(i, k));

	if (column != m_map.end()) {
		return (column->second[j]);
	}
	else {
		auto& column = m_map[hashXZ(i, k)];
		for (int y = 0; y < CHUNKS_IN_WORLD_HEIGHT; ++y) {
			generate_chunk_terrain(column, i, y, k);
			column[y].init({ i, y, k }, this);
		}
		Chunk& chunk = m_map[hashXZ(i, k)][j];
		chunk.init({ i, j, k }, this);
		return chunk;
	}
}

Map::Column& Map::get_column_or_generate(int i, int k)
{
	auto column = m_map.find(hashXZ(i, k));

	if (column != m_map.end()) {
		return column->second;
	}
	else {
		auto& column = m_map[hashXZ(i, k)];

		for (int j = 0; j < CHUNKS_IN_WORLD_HEIGHT; ++j) {
			generate_chunk_terrain(column, i, j, k);
			column[j].init({ i, j, k }, this);
		}
		return column;
	}
}

void Map::unload_columns(int start_x, int end_x, int start_z, int end_z)
{
	for (auto it = m_map.begin(); it != m_map.end();) {
		auto& column = it->second;
		auto& pos = column[0].get_pos();
		if (pos.x < start_x || pos.z < start_z || pos.x > end_x || pos.z > end_z) {
			it = m_map.erase(it);
		}
		else {
			it++;
		}
	}
}