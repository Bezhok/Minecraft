#include "pch.h"
#include "Map.h"
#include "Chunk.h"
#include "block_db.h"

using std::unordered_map;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::getline;
using std::to_string;
using std::string;

using namespace World;

Map::Map()
{
	//m_map.set_empty_key(-1000);
	//m_map.reserve(CHUNKS_IN_WORLD *CHUNKS_IN_WORLD);
	m_map.reserve(1024);
	//m_map = std::make_shared<map_type>();


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
	for (size_t x = 0; x < size.x / 2; ++x) {
		for (size_t y = 0; y < size.y / 2; ++y) {

			int h = height_map.getPixel(x, y).r/16+30;

			for (int ss = 0; ss < 30; ++ss, --h) {
				int
					chunk_x = x / BLOCKS_IN_CHUNK,
					chunk_y = h / BLOCKS_IN_CHUNK,
					chunk_z = y / BLOCKS_IN_CHUNK;

				if (chunk_y < CHUNKS_IN_WORLD_HEIGHT &&
					chunk_x < CHUNKS_IN_WORLD &&
					chunk_z < CHUNKS_IN_WORLD) 
				{
					Chunk& chunk = get_chunk(chunk_x, chunk_y, chunk_z);
					chunk.init();
					chunk.set_pos({ chunk_x, chunk_y, chunk_z });
					chunk.m_map = this;

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

	if (x < 0 || y < 0 || z < 0
		|| chunk_x >= CHUNKS_IN_WORLD
		|| chunk_y >= CHUNKS_IN_WORLD_HEIGHT
		|| chunk_z >= CHUNKS_IN_WORLD) return false;

	Chunk& chunk = get_chunk(chunk_x, chunk_y, chunk_z);
	return chunk.get_type(x%BLOCKS_IN_CHUNK, y%BLOCKS_IN_CHUNK, z%BLOCKS_IN_CHUNK) != block_id::Air;
}

bool Map::is_chunk_in_map(int x, int y, int z)
{
	return (x >= 0 && y >= 0 && z >= 0
		&& x < CHUNKS_IN_WORLD
		&& y < CHUNKS_IN_WORLD_HEIGHT
		&& z < CHUNKS_IN_WORLD);
}

bool Map::create_block(int x, int y, int z, block_id id)
{
	int chunk_x          = x / BLOCKS_IN_CHUNK,
		chunk_y          = y / BLOCKS_IN_CHUNK,
		chunk_z          = z / BLOCKS_IN_CHUNK,
		block_in_chunk_x = x % BLOCKS_IN_CHUNK,
		block_in_chunk_y = y % BLOCKS_IN_CHUNK,
		block_in_chunk_z = z % BLOCKS_IN_CHUNK;

	if (!is_chunk_in_map(chunk_x, chunk_y, chunk_z)) return false;

	Chunk& chunk = get_chunk(chunk_x, chunk_y, chunk_z);
	auto old_id = chunk.get_type(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

	if (old_id == block_id::Air) {
		chunk.init();
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

	if (!is_chunk_in_map(chunk_x, chunk_y, chunk_z)) return false;

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

bool Map::save()
{
	return true;
}

bool Map::load()
{
	return true;
}
