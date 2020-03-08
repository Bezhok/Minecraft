#pragma once

#include <atomic>
#include "pch.h"
#include "game_constants.h"

struct RenderRange;
class Player;
namespace World {
    class Map;

    class Chunk;

    class MapMeshBuilder {
     private:
        Map *m_map = nullptr;
        Player *m_player = nullptr;
        sf::Window *m_window = nullptr;
        sf::Thread m_vertices_generator_thread;
        sf::Thread m_update_edited_chunk_thread;
        std::vector<World::Chunk *> m_chunks4vertices_generation;
        std::vector<World::Chunk *> m_chunks4vbo_generation;
        std::atomic<size_t> chunks4vbo_generation_size;

        bool m_is_thread_free = true;
        bool m_should_update_priority_chunks = false;
        std::vector<Chunk *> m_priority4_rendering;
        bool m_is_new_chunk = false;
        int m_visible_columns_count = 0;
        const float SPHERE_DIAMETER = sqrtf(3.f * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK);
     public:
        sf::Mutex m_mutex__chunks4vbo_generation, m_mutex__chunks4rendering;
        phmap::parallel_node_hash_set<World::Chunk *> m_chunks4rendering;

     private:
        void add_chunks_range2vertices_generation(RenderRange &range);

        void unload_columns(RenderRange &range);

        void add_visible_chunks_in_range2vertices_generation(int i, int k, glm::mat4 &pv, RenderRange &range);

        void update_edited_chunk();

     public:
        void wait();

        void generate_vertices();

        void add_new_chunks2rendering();

        void regenerate_edited_chunk_vertices();

        void launch(Map *map, Player *player, sf::Window *window);

        MapMeshBuilder();

        size_t get_chunks4vertices_generation_size() { return m_chunks4vertices_generation.size(); };

        size_t get_chunks4rendering_size() { return m_chunks4rendering.size(); };

        bool add_column2vertices_generation(std::array<Chunk, 16> &column);
    };
}