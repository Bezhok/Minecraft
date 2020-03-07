#include "Camera.h"
#include "game_constants.h"
#include "Converter.h"
#include "Map.h"

using World::Converter;

glm::mat4 Camera::calc_projection_view(sf::Vector2u &window_size) const {
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat) window_size.x / (GLfloat) window_size.y,
                                            0.1f, RENDER_DISTANCE);
    glm::mat4 view = glm::lookAt(
        glm::vec3(
            m_pos->x,
            m_pos->y + 0.8f,//m_size.y
            m_pos->z
        ),
        glm::vec3(
            m_pos->x - sin(glm::radians(m_angle.x)),
            m_pos->y + 0.8f + tan(glm::radians(m_angle.y)),
            m_pos->z - cos(glm::radians(m_angle.x))
        ),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return projection * view;
}
sf::Vector3i Camera::determine_look_at_block(sf::Vector3i *prev_pos) const {
    sf::Vector3f curr = *m_pos;
    curr.y += 0.8f;
    sf::Vector3f prev = curr;

    auto dist = [&]() -> float {
      return (m_pos->x - curr.x) * (m_pos->x - curr.x) + (m_pos->y - curr.y) * (m_pos->y - curr.y)
          + (m_pos->z - curr.z) * (m_pos->z - curr.z);
    };

    while (dist() < 36) {
        curr.x += -sinf(glm::radians(m_angle.x)) / 80.F;
        curr.y += tanf(glm::radians(m_angle.y)) / 80.F;
        curr.z += -cosf(glm::radians(m_angle.x)) / 80.F;

        sf::Vector3i conv = Converter::coord2block_coord(curr);
        if (!m_map->is_air(conv.x, conv.y, conv.z) &&
            !m_map->is_water(conv.x, conv.y, conv.z) &&
            conv.y >= 0
            && conv.y < BLOCKS_IN_CHUNK * CHUNKS_IN_WORLD_HEIGHT
            ) {
            if (prev_pos != nullptr) {
                *prev_pos = Converter::coord2block_coord(prev);
            }
            return conv;
        }
        prev = curr;
    }

    return {-1, -1, -1};
}
void Camera::init(sf::Vector3f *pos, World::Map *map) {
    m_pos = pos;
    m_map = map;
}
void Camera::rotate(sf::Vector2f delta) {
    m_angle += delta;
    if (m_angle.y < -89) m_angle.y = -89;
    if (m_angle.y > 89) m_angle.y = 89;
    if (m_angle.x > 361) m_angle.x -= 360;
    if (m_angle.x < -361) m_angle.x += 360;
}

