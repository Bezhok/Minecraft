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
    m_size = {1.f / 4.F, 1.f, 1.f / 4.F};
    m_pos = {5450, 80, 1256};
    m_pos = {5356, 80, 1495};

    m_movement.init(this);
    m_cam.init(&m_pos, m_map);
    m_cam.set_shift({0, 0.8f*m_size.y, 0});
    
    m_collider.init(&m_pos, &m_size);
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

void Player::update(double dtime) {
    control_world_border();

    auto dpos = sf::Vector3<double>();
    m_movement.set_dpos(dpos);
    m_movement.process_movingXZ(dtime, m_cam.m_angle.x);
    m_movement.process_movingY(dtime);
    dpos = m_movement.get_dpos();

    m_pos.y += dpos.y;
    collision(0, dpos.y, 0);
    if (dpos.y < 0 && is_on_ground()) {
        m_movement.reset_speedY();
    }

    m_pos.x += dpos.x;
    collision(dpos.x, 0, 0);

    m_pos.z += dpos.z;
    collision(0, 0, dpos.z);
}

void Player::flight_on() {
    if (!m_movement.is_flying()) {
        m_movement.set_flying(true);
        m_movement.set_speed(2.5f * DEFAULT_PLAYER_SPEED);
        m_movement.reset_speedY();
    }
}

void Player::flight_off() {
    if (m_movement.is_flying()) {
        m_movement.set_flying(false);
        m_movement.set_speed(DEFAULT_PLAYER_SPEED);

        m_movement.reset_speedY();
    }
}

void Player::put_block() {
    sf::Vector3i prev_pos;
    sf::Vector3i pos = m_cam.determine_look_at_block(&prev_pos);

    if (pos.y != -1) {
        bool able_create = true;
        // is we in this block
        m_collider.iterate_throw_near_blocks([&](int x, int y, int z) -> bool {
          if (x == prev_pos.x && y == prev_pos.y && z == prev_pos.z) {
              able_create = false;
              return false;
          }
          return true;
        });

        if (able_create) {
            m_map->create_block(
                prev_pos.x,
                prev_pos.y,
                prev_pos.z,
                m_inventory.get_curr_block()
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

void Player::collision(float dx, float dy, float dz) {
    m_is_in_water = false;
    //for  blocks in player's area
    m_collider.iterate_throw_near_blocks([&](int x, int y, int z) -> bool {
      if (m_map->is_solid(x, y, z)) { //if collided with block
          m_collider.push_out({x, y, z}, {dx, dy, dz});
      }
      if (m_map->is_water(x, y, z)) {
          m_is_in_water = true;
      }
      m_is_under_water = m_map->is_water(x, y + 1, z);

      return true;
    });
}

void Player::on_notify(const InputEvent *event) {
    switch (event->type) {
        case EventType::MOVEMENT: m_movement.switch_movement_state(event->direction, event->is_begin);
            break;
        case EventType::CREATE: put_block();
            break;
        case EventType::REMOVE: delete_block();
            break;
        case EventType::CHANGE_INV_ITEM: m_inventory.change_curr_hot_bar_item(event->delta.x);
            break;
        case EventType::CAMERA_ROTATION: m_cam.rotate(event->delta);
            break;
        case EventType::RSHIFT_RELEASED: switch_flight_state();
            break;
    };
}

void Player::switch_flight_state() {
    if (m_god) {
        if (m_movement.is_flying()) {
            flight_off();
        } else {
            flight_on();
        }
    }
}

const Camera &Player::get_cam() {
    return m_cam;
}
bool Player::is_on_ground() const {
    return m_collider.is_on_ground();
}
void Player::set_is_on_ground(bool is_on_ground) {
    m_collider.set_is_on_ground(is_on_ground);
}
bool Player::is_in_water() const {
    return m_is_in_water;
}
