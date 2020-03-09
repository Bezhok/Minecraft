#pragma once

#include "pch.h"

class Collider {
public:
    bool is_on_ground() const;

    void set_is_on_ground(bool is_on_ground);

    void init(sf::Vector3f *pos, sf::Vector3f *size);

    void iterate_throw_near_blocks(std::function<bool(int, int, int)> &&fun);

    void push_out(sf::Vector3i from, sf::Vector3f d);

private:
    sf::Vector3f *m_pos, *m_size;
    bool m_is_on_ground;
};
