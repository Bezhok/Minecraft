#pragma once

#include "pch.h"
#include "game_constants.h"

class Light {
public:
    Light();

    ~Light();

    const glm::vec3 &get_light_position();

    const glm::vec3 &get_light_direction();

    const glm::mat4 &get_light_projection_view();

    sf::Glsl::Vec3 calc_gl_sky_color();

    sf::Glsl::Vec3 calc_gl_light_color();

    void update(const sf::Vector3f &player_position);

private:
    void fix_shadow(glm::mat4 &shadow_matrix);

    void update_angle(float delta);

    void calculate_projection();

    void calculate_view();

    void update_sun(const sf::Vector3f &player_position);

    const sf::Color DAY{145, 186, 252};
    const sf::Color NIGHT{28, 40, 80};
    const float light_movement_radius = RENDER_DISTANCE / 1;
    const float y_rotation_in_deg = 0;

    float m_light_angle_in_deg;
    sf::Color m_sky_color = DAY;
    sf::Color m_light_color{255, 255, 255};
    glm::vec3 m_position{};
    glm::vec3 m_direction{};

    glm::mat4 m_projection_view{};
    glm::mat4 m_light_projection{};
    glm::mat4 m_light_view{};
    bool m_is_day = true;
};

