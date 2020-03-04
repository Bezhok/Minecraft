#pragma once

#include "pch.h"
#include "game_constants.h"

#include "Chunk.h"

namespace World {
    class Map;
}

namespace World {
    class TerrainGenerator {
    private:
        TerrainGenerator();

        FastNoise m_noise;

    public:
        FastNoise m_biome_noise;

        float get_noise(int x, int y) { return m_noise.GetNoise(static_cast<float>(x), static_cast<float>(y)); };

        float get_noise(float x, float y) { return m_noise.GetNoise(x, y); };

        void generate_chunk_terrain(int chunk_x, int chunk_y, int chunk_z);

        void generate_chunk_terrain(std::array<Chunk, CHUNKS_IN_WORLD_HEIGHT> &column, int chunk_x, int chunk_y,
                                    int chunk_z);

        Map *m_map;

        TerrainGenerator(Map *map);

        ~TerrainGenerator();

        void generate_tree(sf::Vector3i &pos, std::array<Chunk, CHUNKS_IN_WORLD_HEIGHT> &column, int chunk_y);

        void generate_cactus(sf::Vector3i &pos, std::array<Chunk, CHUNKS_IN_WORLD_HEIGHT> &column, int chunk_y);
    };
}
   