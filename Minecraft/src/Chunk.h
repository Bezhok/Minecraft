#pragma once

#include "pch.h"
#include "game_constants.h"
#include "Buffers.h"
#include "ChunkMeshBasic.h"

namespace World {
    class Map;

    enum class block_id : uint8_t;

    class Chunk {
        class ChunkLayer {
        public:
            bool is_all_solid() { return solid_block_count == BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK; };

            void update(block_id type);

        private:
            int solid_block_count = 0;
        };

    private:
        sf::Vector3i m_pos = {-1, -1, -1};
        std::array<block_id, BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK> m_blocks;
        std::array<ChunkLayer, BLOCKS_IN_CHUNK> m_layers;

        bool m_is_blocked_vertices_generation = false;
        bool m_is_rendering = false;
        bool m_is_init = false;
        World::Map *m_map = nullptr;

    public:
        ChunkMeshBasic m_blocks_mesh;
        ChunkMeshBasic m_water_mesh;

    private:
        void generate_vertices();

        bool is_layer_solid(sf::Vector3i pos, int y);

        bool is_empty();

    public:
        /* for Block mesh basic */
        bool should_make_layer(int y);

        bool is_air__in_chunk(int mx, int my, int mz);

        bool is_opaque__in_chunk(int x, int y, int z);

        bool is_water__in_chunk(int x, int y, int z);

        /* Related to meshes */
        bool is_rendering() { return m_is_rendering; };

        bool can_generate_vertices() { return !is_empty() && !is_rendering() && !m_is_blocked_vertices_generation; };

        void set_is_rendering(bool flag) { m_is_rendering = flag; };

        void update_vao();

        void update_vertices();

        void free_buffers();

        /* Main */
        static bool is_block_type_transperent(block_id type);

        static bool is_block_type_solid(block_id type);

        void init(const sf::Vector3i &pos, World::Map *map);

        Chunk();

        ~Chunk();

        const sf::Vector3i &get_pos() { return m_pos; };

        enum block_id get_block_type(int x, int y, int z);

        void set_block_type(int x, int y, int z, enum block_id type);
    };
}