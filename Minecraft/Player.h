#pragma once
#include "pch.h"
#include "Entity.h"
#include "game_constants.h"

namespace World { 
	class Map;
	enum class block_id :uint8_t;
}

class Player :
	public Base::Entity
{
	enum Key
	{
		W = 0, A, S, D, Space, LShift
	};
public:
	sf::Vector2f m_camera_angle;
private:
	bool m_is_keys_pressed[6] = { false };
	float m_direction_speed[6] = { 0 };

	std::vector<std::pair<World::block_id, int>> m_inventory;
	World::block_id m_curr_block;
	float m_speed = DEFAULT_PLAYER_SPEED;

	sf::Vector3f m_dpos;
	bool m_on_ground = false;
	bool m_flying = false;
	bool m_god = false;

public:
	/* set default value */
	void init(World::Map *map);

	/* calculate movement */
	void update(float time);
	void input(sf::Event& e);

	/* eponymous */
	void god_on() { m_god = true; }
	void god_off() { m_god = false; }

	void flight_on();
	void flight_off();
	bool m_is_in_water = false;
	bool m_is_under_water = false;

	/* getters */
	const auto& get_inventory() { return m_inventory; };

	sf::Vector3i get_block_look_at_pos(sf::Vector3i* prev_pos = nullptr);
private:
	void collision(float dx, float dy, float dz);
	void put_block();
	void delete_block();

	void keyboard_input(sf::Event& e);
	void mouse_input(sf::Event& e);
};

