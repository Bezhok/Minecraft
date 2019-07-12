#include "pch.h"
#include "Map.h"
#include "Chunk.h"
#include "block_db.h"
#include "TerrainGenerator.h"

using namespace World;


Map::Map()
{
	srand(time(NULL));

	m_map.reserve(RENDER_DISTANCE_CHUNKS*RENDER_DISTANCE_CHUNKS*16);
	//mmmmmmap.reserve(RENDER_DISTANCE_CHUNKS*RENDER_DISTANCE_CHUNKS * 80);


	m_should_be_updated_neighbours.resize(6);

	int chunks_count = RENDER_DISTANCE_CHUNKS * RENDER_DISTANCE_CHUNKS*CHUNKS_IN_WORLD_HEIGHT*10;
	m_global_vao_vbo_buffers.resize(chunks_count);

	for (auto& e : m_global_vao_vbo_buffers) {
		glGenVertexArrays(1, &e.first);
		glGenBuffers(1, &e.second);
	}

	//TODO load
	
	//int map_size = 20;
	//for (int chunk_x = 0; chunk_x < map_size; ++chunk_x)
	//	for (int chunk_y = 0; chunk_y < CHUNKS_IN_WORLD_HEIGHT; ++chunk_y)
	//		for (int chunk_z = 0; chunk_z < map_size; ++chunk_z) {
	//			generate_chunk_terrain(chunk_x, chunk_y, chunk_z);
	//		}
}

void Map::generate_chunk_terrain(int chunk_x, int chunk_y, int chunk_z)
{
	auto& column = get_column(chunk_x, chunk_z);

	Chunk& chunk = column[chunk_y];
	if (!chunk.is_init()) {
		chunk.init({ chunk_x, chunk_y, chunk_z }, this);
	}
	//std::array<int, BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK> height_map;

	std::vector<sf::Vector3i> tree_pos;

	for (int block_x = 0; block_x < BLOCKS_IN_CHUNK; ++block_x)
		for (int block_z = 0; block_z < BLOCKS_IN_CHUNK; ++block_z) {
			int x = Map::chunk_coord2block_coord(chunk_x) + block_x;
			int z = Map::chunk_coord2block_coord(chunk_z) + block_z;

			int h = (m_terrain_generator.get_noise(x + 1, z + 1) + 1) * 24 + 20;

			for (int block_y = 0; block_y < BLOCKS_IN_CHUNK; ++block_y) {
				int y = Map::chunk_coord2block_coord(chunk_y) + block_y;

				if (y > h) break;

				//if (!chunk.is_init()) {
				//	chunk.init({ chunk_x, chunk_y, chunk_z }, this);
				//}

				block_id id;
				if (y == h) {
					if (rand() % 200 == 10) {
						tree_pos.emplace_back(block_x, block_y, block_z);
					}
					//else {
						id = block_id::Grass;
					//}
				}
				else if (y > h - 5) {
					id = block_id::Dirt;
				}
				else {
					id = block_id::Stone;
				}

				chunk.set_type(block_x, block_y, block_z, id);
			}

			for (auto& pos : tree_pos) {
				int tree_height = 5 + rand() % 3;
				for (int y = 0; y < tree_height; ++y) {
					auto full_pos = pos + sf::Vector3i{ 0, y, 0 };
					set_block(full_pos, column, chunk_y, block_id::Oak);
				}

				auto full_pos = pos + sf::Vector3i{ 0, tree_height, 0 };
				set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

				full_pos = pos + sf::Vector3i{ -1, tree_height, 0 };
				set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

				full_pos = pos + sf::Vector3i{ 1, tree_height, 0 };
				set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

				full_pos = pos + sf::Vector3i{ 0, tree_height, 1 };
				set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

				full_pos = pos + sf::Vector3i{ 0, tree_height, -1 };
				set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

				for (int x = -1; x <= 1; ++x)
					for (int z = -1; z <= 1; ++z) {
						if (x != 0 || z != 0) {
							full_pos = pos + sf::Vector3i{ x, tree_height -1, z };
							set_block(full_pos, column, chunk_y, block_id::Oak_leafage);
						}
					}

				for (int x = -2; x <= 2; ++x)
					for (int z = -2; z <= 2; ++z) {
						if (x != 0 || z != 0) {
							full_pos = pos + sf::Vector3i{ x, tree_height - 2, z };
							set_block(full_pos, column, chunk_y, block_id::Oak_leafage);
						}
					}

			}

			tree_pos.clear();
		}
}

void Map::set_block(sf::Vector3i& pos_in_chunk, Map::Column& column, int y, block_id type)
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

bool Map::is_block(int x, int y, int z) //block x,y,z in chunk
{
	int chunk_x = x / BLOCKS_IN_CHUNK,
		chunk_y = y / BLOCKS_IN_CHUNK,
		chunk_z = z / BLOCKS_IN_CHUNK;

	if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

	return get_chunk_n(chunk_x, chunk_y, chunk_z).get_type(x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK, z % BLOCKS_IN_CHUNK) != block_id::Air;
}

bool Map::create_block(int x, int y, int z, block_id id)
{
	int chunk_x          = x / BLOCKS_IN_CHUNK,
		chunk_y          = y / BLOCKS_IN_CHUNK,
		chunk_z          = z / BLOCKS_IN_CHUNK,
		block_in_chunk_x = x % BLOCKS_IN_CHUNK,
		block_in_chunk_y = y % BLOCKS_IN_CHUNK,
		block_in_chunk_z = z % BLOCKS_IN_CHUNK;
	
	if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

	Chunk& chunk = get_chunk_n(chunk_x, chunk_y, chunk_z);

	auto old_id = chunk.get_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

	if (old_id == block_id::Air) {
		chunk.set_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z, id);

		m_edited_block_pos = { block_in_chunk_x, block_in_chunk_y, block_in_chunk_z };
		m_edited_chunk_pos = { chunk_x, chunk_y, chunk_z };
		m_redraw_chunk = true;
		return true;
	}
	else {
		return false;
	}
}

bool Map::delete_block(int x, int y, int z)
{
	int chunk_x          = x / BLOCKS_IN_CHUNK,
		chunk_y          = y / BLOCKS_IN_CHUNK,
		chunk_z          = z / BLOCKS_IN_CHUNK,
		block_in_chunk_x = x % BLOCKS_IN_CHUNK,
		block_in_chunk_y = y % BLOCKS_IN_CHUNK,
		block_in_chunk_z = z % BLOCKS_IN_CHUNK;

	if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;
	
	Chunk& chunk = get_chunk_n(chunk_x, chunk_y, chunk_z);
	auto old_id = chunk.get_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

	if (old_id == block_id::Air) {
		return false;
	}
	else {
		m_edited_block_pos = { block_in_chunk_x, block_in_chunk_y, block_in_chunk_z };
		m_edited_chunk_pos = { chunk_x, chunk_y, chunk_z};

		if (block_in_chunk_x == 0 && is_block(x - 1, y, z)) {
			m_should_be_updated_neighbours.push_back({ (x - 1) / BLOCKS_IN_CHUNK, chunk_y, chunk_z });
		}

		if (block_in_chunk_x == 15 && is_block(x + 1, y, z)) {
			m_should_be_updated_neighbours.push_back({ (x + 1) / BLOCKS_IN_CHUNK, chunk_y, chunk_z });
		}

		if (block_in_chunk_y == 0 && is_block(x, y - 1, z)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, (y-1)/ BLOCKS_IN_CHUNK, chunk_z });
		}

		if (block_in_chunk_y == 15 && is_block(x, y + 1, z)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, (y + 1) / BLOCKS_IN_CHUNK, chunk_z });
		}

		if (block_in_chunk_z == 0 && is_block(x, y, z - 1)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, chunk_y, (z - 1) / BLOCKS_IN_CHUNK });
		}

		if (block_in_chunk_z == 15 && is_block(x, y, z + 1)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, chunk_y, (z + 1) / BLOCKS_IN_CHUNK });
		}

		chunk.set_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z, block_id::Air);
		m_redraw_chunk = true;
		return true;
	}
	
	return false;
}

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

Chunk& Map::get_chunk_n(int i, int j, int k)
{
	auto column = m_map.find(hashXZ(i, k));

	if (column != m_map.end()) {
		return (column->second[j]);
	}
	else {
		auto& column = m_map[hashXZ(i, k)];
		for (int y = 0; y < CHUNKS_IN_WORLD_HEIGHT; ++y) {
			generate_chunk_terrain(i, y, k);
			column[y].init({ i, y, k }, this);
		}
		Chunk& chunk = m_map[hashXZ(i, k)][j];
		chunk.init({ i, j, k }, this);
		return chunk;
	}
}

Map::Column& Map::get_column_n(int i, int k)
{
	auto column = m_map.find(hashXZ(i, k));

	if (column != m_map.end()) {
		return column->second;
	}
	else {
		auto& column = m_map[hashXZ(i, k)];
		for (int j = 0; j < CHUNKS_IN_WORLD_HEIGHT; ++j) {
			generate_chunk_terrain(i, j, k);
			column[j].init({ i, j, k }, this);
		}

		return column;
	}
}
