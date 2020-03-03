#pragma once

#include "pch.h"
#include "Buffers.h"

using VertexType = GLfloat;


namespace World {
    enum class block_id : uint8_t;

    class Chunk;

    class Map;

    class ChunkMeshBasic {
    private:
        const VertexType BS = 1;

        bool m_is_vertices_created = false;
        VertexType *m_vertices = nullptr;


        //vertices count in current time
        int m_i = 0;

        // vertices count before updating
        int m_old_i = 0;

        Buffers m_buffers;
        Chunk *chunk = nullptr;

    private:
        void bind_texture_second_order(block_id id, const sf::Vector2i &p);

        void bind_texture_first_order(block_id id, const sf::Vector2i &p);

        void bind_texture2positive_x(block_id id);

        void bind_texture2negative_x(block_id id);

        void bind_texture2negative_y(block_id id);

        void bind_texture2positive_y(block_id id);

        void bind_texture2negative_z(block_id id);

        void bind_texture2positive_z(block_id id);

        void add_byte4(VertexType x, VertexType y, VertexType z, VertexType w);

    public:
        /* Main */
        explicit ChunkMeshBasic(Chunk *c) : chunk(c) {};

        ~ChunkMeshBasic();

        void update_vao();

        void update_vertices();

        void free_buffers(Map *map);

        void draw();

        int get_final_points_count() { return m_old_i / 6; };

        /* debug */
        int get_current_faces_count() { return m_i / 36; };

        /* only for Chunk */
        void generate_vertices4positive_x(VertexType x, VertexType y, VertexType z, block_id id);

        void generate_vertices4negative_x(VertexType x, VertexType y, VertexType z, block_id id);

        void generate_vertices4negative_y(VertexType x, VertexType y, VertexType z, block_id id);

        void generate_vertices4positive_y(VertexType x, VertexType y, VertexType z, block_id id);

        void generate_vertices4negative_z(VertexType x, VertexType y, VertexType z, block_id id);

        void generate_vertices4positive_z(VertexType x, VertexType y, VertexType z, block_id id);
    };
}