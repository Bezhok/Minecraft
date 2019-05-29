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
	m_map = std::make_shared<map_type>();

	m_should_be_updated_neighbours.resize(6);
	//for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
	//	for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
	//		for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
	//			for (int x = 0; x < CHUNK_SIZE; ++x) {
	//				for (int y = 0; y < CHUNK_SIZE; ++y) {
	//					for (int z = 0; z < CHUNK_SIZE; ++z) {
	//						if (y == 1 && j == 1 && i < 10 && k < 10) {
	//							DB::block_data block = { x, y, z, DB::block_id::Grass };
	//							m_map->at(i)[j][k].chunk().insert(std::make_pair(m_map->at(i)[j][k].block_hash(x, y, z), block));
	//							m_map->at(i)[j][k].set_pos({ i,j,k });
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	int chunks_count = RENDER_DISTANCE_CHUNKS * RENDER_DISTANCE_CHUNKS*SUPER_CHUNK_SIZE_HEIGHT;
	m_global_vao_vbo_buffers.resize(chunks_count);

	for (auto& e : m_global_vao_vbo_buffers) {
		glGenVertexArrays(1, &e.first);
		glGenBuffers(1, &e.second);
	}

	sf::Image height_map;
	height_map.loadFromFile("resources/noiseTexture.png");//hmp.bmp

	sf::Vector2u size = height_map.getSize();

	for (int x = 0; x < SUPER_CHUNK_SIZE; ++x) {
		for (int y = 0; y < SUPER_CHUNK_SIZE_HEIGHT; ++y) {
			for (int z = 0; z < SUPER_CHUNK_SIZE; ++z) {
				m_map->at(x)[y][z].set_pos({ x, y, z });
			}
		}
	}

	for (size_t x = 0; x < size.x / 2; ++x) {
		for (size_t y = 0; y < size.y / 2; ++y) {

			int h = height_map.getPixel(x, y).r/16+30;

			for (int ss = 0; ss < 30; ++ss, --h) {
				int
					chunk_x = x / CHUNK_SIZE,
					chunk_y = h / CHUNK_SIZE,
					chunk_z = y / CHUNK_SIZE;

				if (chunk_y < SUPER_CHUNK_SIZE_HEIGHT &&
					chunk_x < SUPER_CHUNK_SIZE &&
					chunk_z < SUPER_CHUNK_SIZE) 
				{

					size_t
						block_x = x % CHUNK_SIZE,
						block_y = h % CHUNK_SIZE,
						block_z = y % CHUNK_SIZE;


					DB::block_id id;
					if (ss == 0) {
						id = DB::block_id::Grass;
					}
					else if (ss < 7 && ss > 0) {
						id = DB::block_id::Dirt;
					}
					else {
						id = DB::block_id::Stone;
					}

					m_map->at(chunk_x)[chunk_y][chunk_z].chunk()(block_x, block_y, block_z) = id;



				}
			}

		}
	}

	//load();
}

bool Map::is_block_without_checking_range(int x, int y, int z) {
	Chunk& e = m_map->operator[](x / CHUNK_SIZE)[y / CHUNK_SIZE][z / CHUNK_SIZE];
	//const auto& block = e.chunk()(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE);

	return e.chunk()(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE);
}

bool Map::is_block(int x, int y, int z) //block x,y,z in chunk
{
	int chunk_x = x / CHUNK_SIZE,
		chunk_y = y / CHUNK_SIZE,
		chunk_z = z / CHUNK_SIZE;

	if (x < 0 || y < 0 || z < 0
		|| chunk_x >= SUPER_CHUNK_SIZE
		|| chunk_y >= SUPER_CHUNK_SIZE_HEIGHT
		|| chunk_z >= SUPER_CHUNK_SIZE) return false;

	Chunk& e = m_map->operator[](x / CHUNK_SIZE)[y / CHUNK_SIZE][z / CHUNK_SIZE];
	return e.chunk()(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE);
}

bool Map::is_chunk_in_map(int x, int y, int z)
{
	return (x >= 0 && y >= 0 && z >= 0
		&& x < SUPER_CHUNK_SIZE
		&& y < SUPER_CHUNK_SIZE_HEIGHT
		&& z < SUPER_CHUNK_SIZE);
}

bool Map::create_block(int x, int y, int z, DB::block_id id)
{
	int chunk_x          = x / CHUNK_SIZE,
		chunk_y          = y / CHUNK_SIZE,
		chunk_z          = z / CHUNK_SIZE,
		block_in_chunk_x = x % CHUNK_SIZE,
		block_in_chunk_y = y % CHUNK_SIZE,
		block_in_chunk_z = z % CHUNK_SIZE;

	if (!is_chunk_in_map(chunk_x, chunk_y, chunk_z)) return false;

	Chunk& e = m_map->operator[](chunk_x)[chunk_y][chunk_z];
	auto old_id = e.chunk()(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

	if (!old_id) {
		e.chunk()(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z) = id;


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
	int chunk_x          = x / CHUNK_SIZE,
		chunk_y          = y / CHUNK_SIZE,
		chunk_z          = z / CHUNK_SIZE,
		block_in_chunk_x = x % CHUNK_SIZE,
		block_in_chunk_y = y % CHUNK_SIZE,
		block_in_chunk_z = z % CHUNK_SIZE;

	if (!is_chunk_in_map(chunk_x, chunk_y, chunk_z)) return false;

	Chunk& e = m_map->operator[](chunk_x)[chunk_y][chunk_z];
	auto old_id = e.chunk()(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z);

	if (!old_id) {
		return false;
	}
	else {
		m_edited_chunk_pos = { chunk_x, chunk_y, chunk_z};

		if (block_in_chunk_x == 0 && is_block(x - 1, y, z)) {
			m_should_be_updated_neighbours.push_back({ (x - 1) / CHUNK_SIZE, chunk_y, chunk_z });
		}

		if (block_in_chunk_x == 15 && is_block(x + 1, y, z)) {
			m_should_be_updated_neighbours.push_back({ (x + 1) / CHUNK_SIZE, chunk_y, chunk_z });
		}

		if (block_in_chunk_y == 0 && is_block(x, y - 1, z)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, (y-1)/ CHUNK_SIZE, chunk_z });
		}

		if (block_in_chunk_y == 15 && is_block(x, y + 1, z)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, (y + 1) / CHUNK_SIZE, chunk_z });
		}

		if (block_in_chunk_z == 0 && is_block(x, y, z - 1)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, chunk_y, (z - 1) / CHUNK_SIZE });
		}

		if (block_in_chunk_z == 15 && is_block(x, y, z + 1)) {
			m_should_be_updated_neighbours.push_back({ chunk_x, chunk_y, (z + 1) / CHUNK_SIZE });
		}

		e.chunk()(block_in_chunk_x, block_in_chunk_y, block_in_chunk_z) = DB::block_id::Air;
		m_redraw_chunk = true;
		return true;
	}
}

bool Map::save()
{
	// read
	//ofstream fout;
	//fout.open("Map.txt");

	//for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
	//	for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
	//		for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
	//			for (auto& e : m_map->at(i)[j][k].chunk()) {
	//				fout << i << ' '
	//					<< j << ' '
	//					<< k << ' '
	//					<< e.first << ' '
	//					<< (int)e.second.id << ' '
	//					<< (int)e.second.x << ' '
	//					<< (int)e.second.y << ' '
	//					<< (int)e.second.z << '\n';
	//			}
	//		}
	//	}
	//}
	//fout.close();

	return true;
}

int get_int_from_stringstream(stringstream& line_stream)
{
	string integer;
	line_stream >> integer;

	return stoi(integer);
}

bool Map::load()
{
	//ifstream fin;
	//fin.open("Map.txt");
	//if (fin.is_open() && fin.peek() != std::ifstream::traits_type::eof()) {
	//	string line;
	//	stringstream line_stream;
	//	while (getline(fin, line))
	//	{
	//		line_stream.clear();
	//		line_stream.str(line);

	//		int i, j, k, hash, id, x, y, z;

	//		i = get_int_from_stringstream(line_stream);
	//		j = get_int_from_stringstream(line_stream);
	//		k = get_int_from_stringstream(line_stream);

	//		hash = get_int_from_stringstream(line_stream);
	//		id =(unsigned char) get_int_from_stringstream(line_stream);
	//		x= (unsigned char)get_int_from_stringstream(line_stream);
	//		y= (unsigned char)get_int_from_stringstream(line_stream);
	//		z = (unsigned char)get_int_from_stringstream(line_stream);

	//		DB::block_data block = { x, y, z, (DB::block_id)id };
	//		m_map->at(i)[j][k].chunk().insert(std::make_pair(hash, block));
	//		m_map->at(i)[j][k].set_pos({ i,j,k });
	//	}
	//	fin.close();
	//}
	//else {
	//	fin.close();
	//	return false;
	//}

	return true;
}
