#include "TerrainGenerator.h"
#include "Map.h"
#include "block_db.h"

using namespace World;


TerrainGenerator::TerrainGenerator(Map* map) : m_map(map)
{
	
	srand(m_noise.GetSeed());

	//m_noise.SetNoiseType(FastNoise::Perlin);
	//m_noise.SetFrequency(0.006f);
	//m_noise.SetFractalOctaves(5);

	m_noise.SetNoiseType(FastNoise::PerlinFractal);
	m_noise.SetFrequency(0.01f);
	//m_noise.SetFractalGain(10.5);
	//m_noise.SetFractalLacunarity(0.99);
	//m_noise.SetFractalType(FastNoise::Billow);
	m_noise.SetFractalOctaves(1);
	m_noise.SetInterp(FastNoise::Hermite);

	m_biome_noise.SetNoiseType(FastNoise::ValueFractal);
	m_biome_noise.SetFrequency(0.002f); //0.0009
	m_biome_noise.SetFractalOctaves(5);
	//m_biome_noise.SetFractalType(FastNoise::Billow);
	//m_noise.SetGradientPerturbAmp(5);
	//m_noise.GradientPerturbFractal();
}

TerrainGenerator::~TerrainGenerator()
{
}

inline void TerrainGenerator::generate_chunk_terrain(int chunk_x, int chunk_y, int chunk_z)
{
	auto& column = m_map->get_column(chunk_x, chunk_z);
	generate_chunk_terrain(column, chunk_x, chunk_y, chunk_z);
}

void World::TerrainGenerator::generate_chunk_terrain(std::array<Chunk, CHUNKS_IN_WORLD_HEIGHT>& column, int chunk_x, int chunk_y, int chunk_z)
{
	//auto& column = m_map->get_column(chunk_x, chunk_z);

	Chunk& chunk = column[chunk_y];
	chunk.init({ chunk_x, chunk_y, chunk_z }, m_map);

	srand(chunk_x+chunk_z*chunk_z);

	int offset = 0;//30
	static const int WATER_LEVEL = offset + 30;

	std::vector<sf::Vector3i> tree_pos;
	for (int block_x = 0; block_x < BLOCKS_IN_CHUNK; ++block_x)
		for (int block_z = 0; block_z < BLOCKS_IN_CHUNK; ++block_z) {
			int x = Map::chunk_coord2block_coord(chunk_x) + block_x;
			int z = Map::chunk_coord2block_coord(chunk_z) + block_z;

			int h = 0;
			//float b = (m_biome_noise.GetNoise(static_cast<float>(x) + 1, static_cast<float>(z) + 1) / 2 + 0.5f);
			float b = m_biome_noise.GetNoise(static_cast<float>(x) + 1, static_cast<float>(z) + 1);

			//std::cout << b << std::endl;

			int biome_type = 0;
			static int tree_freeq[] = {200, 1000, 50};
			block_id top_block_type;
			block_id below_block_type;
			//bool default_biom = true;
			if (b >= 0) {
				biome_type = 2;
				if (b > 0.2) {
					biome_type = 0;
				}

				top_block_type = block_id::Grass;
				below_block_type = block_id::Dirt;
				h = static_cast<int>((get_noise(x + 1, z + 1) + 1) * 40) + offset;
			}
			else
			{
				biome_type = 1;
				top_block_type = below_block_type = block_id::Sand;
				
				h = static_cast<int>((get_noise(x + 1, z + 1) + 1) * 15) + offset+19;
			}


			for (int block_y = 0; block_y < BLOCKS_IN_CHUNK; ++block_y) {
				int y = Map::chunk_coord2block_coord(chunk_y) + block_y;
				block_id id = block_id::Air;



				if (h < WATER_LEVEL && y >= h && y < WATER_LEVEL) {
					id = block_id::Water;
				}
				else if (y > h) {
					break;
				}
				else if (h - 4 < WATER_LEVEL && y > h - 4) {
					id = block_id::Sand;
				}
				else {
					if (y == h) {
						if (glm::linearRand(0, tree_freeq[biome_type]) == 1) {
							tree_pos.emplace_back(block_x, block_y + 1, block_z);
							id = below_block_type;
						}
						else {
							id = top_block_type;
						}
					}
					else if (y > h - 5) {
						id = below_block_type;
					}
					else {
						id = block_id::Stone;
					}
				}
				chunk.set_block_type(block_x, block_y, block_z, id);
			}

			if (biome_type==0 || biome_type == 2) {

				for (auto& pos : tree_pos) {
					int tree_height = glm::linearRand(5, 7);
					for (int y = 0; y < tree_height; ++y) {
						auto full_pos = pos + sf::Vector3i{ 0, y, 0 };
						m_map->set_block(full_pos, column, chunk_y, block_id::Oak);
					}

					auto full_pos = pos + sf::Vector3i{ 0, tree_height, 0 };
					m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

					full_pos = pos + sf::Vector3i{ -1, tree_height, 0 };
					m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

					full_pos = pos + sf::Vector3i{ 1, tree_height, 0 };
					m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

					full_pos = pos + sf::Vector3i{ 0, tree_height, 1 };
					m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

					full_pos = pos + sf::Vector3i{ 0, tree_height, -1 };
					m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);

					for (int x = -1; x <= 1; ++x)
						for (int z = -1; z <= 1; ++z) {
							if (x != 0 || z != 0) {
								full_pos = pos + sf::Vector3i{ x, tree_height - 1, z };
								m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);
							}
						}

					for (int x = -2; x <= 2; ++x)
						for (int z = -2; z <= 2; ++z) {
							if (x != 0 || z != 0) {
								full_pos = pos + sf::Vector3i{ x, tree_height - 2, z };
								m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);
							}
						}

					for (int x = -2; x <= 2; ++x)
						for (int z = -2; z <= 2; ++z) {
							if (x != 0 || z != 0) {
								full_pos = pos + sf::Vector3i{ x, tree_height - 3, z };
								m_map->set_block(full_pos, column, chunk_y, block_id::Oak_leafage);
							}
						}
				}
			}
			else {
				for (auto& pos : tree_pos) {
					int tree_height = glm::linearRand(3, 6);
					for (int y = 0; y < tree_height; ++y) {
						auto full_pos = pos + sf::Vector3i{ 0, y, 0 };
						m_map->set_block(full_pos, column, chunk_y, block_id::Cactus);
					}
				}
			}

			tree_pos.clear();
		}
}
