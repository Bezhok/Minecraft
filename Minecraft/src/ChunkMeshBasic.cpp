#include "pch.h"
#include "block_db.h"
#include "Map.h"
#include "ChunkMeshBasic.h"
#include "Chunk.h"

using namespace World;

inline void ChunkMeshBasic::add_byte4(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    m_vertices[m_i++] = x;
    m_vertices[m_i++] = y;
    m_vertices[m_i++] = z;
    m_vertices[m_i++] = w;
    m_i += 2;
}

ChunkMeshBasic::~ChunkMeshBasic() {
    if (m_is_vertices_created) {
        delete[] m_vertices;
        --vertices_wasnt_free;
    }
}

void ChunkMeshBasic::update_vao() {
    if (m_is_vertices_created) {
        m_old_i = m_i;
        if (m_i > 0) {
            if (m_buffers.VAO == 0) {
                glGenVertexArrays(1, &m_buffers.VAO);
                glGenBuffers(1, &m_buffers.VBO);
            }

            glBindVertexArray(m_buffers.VAO);
            /**/
            glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
            glBufferData(GL_ARRAY_BUFFER, m_i * sizeof(GLfloat), m_vertices,
                         GL_STATIC_DRAW); //GL_DYNAMIC_DRAW GL_STATIC_DRAW

            // Position attribute
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *) nullptr);
            glEnableVertexAttribArray(0);

            // TexCoord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                                  (GLvoid *) (4 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
            /**/
            glBindVertexArray(0); // Unbind VAO
        }
        delete[] m_vertices;
        --vertices_wasnt_free;
        m_is_vertices_created = false;
    }
}

void ChunkMeshBasic::update_vertices() {
    if (m_is_vertices_created) {
        delete[] m_vertices;
        --vertices_wasnt_free;
    }

    m_i = 0;
    m_vertices = new GLfloat[BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK * 36 * 6];
    m_is_vertices_created = true;
    ++vertices_wasnt_free;
}

void World::ChunkMeshBasic::free_buffers(Map *map) {
    if (m_buffers.VAO != 0) {
        map->m_should_be_freed_buffers.push_back(m_buffers);
        m_buffers = Buffers();
    }
}

void ChunkMeshBasic::bind_texture_first_order(const sf::Vector2i &t_p) {
    m_vertices[m_i + 4] = t_p.x;
    m_vertices[m_i + 5] = (t_p.y + 1);
    m_vertices[m_i + 10] = (t_p.x + 1);
    m_vertices[m_i + 11] = (t_p.y + 1);
    m_vertices[m_i + 16] = t_p.x;
    m_vertices[m_i + 17] = t_p.y;
    m_vertices[m_i + 22] = t_p.x;
    m_vertices[m_i + 23] = t_p.y;
    m_vertices[m_i + 28] = (t_p.x + 1);
    m_vertices[m_i + 29] = (t_p.y + 1);
    m_vertices[m_i + 34] = (t_p.x + 1);
    m_vertices[m_i + 35] = t_p.y;
}

void ChunkMeshBasic::bind_texture_second_order(const sf::Vector2i &t_p) {
    m_vertices[m_i + 4] = t_p.x;
    m_vertices[m_i + 5] = (t_p.y + 1);
    m_vertices[m_i + 10] = t_p.x;
    m_vertices[m_i + 11] = t_p.y;
    m_vertices[m_i + 16] = (t_p.x + 1);
    m_vertices[m_i + 17] = (t_p.y + 1);
    m_vertices[m_i + 22] = t_p.x;
    m_vertices[m_i + 23] = t_p.y;
    m_vertices[m_i + 28] = (t_p.x + 1);
    m_vertices[m_i + 29] = t_p.y;
    m_vertices[m_i + 34] = (t_p.x + 1);
    m_vertices[m_i + 35] = (t_p.y + 1);
}

inline void ChunkMeshBasic::bind_texture2negative_x(BlockType id) {
    bind_texture_first_order(DB::s_atlas_db(id, Side::negative_x));
}

inline void ChunkMeshBasic::bind_texture2positive_x(BlockType id) {
    bind_texture_second_order(DB::s_atlas_db(id, Side::positive_x));
}

inline void ChunkMeshBasic::bind_texture2negative_y(BlockType id) {
    bind_texture_second_order(DB::s_atlas_db(id, Side::negative_y));
}

inline void ChunkMeshBasic::bind_texture2positive_y(BlockType id) {
    bind_texture_first_order(DB::s_atlas_db(id, Side::positive_y));
}

inline void ChunkMeshBasic::bind_texture2negative_z(BlockType id) {
    bind_texture_second_order(DB::s_atlas_db(id, Side::negative_z));
}

inline void ChunkMeshBasic::bind_texture2positive_z(BlockType id) {
    bind_texture_first_order(DB::s_atlas_db(id, Side::positive_z));
}

void World::ChunkMeshBasic::draw() {
    glBindVertexArray(m_buffers.VAO);
    glDrawArrays(GL_TRIANGLES, 0, get_final_points_count());
    glBindVertexArray(0);
}

void World::ChunkMeshBasic::generate_vertices4positive_x(GLfloat x, GLfloat y, GLfloat z, BlockType id) {
    GLfloat normal_id = 2.2f;
    bind_texture2positive_x(id);
    add_byte4(x + BS, y, z, normal_id);
    add_byte4(x + BS, y + BS, z, normal_id);
    add_byte4(x + BS, y, z + BS, normal_id);
    add_byte4(x + BS, y + BS, z, normal_id);
    add_byte4(x + BS, y + BS, z + BS, normal_id);
    add_byte4(x + BS, y, z + BS, normal_id);
}

void World::ChunkMeshBasic::generate_vertices4negative_x(GLfloat x, GLfloat y, GLfloat z, BlockType id) {
    GLfloat normal_id = 1.2f;
    bind_texture2negative_x(id);
    add_byte4(x, y, z, normal_id);
    add_byte4(x, y, z + BS, normal_id);
    add_byte4(x, y + BS, z, normal_id);
    add_byte4(x, y + BS, z, normal_id);
    add_byte4(x, y, z + BS, normal_id);
    add_byte4(x, y + BS, z + BS, normal_id);
}

void World::ChunkMeshBasic::generate_vertices4negative_y(GLfloat x, GLfloat y, GLfloat z, BlockType id) {
    GLfloat normal_id = 3.2f;
    bind_texture2negative_y(id);
    add_byte4(x, y, z, normal_id);
    add_byte4(x + BS, y, z, normal_id);
    add_byte4(x, y, z + BS, normal_id);
    add_byte4(x + BS, y, z, normal_id);
    add_byte4(x + BS, y, z + BS, normal_id);
    add_byte4(x, y, z + BS, normal_id);
}

void World::ChunkMeshBasic::generate_vertices4positive_y(GLfloat x, GLfloat y, GLfloat z, BlockType id) {
    bind_texture2positive_y(id);
    if (id != BlockType::Water) {
        GLfloat normal_id = 4.2f;
        add_byte4(x, y + BS, z, normal_id);
        add_byte4(x, y + BS, z + BS, normal_id);
        add_byte4(x + BS, y + BS, z, normal_id);
        add_byte4(x + BS, y + BS, z, normal_id);
        add_byte4(x, y + BS, z + BS, normal_id);
        add_byte4(x + BS, y + BS, z + BS, normal_id);
    } else {
        add_byte4(x, y + BS, z, 11.2f);
        add_byte4(x, y + BS, z + BS, 12.2f);
        add_byte4(x + BS, y + BS, z, 13.2f);
        add_byte4(x + BS, y + BS, z, 14.2f);
        add_byte4(x, y + BS, z + BS, 15.2f);
        add_byte4(x + BS, y + BS, z + BS, 16.2f);
    }
}

void World::ChunkMeshBasic::generate_vertices4negative_z(GLfloat x, GLfloat y, GLfloat z, BlockType id) {
    GLfloat normal_id = 5.2f;
    bind_texture2negative_z(id);
    add_byte4(x, y, z, normal_id);
    add_byte4(x, y + BS, z, normal_id);
    add_byte4(x + BS, y, z, normal_id);
    add_byte4(x, y + BS, z, normal_id);
    add_byte4(x + BS, y + BS, z, normal_id);
    add_byte4(x + BS, y, z, normal_id);
}

void World::ChunkMeshBasic::generate_vertices4positive_z(GLfloat x, GLfloat y, GLfloat z, BlockType id) {
    GLfloat normal_id = 6.2f;
    bind_texture2positive_z(id);
    add_byte4(x, y, z + BS, normal_id);
    add_byte4(x + BS, y, z + BS, normal_id);
    add_byte4(x, y + BS, z + BS, normal_id);
    add_byte4(x, y + BS, z + BS, normal_id);
    add_byte4(x + BS, y, z + BS, normal_id);
    add_byte4(x + BS, y + BS, z + BS, normal_id);
}
