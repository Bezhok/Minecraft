#pragma once
#include "pch.h"
#include "game_constants.h"
#include "Direction.h"

namespace Base {
    class Entity;
}
class Movement {
 private:
    float m_speed = DEFAULT_PLAYER_SPEED;
    sf::Vector3<double> m_dpos;
    bool m_flying = false;
    double acceleration;
    std::unordered_map<Direction, double> m_direction_speed;
    std::unordered_map<Direction, bool> m_is_moving;
    Base::Entity *m_entity;
 private:
    double get_overclocking_moving(Direction key, float dtime);
    double get_braking_moving(Direction key, float dtime);
    double get_moving(Direction key, float dtime);
    void process_flight(float dtime);
 public:
    void set_dpos(sf::Vector3<double> dpos);
    const sf::Vector3<double> &get_dpos() const;
    float get_speed() const;
    void set_speed(float speed);
    void set_flying(bool flying);
    bool is_flying() const;

    void reset_speedY();
    void init(Base::Entity *entity);
    void process_movingXZ(float dtime, float angleX);
    void process_movingY(float dtime);
    void switch_movement_state(Direction direction, bool is_begin);
};
