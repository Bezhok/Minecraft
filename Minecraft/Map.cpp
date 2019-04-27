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
	m_world = std::make_shared<map_type>();

	//int x = sizeof map_type;

	sf::Image height_map;
	height_map.loadFromFile("resources/hmp.bmp");

	size_t x = 0, y = 0;
	sf::Vector2u size = height_map.getSize();

	for (size_t x = 0; x < size.x/4; ++x) {
		for (size_t y = 0; y < size.y/4; ++y) {

			int h = height_map.getPixel(x, y).r;

			//for (int ss=0; ss < 5; ++ss, --h) {
				if (h / CHUNK_SIZE < SUPER_CHUNK_SIZE &&
					x / CHUNK_SIZE < SUPER_CHUNK_SIZE &&
					y / CHUNK_SIZE < SUPER_CHUNK_SIZE_HEIGHT
					) {
					size_t
						block_x = x % CHUNK_SIZE,
						block_y = h % CHUNK_SIZE,
						block_z = y % CHUNK_SIZE;

					DB::block_data block = { block_x, block_y, block_z, DB::block_id::Grass };
					m_world->at(x / CHUNK_SIZE)[h / CHUNK_SIZE][y / CHUNK_SIZE].chunk().insert(
						std::make_pair(
							m_world->at(block_x)[block_y][block_z].block_hash(block_x, block_y, block_z),
							block
						)
					);
				}
			//}

		}
	}
	
	//for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
	//	for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
	//		for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
	//			for (int x = 0; x < CHUNK_SIZE; ++x) {
	//				for (int y = 0; y < CHUNK_SIZE; ++y) {
	//					for (int z = 0; z < CHUNK_SIZE; ++z) {
	//						if (y == 1 && j == 1) {
	//							if (xpix < size.x && ypix < size.y) {
	//								//height_map.getPixel(xpix, ypix);

	//								DB::block_data block = { x, y, z, DB::block_id::Grass };
	//								m_world->at(i)[j][k].chunk().insert(std::make_pair(m_world->at(i)[j][k].block_hash(x, y, z), block));
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}


	//save();


	//load();
}

Map::~Map()
{
}

bool Map::is_block(int x, int y, int z)//block x,y,z in chunk
{
	if (x < 0 || y < 0 || z < 0
		|| x / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| y / CHUNK_SIZE >= SUPER_CHUNK_SIZE_HEIGHT
		|| z / CHUNK_SIZE >= SUPER_CHUNK_SIZE) return false;

	Chunk &e = m_world->at(x / CHUNK_SIZE)[y / CHUNK_SIZE][z / CHUNK_SIZE];
	auto it = e.chunk().find(e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE));

	if (e.chunk().end() == it)
		return false;
	else
		return true;
}

bool Map::create_block(int x, int y, int z, DB::block_id id)
{
	if (x < 0 || y < 0 || z < 0
		|| x / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| y / CHUNK_SIZE >= SUPER_CHUNK_SIZE_HEIGHT
		|| z / CHUNK_SIZE >= SUPER_CHUNK_SIZE) return false;

	Chunk &e = m_world->at(x / CHUNK_SIZE)[y / CHUNK_SIZE][z / CHUNK_SIZE];
	auto it = e.chunk().find(e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE));

	if (e.chunk().end() == it) {
		e.chunk()[e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE)] = 
		{ static_cast<unsigned char>(x%CHUNK_SIZE),
			static_cast<unsigned char>(y%CHUNK_SIZE),
			static_cast<unsigned char>(z%CHUNK_SIZE),
			id 
		};

		m_edited_chunk_coord = { x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE };
		m_redraw_chunk = true;
		return true;
	}
	else {
		return false;
	}
}

bool Map::delete_block(int x, int y, int z)
{
	if (x < 0 || y < 0 || z < 0
		|| x / CHUNK_SIZE >= SUPER_CHUNK_SIZE
		|| y / CHUNK_SIZE >= SUPER_CHUNK_SIZE_HEIGHT
		|| z / CHUNK_SIZE >= SUPER_CHUNK_SIZE) return false;

	Chunk &e = m_world->at(x / CHUNK_SIZE)[y / CHUNK_SIZE][z / CHUNK_SIZE];
	auto it = e.chunk().find(e.block_hash(x%CHUNK_SIZE, y%CHUNK_SIZE, z%CHUNK_SIZE));

	if (e.chunk().end() == it)
		return false;
	else {
		m_edited_chunk_coord = { x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE };
		e.chunk().erase(it);
		m_redraw_chunk = true;
		return true;
	}
}

bool Map::save()
{
	// read
	ofstream fout;
	fout.open("Map.txt");

	for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
		for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
			for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
				for (auto &e : m_world->at(i)[j][k].chunk()) {
					fout << i << ' '
						<< j << ' '
						<< k << ' '
						<< e.first << ' '
						<< e.second.id << ' '
						<< e.second.x << ' '
						<< e.second.y << ' '
						<< e.second.z << '\n';
				}
			}
		}
	}
	fout.close();

	return true;
}

int Map::get_int_from_stringstream(stringstream &line_stream)
{
	string integer;
	line_stream >> integer;

	return stoi(integer);
}

bool Map::load()
{
	ifstream fin;
	fin.open("Map.txt");
	if (fin.is_open() && fin.peek() != std::ifstream::traits_type::eof()) {
		string line;
		stringstream line_stream;
		while (getline(fin, line))
		{
			line_stream.clear();
			line_stream.str(line);

			int i, j, k, hash, id, x, y, z;

			i = get_int_from_stringstream(line_stream);
			j = get_int_from_stringstream(line_stream);
			k = get_int_from_stringstream(line_stream);

			hash = get_int_from_stringstream(line_stream);
			id = get_int_from_stringstream(line_stream);
			x = get_int_from_stringstream(line_stream);
			y = get_int_from_stringstream(line_stream);
			z = get_int_from_stringstream(line_stream);

			DB::block_data block = { x, y, z, (DB::block_id)id };
			m_world->at(i)[j][k].chunk().insert(std::make_pair(hash, block));
		}
		fin.close();
	}
	else {
		fin.close();
		return false;
	}

	return true;
}
