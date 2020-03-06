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
}

void Player::update(double dtime) {
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

    static double acceleration = 2.5 * m_speed;// DEFAULT_PLAYER_SPEED m_speed;

    auto get_oveclocking_moving = [&](Direction key) -> double {
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
    };

    auto get_braking_moving = [&](Direction key) -> double {
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
    };

    auto get_moving = [&](Direction key) -> double {
      if (m_is_moving[key]) {
          return get_oveclocking_moving(key);
      } else {
          return get_braking_moving(key);
      }
    };

    m_dpos.x = 0.0;
    m_dpos.y = 0.0;
    m_dpos.z = 0.0;

    // forward
    double dmoving = get_moving(Direction::FORWARD);
    m_dpos.x += -sinf(m_camera_angle.x / 180 * PI) * dmoving;
    m_dpos.z += -cosf(m_camera_angle.x / 180 * PI) * dmoving;

    // back
    dmoving = get_moving(Direction::BACK);
    m_dpos.x += +sinf(m_camera_angle.x / 180 * PI) * dmoving;
    m_dpos.z += +cosf(m_camera_angle.x / 180 * PI) * dmoving;

    // left
    dmoving = get_moving(Direction::LEFT);
    m_dpos.x += +sinf((m_camera_angle.x - 90) / 180 * PI) * dmoving;
    m_dpos.z += +cosf((m_camera_angle.x - 90) / 180 * PI) * dmoving;

    //rigth
    dmoving = get_moving(Direction::RIGHT);
    m_dpos.x += +sinf((m_camera_angle.x + 90) / 180 * PI) * dmoving;
    m_dpos.z += +cosf((m_camera_angle.x + 90) / 180 * PI) * dmoving;

    // up(jump)
    if (m_flying) {
        dmoving = get_moving(Direction::UP);

        m_dpos.y += dmoving;
        m_on_ground = false;
    } else if (m_is_moving[Direction::UP]) {
        if (m_is_in_water) {
            m_dpos.y += 3.0 * dtime;
            m_on_ground = false;
            m_direction_speed[Direction::UP] = 1.5;
        } else if (m_on_ground) {
            m_on_ground = false;
            m_direction_speed[Direction::UP] = 9.5;
        }
    }

    // lshift
    dmoving = get_moving(Direction::DOWN);
    if (m_flying) {
        m_dpos.y -= dmoving;
    }

    if (!m_flying) {
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

sf::Vector3i Player::determine_look_at_block(sf::Vector3i *prev_pos /*= nullptr*/) {
    float x = m_pos.x,
        y = m_pos.y + 0.8f,
        z = m_pos.z;

    float prev_x = x,
        prev_y = y,
        prev_z = z;

    auto dist = [&]() -> float {
      return std::sqrt((m_pos.x - x) * (m_pos.x - x) + (m_pos.y - y) * (m_pos.y - y) + (m_pos.z - z) * (m_pos.z - z));
    };
    while (dist() < 6) {
        x += -sinf(m_camera_angle.x / 180 * PI) / 80.F;
        y += tanf(m_camera_angle.y / 180 * PI) / 80.F;
        z += -cosf(m_camera_angle.x / 180 * PI) / 80.F;

        if (!m_map
            ->is_air(Converter::coord2block_coord(x), Converter::coord2block_coord(y), Converter::coord2block_coord(z))
            &&
                !m_map->is_water(Converter::coord2block_coord(x),
                                 Converter::coord2block_coord(y),
                                 Converter::coord2block_coord(z)) &&
            Converter::coord2block_coord(y) >= 0
            && Converter::coord2block_coord(y) < BLOCKS_IN_CHUNK * CHUNKS_IN_WORLD_HEIGHT
            ) {
            if (prev_pos != nullptr) {
                *prev_pos = {
                    Converter::coord2block_coord(prev_x),
                    Converter::coord2block_coord(prev_y),
                    Converter::coord2block_coord(prev_z)
                };
            }

            return {
                Converter::coord2block_coord(x),
                Converter::coord2block_coord(y),
                Converter::coord2block_coord(z)
            };
        }

        prev_x = x;
        prev_y = y;
        prev_z = z;
    }

    return {-1, -1, -1};
}

void Player::put_block() {
    sf::Vector3i prev_pos;
    sf::Vector3i pos = determine_look_at_block(&prev_pos);

    if (pos.y != -1) {
        bool able_create = true;
        // is we in this block
        for (int matrix_x = Converter::coord2block_coord(m_pos.x - m_size.x); matrix_x < m_pos.x + m_size.x;
             matrix_x++) {
            for (int matrix_y = Converter::coord2block_coord(m_pos.y - m_size.y); matrix_y < m_pos.y + m_size.y;
                 matrix_y++) {
                for (int matrix_z = Converter::coord2block_coord(m_pos.z - m_size.z);
                     matrix_z < m_pos.z + m_size.z; matrix_z++) {

                    if (matrix_x == prev_pos.x && matrix_y == prev_pos.y && matrix_z == prev_pos.z) {
                        able_create = false;
                        break;
                    }
                }
            }
        }

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
    sf::Vector3i pos = determine_look_at_block();
    if (pos.y != -1) {
        m_map->delete_block(pos.x, pos.y, pos.z);
    }

}

glm::mat4 Player::calc_projection_view(sf::Vector2u &window_size) {
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat) window_size.x / (GLfloat) window_size.y,
                                            0.1f, RENDER_DISTANCE);
    glm::mat4 view = glm::lookAt(
        glm::vec3(
            get_position().x,
            get_position().y + 0.8f,//m_size.y
            get_position().z
        ),
        glm::vec3(
            get_position().x - sin(m_camera_angle.x / 180.0f * PI),
            get_position().y + 0.8f + tan(m_camera_angle.y / 180.0f * PI),
            get_position().z - cos(m_camera_angle.x / 180.0f * PI)
        ),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return projection * view;
}

void Player::collision(float dx, float dy, float dz) {
    m_is_in_water = false;

    //for  blocks in player's area
    for (int y = Converter::coord2block_coord(m_pos.y - m_size.y); y < m_pos.y + m_size.y; y++) {

        for (int x = Converter::coord2block_coord(m_pos.x - m_size.x); x < m_pos.x + m_size.x; x++) {
            for (int z = Converter::coord2block_coord(m_pos.z - m_size.z); z < m_pos.z + m_size.z; z++) {
                if (m_map->is_solid(x, y, z)) { //if collided with block
                    if (dx > 0) m_pos.x = x - m_size.x;
                    if (dx < 0) m_pos.x = x + 1 + m_size.x;
                    if (dy > 0) m_pos.y = y - m_size.y;
                    if (dy < 0) {
                        m_pos.y = y + 1 + m_size.y;
                        m_on_ground = true;
                        m_direction_speed[Direction::UP] = 0.f;
                    }
                    if (dz > 0) m_pos.z = z - m_size.z;
                    if (dz < 0) m_pos.z = z + 1 + m_size.z;
                }

                if (m_map->is_water(x, y, z)) {
                    m_is_in_water = true;
                }

                m_is_under_water = m_map->is_water(x, y, z);
            }
        }
    }
}

void Player::on_notify(const InputEvent *event) {
    static float start_direction_speed = m_speed / 10;

    switch (event->type) {
        case EventType::MOVEMENT: {
            if (event->is_begin) {
                m_is_moving[event->direction] = true;
                m_direction_speed[event->direction] = start_direction_speed;
            } else if (event->is_end) {
                m_is_moving[event->direction] = false;
            }
            break;
        }
        case EventType::CREATE: put_block();
            break;
        case EventType::REMOVE: delete_block();
            break;
        case EventType::CHANGE_INV_ITEM: {
            static int curr_block_index = 0;
            // todo
            if (curr_block_index + event->delta.x >= (int) m_inventory.size()) {
                m_curr_block = m_inventory[0].first;
                curr_block_index = 0;
            } else if (curr_block_index + event->delta.x < 0) {
                m_curr_block = m_inventory.back().first;
                curr_block_index = m_inventory.size() - 1;
            } else {
                m_curr_block = m_inventory[curr_block_index + event->delta.x].first;
                curr_block_index += event->delta.x;
            }
            m_curr_block = m_inventory[curr_block_index].first;
        }
            break;
        case EventType::CAMERA_ROTATION: {
            m_camera_angle.x += event->delta.x;
            m_camera_angle.y += event->delta.y;
            if (m_camera_angle.y < -89) { m_camera_angle.y = -89; }
            if (m_camera_angle.y > 89) { m_camera_angle.y = 89; }

            if (m_camera_angle.x > 361) {
                m_camera_angle.x -= 360;
            }

            if (m_camera_angle.x < -361) {
                m_camera_angle.x += 360;
            }

            break;
        }
        case EventType::RSHIFT_RELEASED: {
            if (m_god) {
                if (m_flying)
                    flight_off();
                else
                    flight_on();
            }
            break;
        }
    };
}
