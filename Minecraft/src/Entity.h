#pragma once

#include "pch.h"

namespace World {
    class Map;
}

namespace Base {
    class Entity {
     protected:
        sf::Vector3f m_pos;
        sf::Vector3f m_rotation;
        sf::Vector3f m_size;
        World::Map *m_map = nullptr;

     public:
        virtual bool is_on_ground() const = 0;
        virtual void set_is_on_ground(bool is_on_ground) = 0;
        virtual bool is_in_water() const = 0;
        /* eponymous */
        const sf::Vector3f &get_position() const { return m_pos; }
    };
}
