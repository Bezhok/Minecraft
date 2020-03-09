#include "Movement.h"
#include "Entity.h"

using Base::Entity;

double Movement::get_overclocking_moving(Direction key, float dtime) {
    double dmoving = m_speed * dtime;

    if (m_direction_speed[key] < m_speed) {
        dmoving = m_direction_speed[key];

        m_direction_speed[key] += acceleration * dtime;

        dmoving += m_direction_speed[key];
        dmoving *= dtime / 2;
    } else {
        m_direction_speed[key] = m_speed + 0.001;
    }

    return dmoving;
}

double Movement::get_braking_moving(Direction key, float dtime) {
    double dmoving = 0.f;
    if (m_direction_speed[key] > 0) {
        dmoving = m_direction_speed[key];

        m_direction_speed[key] -= acceleration * dtime;

        dmoving += m_direction_speed[key];
        dmoving *= dtime / 2;
    } else {
        m_direction_speed[key] = 0.0;
    }

    return dmoving;
}

double Movement::get_moving(Direction key, float dtime) {
    if (m_is_moving[key]) {
        return get_overclocking_moving(key, dtime);
    } else {
        return get_braking_moving(key, dtime);
    }
}

void Movement::process_movingXZ(float dtime, float angleX) {
    // forward
    double dmoving = get_moving(Direction::FORWARD, dtime);
    float temp = glm::radians(angleX);
    m_dpos.x += -sinf(temp) * dmoving;
    m_dpos.z += -cosf(temp) * dmoving;

    // back
    dmoving = get_moving(Direction::BACK, dtime);
    m_dpos.x += +sinf(temp) * dmoving;
    m_dpos.z += +cosf(temp) * dmoving;

    // left
    temp = glm::radians(angleX - 90);
    dmoving = get_moving(Direction::LEFT, dtime);
    m_dpos.x += +sinf(temp) * dmoving;
    m_dpos.z += +cosf(temp) * dmoving;

    //rigth
    temp = glm::radians(angleX + 90);
    dmoving = get_moving(Direction::RIGHT, dtime);
    m_dpos.x += +sinf(temp) * dmoving;
    m_dpos.z += +cosf(temp) * dmoving;
}

void Movement::process_flight(float dtime) {
    double dmoving;

    dmoving = get_moving(Direction::UP, dtime);
    m_entity->set_is_on_ground(false);
    m_dpos.y += dmoving;

    // lshift
    dmoving = get_moving(Direction::DOWN, dtime);
    m_dpos.y -= dmoving;
}

void Movement::process_movingY(float dtime) {
    // up(jump)
    if (m_flying) {
        process_flight(dtime);
    } else {
        if (m_is_moving[Direction::UP]) {
            if (m_entity->is_in_water()) {
                m_dpos.y += 3.0 * dtime;
                m_entity->set_is_on_ground(false);
                m_direction_speed[Direction::UP] = 1.5;
            } else if (m_entity->is_on_ground()) {
                m_entity->set_is_on_ground(false);
                m_direction_speed[Direction::UP] = 9.5;
            }
        }

        if (!m_entity->is_on_ground()) {
            double s = m_direction_speed[Direction::UP];
            if (s > -80.0) {
                if (m_entity->is_in_water()) {
                    if (!m_is_moving[Direction::UP]) {
                        m_dpos.y -= 3.f * dtime;
                    }
                } else {
                    static double a = 3 * 9.8;
                    s = m_direction_speed[Direction::UP] * dtime - a * dtime * dtime / 2.0;
                    m_dpos.y += s;

                    m_direction_speed[Direction::UP] -= a * dtime;
                }
            } else {
                m_direction_speed[Direction::UP] = -81.0;
                m_dpos.y += m_direction_speed[Direction::UP] * dtime;
            }

        }
    }
    m_entity->set_is_on_ground(false);//reset
}

void Movement::switch_movement_state(Direction direction, bool is_begin) {
    static float start_direction_speed = m_speed / 10;
    if (is_begin) {
        m_is_moving[direction] = true;

        if (direction != Direction::UP)
            m_direction_speed[direction] = start_direction_speed;
    } else {
        m_is_moving[direction] = false;
    }
}

void Movement::init(Entity *entity) {
    m_entity = entity;
    m_speed = DEFAULT_PLAYER_SPEED;

    acceleration = 2.5 * m_speed;

    const Direction buffer[] =
            {Direction::RIGHT, Direction::LEFT, Direction::BACK, Direction::FORWARD, Direction::UP, Direction::DOWN};

    for (const auto &item : buffer) {
        m_is_moving[item] = false;
        m_direction_speed[item] = 0.0;
    }
}

const sf::Vector3<double> &Movement::get_dpos() const {
    return m_dpos;
}

void Movement::set_dpos(sf::Vector3<double> dpos) {
    m_dpos = dpos;
}

bool Movement::is_flying() const {
    return m_flying;
}

float Movement::get_speed() const {
    return m_speed;
}

void Movement::set_speed(float speed) {
    m_speed = speed;
    acceleration = 2.5 * m_speed;
}

void Movement::set_flying(bool flying) {
    m_flying = flying;
}

void Movement::reset_speedY() {
    m_direction_speed[Direction::UP] = 0;
}
