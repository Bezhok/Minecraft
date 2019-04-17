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
	DB::block_id m_curr_block;
	sf::Vector2f m_camera_angle;
	float m_speed = STANDART_PLAYER_SPEED;
	vector<pair<DB::block_id, int>> m_inventory;

	Player();
	void init(World::Map *world);
	~Player();
	void input(sf::Event &e);
	void update(float time);

	void god_on() { m_god = true; }
	void god_off() { m_god = false; }

private:
	void collision(float dx, float dy, float dz);
	void flight_on();
	void flight_off();
	void put_block();
	void delete_block();

	sf::Vector3f m_dpos;
	bool m_on_ground = false;
	bool m_flying = false;
	bool m_god = false;
	void keyboard_input(sf::Event &e);
	void mouse_input(sf::Event &e);
};

