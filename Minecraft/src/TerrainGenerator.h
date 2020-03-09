#pragma once

#include "pch.h"
#include "game_constants.h"
#include "Chunk.h"

namespace World {
    class Map;
    class Biome;
}

namespace World {
    class TerrainGenerator {
        using Column = std::array<Chunk, CHUNKS_IN_WORLD_HEIGHT>;
    public:
        float get_noise(int x, int y) { return m_noise.GetNoise(static_cast<float>(x), static_cast<float>(y)); };

        float get_noise(float x, float y) { return m_noise.GetNoise(x, y); };

        void generate_chunk_terrain(int chunk_x, int chunk_y, int chunk_z);

        void generate_chunk_terrain(Column &column, int chunk_x, int chunk_y,
                                    int chunk_z);

        explicit TerrainGenerator(Map *map);

        ~TerrainGenerator();

    private:
        void generate_structures(Column &column, int chunk_y, Biome *biome);

        Map *m_map;
        FastNoise m_noise;
        FastNoise m_biome_noise;

        int m_offset = 0;
        int m_water_level = 0;
    };
}
   