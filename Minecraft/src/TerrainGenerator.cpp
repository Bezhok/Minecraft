#include "TerrainGenerator.h"
#include "Map.h"
#include "Converter.h"
#include "block_db.h"
#include "Biome.h"
#include "DesertBiome.h"
#include "PlainsBiome.h"

using namespace World;

TerrainGenerator::TerrainGenerator(Map *map) : m_map(map) {

    srand(m_noise.GetSeed());

    m_noise.SetNoiseType(FastNoise::PerlinFractal);
    m_noise.SetFrequency(0.01f);
    m_noise.SetFractalOctaves(1);
    m_noise.SetInterp(FastNoise::Hermite);

    m_biome_noise.SetNoiseType(FastNoise::ValueFractal);
    m_biome_noise.SetFrequency(0.002f);
    m_biome_noise.SetFractalOctaves(5);

    m_offset = 0;//30
    m_water_level = m_offset + 30;
}

TerrainGenerator::~TerrainGenerator() {

}

inline void TerrainGenerator::generate_chunk_terrain(int chunk_x, int chunk_y, int chunk_z) {
    auto &column = m_map->get_column(chunk_x, chunk_z);
    generate_chunk_terrain(column, chunk_x, chunk_y, chunk_z);
}

void World::TerrainGenerator::generate_chunk_terrain(Column &column, int chunk_x,
                                                     int chunk_y, int chunk_z) {
    Chunk &chunk = column[chunk_y];
    chunk.init({chunk_x, chunk_y, chunk_z}, m_map);

    srand(chunk_x + chunk_z * chunk_z);

    PlainsBiome plain(m_water_level);
    DesertBiome desert(m_water_level);
    PlainsBiome groove(m_water_level);
    groove.set_tree_frequency(30);

    Biome *biome;
    for (int block_x = 0; block_x < BLOCKS_IN_CHUNK; ++block_x)
        for (int block_z = 0; block_z < BLOCKS_IN_CHUNK; ++block_z) {
            // noise calculating
            int x = Converter::chunk_coord2block_coord(chunk_x) + block_x;
            int z = Converter::chunk_coord2block_coord(chunk_z) + block_z;
            int ground_h = 0;
            float biome_noise = m_biome_noise.GetNoise(static_cast<float>(x) + 1, static_cast<float>(z) + 1);
            float shifted_noise = get_noise(x + 1, z + 1) + 1;

            // biome determining
            if (biome_noise >= 0) {
                if (biome_noise > 0.2) {
                    biome = &plain;
                } else {
                    biome = &groove;
                }
                ground_h = static_cast<int>(shifted_noise * 40) + m_offset;
            } else {
                biome = &desert;
                ground_h = static_cast<int>(shifted_noise * 15) + m_offset + 19;
            }

            // blocks generation
            for (int block_y = 0; block_y < BLOCKS_IN_CHUNK; ++block_y) {
                int block_global_y = Converter::chunk_coord2block_coord(chunk_y) + block_y;

                BlockType id = biome->generate_block({block_x, block_y, block_z}, ground_h, block_global_y);
                if (id == BlockType::Air) break;
                chunk.set_block_type(block_x, block_y, block_z, id);
            }

            generate_structures(column, chunk_y, biome);
        }
}

void TerrainGenerator::generate_structures(Column &column, int chunk_y, Biome *biome) {
    // tree generation
    for (auto &pos : biome->get_tree_positions()) {
        biome->generate_tree([&](sf::Vector3i loc_pos, BlockType type) {
          sf::Vector3i full_pos = pos + loc_pos;
          m_map->set_block_type(full_pos, column, chunk_y, type);
        });
    }
    biome->clear();
}