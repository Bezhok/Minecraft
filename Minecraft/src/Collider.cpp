#include "Collider.h"
#include "Converter.h"

void Collider::push_out(sf::Vector3i from, sf::Vector3f d) {
    if (d.x > 0) m_pos->x = from.x - m_size->x;
    if (d.x < 0) m_pos->x = from.x + 1 + m_size->x;
    if (d.y > 0) m_pos->y = from.y - m_size->y;
    if (d.y < 0) {
        m_pos->y = from.y + 1 + m_size->y;
        m_is_on_ground = true;
    }
    if (d.z > 0) m_pos->z = from.z - m_size->z;
    if (d.z < 0) m_pos->z = from.z + 1 + m_size->z;
}

void Collider::iterate_throw_near_blocks(std::function<bool(int, int, int)> &&fun) {
    auto start = World::Converter::coord2block_coord(*m_pos - *m_size);
    sf::Vector3<float> end;
    end = *m_pos + *m_size;
    for (int x = start.x; x < end.x; x++) {
        for (int y = start.y; y < end.y; y++) {
            for (int z = start.z; z < end.z; z++) {
                // bool - should continue
                if (!fun(x, y, z)) return;
            }
        }
    }
}

void Collider::init(sf::Vector3f *pos, sf::Vector3f *size) {
    m_pos = pos;
    m_size = size;
}

bool Collider::is_on_ground() const {
    return m_is_on_ground;
}
void Collider::set_is_on_ground(bool is_on_ground) {
    m_is_on_ground = is_on_ground;
}
