#pragma once

#include "pch.h"

namespace World {
    class Map;
}

class Camera {
public:
    void set_shift(sf::Vector3f shift);

    sf::Vector3i determine_look_at_block(sf::Vector3i *prev_pos = nullptr) const;

    glm::mat4 calc_projection_view(sf::Vector2u &window_size) const;

    void rotate(sf::Vector2f delta);

    void init(sf::Vector3f *pos, World::Map *map);

private:
    friend class Player;
    sf::Vector2f m_angle;
    sf::Vector3f *m_pos = nullptr;
    sf::Vector3f m_shift;
    World::Map *m_map = nullptr;
};
