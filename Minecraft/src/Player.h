#pragma once

#include "pch.h"
#include "Entity.h"
#include "game_constants.h"
#include "Direction.h"
#include "Observer.h"
#include "Camera.h"
#include "Collider.h"
#include "Movement.h"
#include "Inventory.h"

namespace World {
    class Map;
    enum class BlockType : uint8_t;
}

class Player :
        public Base::Entity, public Observer {
public:
    const Camera &get_cam();

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
    Inventory &get_inventory() { return m_inventory; };

    bool is_on_ground() const override;

    void set_is_on_ground(bool is_on_ground) override;

    bool is_in_water() const override;

    void on_notify(const InputEvent *event) override;

private:
    void put_block();

    void delete_block();

    void collision(float dx, float dy, float dz);

    void control_world_border();

    void switch_flight_state();

    bool m_god = false;
    Camera m_cam;
    Collider m_collider;
    Movement m_movement;
    Inventory m_inventory;
};

