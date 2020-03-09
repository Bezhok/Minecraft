#pragma once

#include "pch.h"
#include "Buffers.h"

namespace World {
    enum class BlockType : uint8_t;
    class Chunk;
    class Map;

    class ChunkMeshBasic {
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

    private:
        inline void bind_texture_second_order(const sf::Vector2i &t_p);

        inline void bind_texture_first_order(const sf::Vector2i &t_p);

        inline void bind_texture2positive_x(BlockType id);

        inline void bind_texture2negative_x(BlockType id);

        inline void bind_texture2negative_y(BlockType id);

        inline void bind_texture2positive_y(BlockType id);

        inline void bind_texture2negative_z(BlockType id);

        inline void bind_texture2positive_z(BlockType id);

        inline void add_byte4(GLfloat x, GLfloat y, GLfloat z, GLfloat w);

        friend class Chunk;

        /* only for Chunk */
        void generate_vertices4positive_x(GLfloat x, GLfloat y, GLfloat z, BlockType id);

        void generate_vertices4negative_x(GLfloat x, GLfloat y, GLfloat z, BlockType id);

        void generate_vertices4negative_y(GLfloat x, GLfloat y, GLfloat z, BlockType id);

        void generate_vertices4positive_y(GLfloat x, GLfloat y, GLfloat z, BlockType id);

        void generate_vertices4negative_z(GLfloat x, GLfloat y, GLfloat z, BlockType id);

        void generate_vertices4positive_z(GLfloat x, GLfloat y, GLfloat z, BlockType id);

        const GLfloat BS = 1;
        bool m_is_vertices_created = false;
        GLfloat *m_vertices = nullptr;

        //vertices count in current time
        int m_i = 0;

        // vertices count before updating
        int m_old_i = 0;
        Buffers m_buffers;
        Chunk *chunk = nullptr;

    };
}