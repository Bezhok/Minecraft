#pragma once

#include "pch.h"
#include "Entity.h"
#include "game_constants.h"
#include "Direction.h"
#include "Observer.h"
#include "Camera.h"

namespace World {
    class Map;

    enum class BlockType : uint8_t;
}

class Player :
    public Base::Entity, public Observer {
 private:
    double acceleration;
    std::vector<std::pair<World::BlockType, int>> m_inventory;
    World::BlockType m_curr_block;
    float m_speed = DEFAULT_PLAYER_SPEED;
    sf::Vector3<double> m_dpos;
    bool m_on_ground = false;
    bool m_flying = false;
    bool m_god = false;
    std::unordered_map<Direction, double> m_direction_speed;
    std::unordered_map<Direction, bool> m_is_moving;

    Camera m_cam;
 public:
    const Camera &get_cam();
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

    void on_notify(const InputEvent *event) override;
 private:
    void collision(float dx, float dy, float dz);

    void put_block();

    void delete_block();
    void change_item(int delta);
    void switch_flight_state();
    void switch_movement_state(Direction direction, bool is_begin);
    void control_world_border();
    void iterate_throw_near_blocks(std::function<bool(int, int, int)> &&fun);
    double get_overclocking_moving(Direction key, float dtime);
    double get_braking_moving(Direction key, float dtime);
    double get_moving(Direction key, float dtime);
    void push_out(sf::Vector3i from, sf::Vector3f d);
    void process_movingXZ(double dtime);
    void process_movingY(double dtime);
    void process_flight(double dtime);
};

