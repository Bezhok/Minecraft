#pragma once
#include "pch.h"
#include "Entity.h"
#include "Map.h"
#include "game_constants.h"
#include "block_db.h"

using namespace Base;
using namespace World;

using std::vector;
using std::pair;
class Player :
	public Entity
{
public:
	sf::Vector2f m_camera_angle;
private:
	vector<pair<DB::block_id, int>> m_inventory;
	DB::block_id m_curr_block;
	float m_speed = STANDART_PLAYER_SPEED;

	sf::Vector3f m_dpos;
	bool m_on_ground = false;
	bool m_flying = false;
	bool m_god = false;

public:
	/* set default value */
	void init(World::Map *map);

	/* calculate movement */
	void update(float time);
	void input(sf::Event &e);

	/* eponymous */
	void god_on() { m_god = true; }
	void god_off() { m_god = false; }

	void flight_on();
	void flight_off();

	/* getters */
	const auto& get_inventory() { return m_inventory; };
private:
	void collision(float dx, float dy, float dz);
	void put_block();
	void delete_block();

	void keyboard_input(sf::Event &e);
	void mouse_input(sf::Event &e);
};

