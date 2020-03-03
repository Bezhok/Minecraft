#pragma once

#include "pch.h"

class Light {
private:
    const sf::Color DAY{145, 186, 252};
    const sf::Color NIGHT{28, 40, 80};

    sf::Clock m_time;
    float m_light_angle_in_deg;
    sf::Color m_sky_color = DAY;
    sf::Color m_light_color{255, 255, 255};
    glm::vec3 m_position;
    glm::vec3 m_direction;

    glm::mat4 m_projection_view;
    bool m_is_day = true;
public:
    Light();

    ~Light();

    const glm::vec3 &get_light_position();

    const glm::vec3 &get_light_direction();

    const glm::mat4 &get_light_projection_view();

    sf::Glsl::Vec3 calc_gl_sky_color();

    sf::Glsl::Vec3 calc_gl_light_color();

    void update(const sf::Vector3f &player_position);
};

