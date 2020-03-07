#include "pch.h"
#include "Player.h"
#include "game_constants.h"
#include "Converter.h"
#include "Map.h"
#include "block_db.h"
#include "InputEvent.h"

using namespace World;

void Player::init(Map *map) {
    m_map = map;
    m_speed = DEFAULT_PLAYER_SPEED;
    m_size = {1.f / 4.F, 1.f, 1.f / 4.F};
    m_pos = {5450, 80, 1256};
    m_pos = {5356, 80, 1495};

    for (BlockType type : DB::s_loaded_blocks)
        m_inventory.emplace_back(type, 1);

    m_curr_block = m_inventory[0].first;

    const Direction buffer[] =
        {Direction::RIGHT, Direction::LEFT, Direction::BACK, Direction::FORWARD, Direction::UP, Direction::DOWN};

    for (const auto &item : buffer) {
        m_is_moving[item] = false;
        m_direction_speed[item] = 0.0;
    }

    m_cam.init(&m_pos, m_map);

    acceleration = 2.5 * m_speed;// DEFAULT_PLAYER_SPEED m_speed;
}

void Player::control_world_border() {
    if (m_pos.y < -10) {
        m_pos.y = -9;
        //death
    } else if (m_pos.x < 10) {
        m_pos.x = 11;
    } else if (m_pos.z < 10) {
        m_pos.z = 11;
    } else if (m_pos.y > BLOCKS_IN_CHUNK * CHUNKS_IN_WORLD_HEIGHT + 5) {
        m_pos.y = BLOCKS_IN_CHUNK * CHUNKS_IN_WORLD_HEIGHT + 4;
    }
}

double Player::get_overclocking_moving(Direction key, float dtime) {
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

double Player::get_braking_moving(Direction key, float dtime) {
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

double Player::get_moving(Direction key, float dtime) {
    if (m_is_moving[key]) {
        return get_overclocking_moving(key, dtime);
    } else {
        return get_braking_moving(key, dtime);
    }
}

void Player::process_flight(double dtime) {
    double dmoving = 0.0;

    dmoving = get_moving(Direction::UP, dtime);

    m_dpos.y += dmoving;
    m_on_ground = false;

    // lshift
    dmoving = get_moving(Direction::DOWN, dtime);

    m_dpos.y -= dmoving;

}

void Player::process_movingY(double dtime) {
    // up(jump)
    if (m_flying) {
        process_flight(dtime);
    } else {
        if (m_is_moving[Direction::UP]) {
            if (m_is_in_water) {
                m_dpos.y += 3.0 * dtime;
                m_on_ground = false;
                m_direction_speed[Direction::UP] = 1.5;
            } else if (m_on_ground) {
                m_on_ground = false;
                m_direction_speed[Direction::UP] = 9.5;
            }
        }

        if (!m_on_ground) {
            double s = m_direction_speed[Direction::UP];

            if (s > -80.0) {
                if (m_is_in_water) {
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

    m_on_ground = false; //reset
}

void Player::process_movingXZ(double dtime) {
    float angleX = m_cam.m_angle.x;
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

void Player::update(double dtime) {
    control_world_border();

    m_dpos = sf::Vector3<double>();

    process_movingXZ(dtime);
    process_movingY(dtime);

    m_pos.y += m_dpos.y;
    collision(0, m_dpos.y, 0);

    m_pos.x += m_dpos.x;
    collision(m_dpos.x, 0, 0);

    m_pos.z += m_dpos.z;
    collision(0, 0, m_dpos.z);
}

void Player::flight_on() {
    if (!m_flying) {
        m_flying = true;
        m_speed = 2.5f * DEFAULT_PLAYER_SPEED;
        m_direction_speed[Direction::UP] = 0;
    }
}

void Player::flight_off() {
    if (m_flying) {
        m_flying = false;
        m_speed = DEFAULT_PLAYER_SPEED;
        m_direction_speed[Direction::UP] = 0;
    }
}

void Player::iterate_throw_near_blocks(std::function<bool(int x, int y, int z)> &&fun) {
    auto start = Converter::coord2block_coord(m_pos - m_size);
    auto end = m_pos + m_size;
    for (int x = start.x; x < end.x; x++) {
        for (int y = start.y; y < end.y; y++) {
            for (int z = start.z; z < end.z; z++) {
                // bool - should continue
                if (!fun(x, y, z)) return;
            }
        }
    }
}

void Player::put_block() {
    sf::Vector3i prev_pos;
    sf::Vector3i pos = m_cam.determine_look_at_block(&prev_pos);

    if (pos.y != -1) {
        bool able_create = true;
        // is we in this block
        iterate_throw_near_blocks([&](int x, int y, int z) -> bool {
          if (x == prev_pos.x && y == prev_pos.y && z == prev_pos.z) {
              able_create = false;
              return false;
          }
        });

        if (able_create) {
            m_map->create_block(
                prev_pos.x,
                prev_pos.y,
                prev_pos.z,
                m_curr_block
            );
            return;
        }
    }
}

void Player::delete_block() {
    sf::Vector3i pos = m_cam.determine_look_at_block();
    if (pos.y != -1) {
        m_map->delete_block(pos.x, pos.y, pos.z);
    }
}

void Player::push_out(sf::Vector3i from, sf::Vector3f d) {
    if (d.x > 0) m_pos.x = from.x - m_size.x;
    if (d.x < 0) m_pos.x = from.x + 1 + m_size.x;
    if (d.y > 0) m_pos.y = from.y - m_size.y;
    if (d.y < 0) {
        m_pos.y = from.y + 1 + m_size.y;
        m_on_ground = true;
        m_direction_speed[Direction::UP] = 0.f;
    }
    if (d.z > 0) m_pos.z = from.z - m_size.z;
    if (d.z < 0) m_pos.z = from.z + 1 + m_size.z;
}

void Player::collision(float dx, float dy, float dz) {
    m_is_in_water = false;
    //for  blocks in player's area
    iterate_throw_near_blocks([&](int x, int y, int z) -> bool {
      if (m_map->is_solid(x, y, z)) { //if collided with block
          push_out({x, y, z}, {dx, dy, dz});
      }
      if (m_map->is_water(x, y, z)) {
          m_is_in_water = true;
      }
      m_is_under_water = m_map->is_water(x, y, z);

      return true;
    });
}

void Player::on_notify(const InputEvent *event) {
    switch (event->type) {
        case EventType::MOVEMENT:switch_movement_state(event->direction, event->is_begin);
            break;
        case EventType::CREATE: put_block();
            break;
        case EventType::REMOVE: delete_block();
            break;
        case EventType::CHANGE_INV_ITEM:change_item(event->delta.x);
            break;
        case EventType::CAMERA_ROTATION:m_cam.rotate(event->delta);
            break;
        case EventType::RSHIFT_RELEASED:switch_flight_state();
            break;
    };
}

void Player::switch_movement_state(Direction direction, bool is_begin) {
    static float start_direction_speed = m_speed / 10;
    if (is_begin) {
        m_is_moving[direction] = true;
        m_direction_speed[direction] = start_direction_speed;
    } else {
        m_is_moving[direction] = false;
    }
}

void Player::switch_flight_state() {
    if (m_god) {
        if (m_flying) {
            flight_off();
        } else {
            flight_on();
        }
    }
}

void Player::change_item(int delta) {
    static int curr_block_index = 0;
    // todo
    if (curr_block_index + delta >= (int) m_inventory.size()) {
        m_curr_block = m_inventory[0].first;
        curr_block_index = 0;
    } else if (curr_block_index + delta < 0) {
        m_curr_block = m_inventory.back().first;
        curr_block_index = m_inventory.size() - 1;
    } else {
        m_curr_block = m_inventory[curr_block_index + delta].first;
        curr_block_index += delta;
    }
    m_curr_block = m_inventory[curr_block_index].first;
}

const Camera &Player::get_cam() {
    return m_cam;
}
