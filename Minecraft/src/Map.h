#pragma once

#include "pch.h"
#include "game_constants.h"
#include "Buffers.h"
#include "Sounds.h"

namespace World {
    class TerrainGenerator;

    class Chunk;

    enum class block_id : uint8_t;

    class Map {
    public:
        using Column = std::array<Chunk, CHUNKS_IN_WORLD_HEIGHT>;
    private:
        phmap::parallel_node_hash_map<size_t, Column> m_map;
        bool m_should_redraw_chunk = false;
        std::unique_ptr<TerrainGenerator> m_terrain_generator;
        block_id m_edited_block_type;
        sf::Vector3i m_edited_block_pos;
        Chunk *m_edited_chunk = nullptr;
        Sounds m_sounds;
    private:
        void play_sound(Sounds::SoundsNames name);

        void
        find_neighbours(const sf::Vector3i &chunk_pos, const sf::Vector3i &block_in_chunk_pos, int x, int y, int z);

        void recalculate_pos(sf::Vector3i &pos_rel2chunk, sf::Vector3i &chunk_pos);

    public:
        size_t get_size() { return m_map.size(); };

        void generate_chunk_terrain(Column &, int, int, int);

        /* load world */
        Map();

        ~Map();

        /* eponymous */
        bool is_solid(int mx, int my, int mz);

        bool is_opaque(int mx, int my, int mz);

        bool is_air(int mx, int my, int mz);

        bool is_water(int x, int y, int z);

        bool create_block(int x, int y, int z, block_id type);

        bool delete_block(int x, int y, int z);

        std::vector<sf::Vector3i> m_should_be_updated_neighbours;

        /* when you add/delete block */
        bool is_chunk_edited() { return m_should_redraw_chunk; };

        void apply_chunk_changes();

        Chunk *get_edited_chunk() { return m_edited_chunk; };

        // glm hash function
        inline void hash_combine(size_t &seed, size_t hash) {
            hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash;
        }

        inline size_t hashXZ(int i, int k) {
            size_t seed = 0;
            std::hash<int> hasher;
            hash_combine(seed, hasher(i));
            hash_combine(seed, hasher(k));
            return seed;
        }

        void unload_columns(int start_x, int end_x, int start_z, int end_z);

        void unload_column(int i, int k);

        Column &get_column(int i, int k);

        Chunk &get_chunk(int i, int j, int k);

        Chunk &get_chunk_or_generate(int i, int j, int k);

        Column &get_column_or_generate(int i, int k);

        std::vector<Buffers> m_should_be_freed_buffers;

        void set_block_type(sf::Vector3i pos_in_chunk, Map::Column &column, int chunk_y, block_id type);

        inline block_id get_type(int x, int y, int z);
    };
}

