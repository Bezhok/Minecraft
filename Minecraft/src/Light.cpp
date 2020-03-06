#include "Light.h"
#include "pch.h"
#include "game_constants.h"

Light::Light() {
    m_light_angle_in_deg = 89.9f;
    m_light_angle_in_deg -= 2.6 * 15;
}

Light::~Light() = default;

const glm::vec3 &Light::get_light_position() {
    return m_position;
}

const glm::vec3 &Light::get_light_direction() {
    return m_direction;
}

const glm::mat4 &Light::get_light_projection_view() {
    return m_projection_view;
}

sf::Glsl::Vec3 Light::calc_gl_sky_color() {

    return {m_sky_color.r / 255.f, m_sky_color.g / 255.f, m_sky_color.b / 255.f};
}

sf::Glsl::Vec3 Light::calc_gl_light_color() {
    return {m_light_color.r / 255.f, m_light_color.g / 255.f, m_light_color.b / 255.f};
}

void Light::update_angle(float delta) {
    m_light_angle_in_deg += delta;

    // always day
    if (m_light_angle_in_deg > 178) {
        m_light_angle_in_deg = 2;
    }

    if (m_light_angle_in_deg < 2) {
        m_light_angle_in_deg = 178;
    }

    if (m_is_day) {
        m_sky_color = DAY;
        m_light_color = {255, 255, 255};
    } else {
        m_sky_color = NIGHT;
        m_light_color = {25, 25, 25};
    }
}

void Light::update(const sf::Vector3f &player_position) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        update_angle(+2.6f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        update_angle(-2.6f);
    }

    update_sun(player_position);
    calculate_projection();
    calculate_view();

    // fixes "swimming" effect
    glm::mat4 shadow_matrix = m_light_projection * m_light_view;
    fix_shadow(shadow_matrix);

    m_direction = glm::normalize(m_direction);
}

void Light::update_sun(const sf::Vector3f &player_position) {

    float y = light_movement_radius / sqrtf(1 + powf(1.f / tanf(glm::radians(m_light_angle_in_deg)), 2));
    float x = y / tanf(glm::radians(m_light_angle_in_deg));
    glm::mat4 sun_mat(1.f);
    m_position =
        glm::rotate(sun_mat, glm::radians(y_rotation_in_deg), glm::vec3(0.0, 1.0, 0.0)) * glm::vec4{x, y, 0, 1.f};
    m_position += glm::vec3(player_position.x, 30, player_position.z);

    m_direction = glm::rotateZ(glm::vec3{1.f, 0.f, 0.f}, glm::radians(m_light_angle_in_deg));
    m_direction = glm::rotateY(m_direction, glm::radians(y_rotation_in_deg));
}

void Light::calculate_view() {
    auto invert_if_should = [&](float v) {
      if (m_is_day) {
          return v;
      } else {
          return -v;
      }
    };

    m_light_view = glm::lookAt(
        m_position,
        glm::vec3(
            m_position.x - invert_if_should(m_direction.x),
            m_position.y - abs(m_direction.y),
            m_position.z - invert_if_should(m_direction.z)
        ),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

void Light::calculate_projection() {
    m_light_projection = glm::ortho(-RENDER_DISTANCE / 2, RENDER_DISTANCE / 2, -RENDER_DISTANCE / 2,
                                    RENDER_DISTANCE / 2, 50.f, 2 * RENDER_DISTANCE);
}

void Light::fix_shadow(glm::mat4 &shadow_matrix) {
    glm::vec4 shadow_origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shadow_origin = shadow_matrix * shadow_origin;
    shadow_origin = shadow_origin * static_cast<float>(SHADOW_SIZE) / 2.f;

    glm::vec4 rounded_origin = glm::round(shadow_origin);
    glm::vec4 round_offset = rounded_origin - shadow_origin;
    round_offset = round_offset * 2.0f / static_cast<float>(SHADOW_SIZE);
    round_offset.z = 0.0f;
    round_offset.w = 0.0f;

    glm::mat4 shadow_proj = m_light_projection;
    shadow_proj[3] += round_offset;
    m_projection_view = shadow_proj * m_light_view;
}

