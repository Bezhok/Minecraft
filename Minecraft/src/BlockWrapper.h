#pragma once

#include "Buffers.h"

class BlockWrapper {
public:
    void draw();

    BlockWrapper();

    ~BlockWrapper();

    const sf::Vector3f &get_pos() const;

    void set_pos(const sf::Vector3f &pos);

    void set_pos(const sf::Vector3i &pos);

    void update_buffers();

private:
    void update_vertices(sf::Vector3i &bpos);

    sf::Vector3f m_pos;
    Buffers m_buffers;
    std::vector<GLfloat> m_vertices;
};

