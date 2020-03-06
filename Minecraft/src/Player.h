#pragma once

#include "pch.h"
#include "Entity.h"
#include "game_constants.h"
#include "Direction.h"
#include "Observer.h"

namespace World {
    class Map;

    enum class BlockType : uint8_t;
}

class Player :
    public Base::Entity, public Observer {
 public:
    sf::Vector2f m_camera_angle;
 private:
    std::vector<std::pair<World::BlockType, int>> m_inventory;
    World::BlockType m_curr_block;
    float m_speed = DEFAULT_PLAYER_SPEED;
    sf::Vector3<double> m_dpos;
    bool m_on_ground = false;
    bool m_flying = false;
    bool m_god = false;
    std::unordered_map<Direction, double> m_direction_speed;
    std::unordered_map<Direction, bool> m_is_moving;
 public:
    /* set default value */
    void init(World::Map *map);

    /* calculate movement */
    void update(double time);

    /* eponymous */
    void god_on() { m_god = true; }

    void god_off() { m_god = false; }

    void flight_on();

    void flight_off();

    bool m_is_in_water = false;
    bool m_is_under_water = false;

    /* getters */
    const auto &get_inventory() { return m_inventory; };

    sf::Vector3i determine_look_at_block(sf::Vector3i *prev_pos = nullptr);

    glm::mat4 calc_projection_view(sf::Vector2u &window_size);

    void on_notify(const InputEvent *event) override;
 private:
    void collision(float dx, float dy, float dz);

    void put_block();

    void delete_block();
};

