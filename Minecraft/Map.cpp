#include "pch.h"
#include "Map.h"
#include "Chunk.h"
#include "block_db.h"

using namespace World;


Map::Map()
{
	m_map.reserve(RENDER_DISTANCE_CHUNKS*RENDER_DISTANCE_CHUNKS*2);



	m_should_be_updated_neighbours.resize(6);

	int chunks_count = RENDER_DISTANCE_CHUNKS * RENDER_DISTANCE_CHUNKS*CHUNKS_IN_WORLD_HEIGHT;
	m_global_vao_vbo_buffers.resize(chunks_count);

	for (auto& e : m_global_vao_vbo_buffers) {
		glGenVertexArrays(1, &e.first);
		glGenBuffers(1, &e.second);
	}

	//TODO load
	sf::Image height_map;
	height_map.loadFromFile("resources/noiseTexture.png");//hmp.bmp
	sf::Vector2u size = height_map.getSize();
	for (size_t x = 0; x < 320; ++x) {
		for (size_t y = 0; y < 320; ++y) {

			int h = height_map.getPixel(x, y).r/16+30;

			for (int ss = 0; ss < 30; ++ss, --h) {
				int
					chunk_x = x / BLOCKS_IN_CHUNK,
					chunk_y = h / BLOCKS_IN_CHUNK,
					chunk_z = y / BLOCKS_IN_CHUNK;

				if (chunk_y < CHUNKS_IN_WORLD_HEIGHT) 
				{
					Chunk& chunk = create_chunk(chunk_x, chunk_y, chunk_z);
					if (!chunk.is_init()) {
						chunk.init({ chunk_x, chunk_y, chunk_z }, this);
					}

					size_t
						block_x = x % BLOCKS_IN_CHUNK,
						block_y = h % BLOCKS_IN_CHUNK,
						block_z = y % BLOCKS_IN_CHUNK;


					block_id id;
					if (ss == 0) {
						id = block_id::Grass;
					}
					else if (ss < 7 && ss > 0) {
						id = block_id::Dirt;
					}
					else {
						id = block_id::Stone;
					}

					chunk.set_type(block_x, block_y, block_z, id);
				}
			}

		}
	}
}

bool Map::is_block(int x, int y, int z) //block x,y,z in chunk
{
	int chunk_x = x / BLOCKS_IN_CHUNK,
		chunk_y = y / BLOCKS_IN_CHUNK,
		chunk_z = z / BLOCKS_IN_CHUNK;

	if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

	Chunk* chunk = get_chunk_ptr(chunk_x, chunk_y, chunk_z);
	if (chunk != nullptr) {
		return chunk->get_type(x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK, z % BLOCKS_IN_CHUNK) != block_id::Air;
	}
	else {
		return false;
	}
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

	Chunk* chunk = get_chunk_ptr(chunk_x, chunk_y, chunk_z);
	if (chunk != nullptr) {
		auto old_id = chunk->get_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

		if (old_id == block_id::Air) {
			chunk->set_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z, id);

			m_edited_block_pos = { block_in_chunk_x, block_in_chunk_y, block_in_chunk_z };
			m_edited_chunk_pos = { chunk_x, chunk_y, chunk_z };
			m_redraw_chunk = true;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		Chunk& chunk = create_chunk(chunk_x, chunk_y, chunk_z);
		chunk.init({ chunk_x, chunk_y, chunk_z }, this);
		

		chunk.set_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z, id);

		m_edited_block_pos = { block_in_chunk_x, block_in_chunk_y, block_in_chunk_z };
		m_edited_chunk_pos = { chunk_x, chunk_y, chunk_z };
		m_redraw_chunk = true;
		return true;
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

	Chunk& chunk = get_chunk(chunk_x, chunk_y, chunk_z);
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
}

Map::Column& Map::get_column(int i, int k)
{
	return m_map.at(hashXZ(i, k));
}

Chunk& Map::get_chunk(int i, int j, int k)
{
	return  m_map.at(hashXZ(i, k))[j];
}

Chunk& Map::create_chunk(int i, int j, int k)
{
	return  m_map[hashXZ(i, k)][j];
}

Chunk* Map::get_chunk_ptr(int i, int j, int k)
{
	auto column = m_map.find(hashXZ(i, k));

	if (column != m_map.end()) {
		return &(column->second[j]);
	}
	else {
		return nullptr;
	}
}

Map::Column* Map::get_column_ptr(int i, int k)
{
	auto column = m_map.find(hashXZ(i, k));

	if (column != m_map.end()) {
		return &(column->second);
	}
	else {
		return nullptr;
	}
}
