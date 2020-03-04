#include "pch.h"
#include "BlockWrapper.h"
#include "game_constants.h"
#include "Map.h"

using namespace World;

void BlockWrapper::draw() {
    glLineWidth(6);

    sf::Vector3i bpos = {
            Map::coord2block_coord_in_chunk(m_pos.x),
            Map::coord2block_coord_in_chunk(m_pos.y),
            Map::coord2block_coord_in_chunk(m_pos.z)
    };

    update_vertices(bpos);

    if (m_buffers.VAO == 0) {
        glGenVertexArrays(1, &m_buffers.VAO);
        glGenBuffers(1, &m_buffers.VBO);
    }

    update_buffers();

    glBindVertexArray(m_buffers.VAO);
    glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size()/3);
    glBindVertexArray(0);

    glLineWidth(1);
}

BlockWrapper::BlockWrapper() {
    m_vertices.resize(60);
}

void BlockWrapper::update_buffers() {
    glBindVertexArray(m_buffers.VAO);
    /**/

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * m_vertices.size(), m_vertices.data(),
                 GL_STATIC_DRAW); //GL_DYNAMIC_DRAW GL_STATIC_DRAW GL_STREAM_DRAW

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GL_FLOAT), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    /**/
    glBindVertexArray(0); // Unbind VAO
}

void BlockWrapper::update_vertices(sf::Vector3i &bpos) {
    float m = 0.02f;
    float p = 1 + m;
    m_vertices = {
            bpos.x - m, bpos.y + p, bpos.z - m,
            bpos.x + p, bpos.y + p, bpos.z - m,
            bpos.x + p, bpos.y + p, bpos.z + p,
            bpos.x - m, bpos.y + p, bpos.z + p,
            bpos.x - m, bpos.y + p, bpos.z - m,

            bpos.x - m, bpos.y - m, bpos.z - m,
            bpos.x + p, bpos.y - m, bpos.z - m,
            bpos.x + p, bpos.y - m, bpos.z + p,
            bpos.x - m, bpos.y - m, bpos.z + p,
            bpos.x - m, bpos.y - m, bpos.z - m,

            bpos.x + p, bpos.y - m, bpos.z - m,
            bpos.x + p, bpos.y + p, bpos.z - m,
            bpos.x + p, bpos.y + p, bpos.z + p,
            bpos.x + p, bpos.y - m, bpos.z + p,
            bpos.x + p, bpos.y - m, bpos.z - m,

            bpos.x - m, bpos.y - m, bpos.z - m,
            bpos.x - m, bpos.y + p, bpos.z - m,
            bpos.x - m, bpos.y + p, bpos.z + p,
            bpos.x - m, bpos.y - m, bpos.z + p,
            bpos.x - m, bpos.y - m, bpos.z - m
    };
}

const sf::Vector3f &BlockWrapper::get_pos() const {
    return m_pos;
}

void BlockWrapper::set_pos(const sf::Vector3f &pos) {
    m_pos = pos;
}

void BlockWrapper::set_pos(const sf::Vector3i &pos) {
    m_pos = sf::Vector3f(pos);
}

BlockWrapper::~BlockWrapper() {
    glDeleteVertexArrays(1, &m_buffers.VAO);
    glDeleteBuffers(1, &m_buffers.VBO);
}
