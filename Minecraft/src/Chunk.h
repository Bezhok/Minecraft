#pragma once

#include "pch.h"
#include "game_constants.h"
#include "Buffers.h"
#include "ChunkMeshBasic.h"

namespace World {
    class Map;

    enum class BlockType : uint8_t;

    class ChunkLayer {
    public:
        bool is_all_solid() { return solid_block_count == BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK; };

        void update(BlockType type);

    private:
        int solid_block_count = 0;
    };

    class Chunk {
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
        static bool is_block_type_transparent(BlockType type);

        static bool is_block_type_solid(BlockType type);

        void init(const sf::Vector3i &pos, World::Map *map);

        Chunk();

        ~Chunk();

        const sf::Vector3i &get_pos() { return m_pos; };

        enum BlockType get_block_type(int x, int y, int z);

        void set_block_type(int x, int y, int z, enum BlockType type);

        ChunkMeshBasic m_blocks_mesh;
        ChunkMeshBasic m_water_mesh;
    private:
        void generate_vertices();

        bool is_layer_solid(sf::Vector3i pos, int y);

        bool is_empty();

        inline void generate_cactus_vertices(GLfloat x, GLfloat y, GLfloat z, BlockType id);

        inline void generate_water_vertices(int i, int j, int k, GLfloat x, GLfloat y, GLfloat z, BlockType id);

        inline void generate_block_vertices(int i, int j, int k, GLfloat x, GLfloat y, GLfloat z, BlockType id);

        sf::Vector3i m_pos = {-1, -1, -1};
        std::array<BlockType, BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK> m_blocks;
        std::array<ChunkLayer, BLOCKS_IN_CHUNK> m_layers;
        bool m_is_blocked_vertices_generation = false;
        bool m_is_rendering = false;
        bool m_is_init = false;
        World::Map *m_map = nullptr;
    };
}