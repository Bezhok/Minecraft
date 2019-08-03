#include "pch.h"
#include "Player.h"
#include "game_constants.h"
#include "Map.h"
#include "block_db.h"

using std::cosf;
using std::sinf;
using std::tanf;
using namespace World;


void Player::init(Map *map)
{
	m_map = map;
	m_speed = DEFAULT_PLAYER_SPEED;
	m_size = { 1.f / 4.F, 1.f, 1.f / 4.F };
	//m_pos = { 320, 51+30, 360 };
	m_pos = { 5356, 80, 1495 };

	for (block_id type : DB::s_loaded_blocks)
		m_inventory.emplace_back(type, 1);

	m_curr_block = m_inventory[0].first;
}

void Player::input(sf::Event& e)
{
	keyboard_input(e);
	mouse_input(e);
}

void Player::update(float dtime)
{
	static float acceleration = 2.f*m_speed;// DEFAULT_PLAYER_SPEED m_speed;
	
	auto get_oveclocking_moving = [&](Key key) -> float {
		float dmoving = m_speed*dtime;

		if (m_direction_speed[key] < m_speed) {
			dmoving = m_direction_speed[key];

			m_direction_speed[key] += acceleration * dtime;

			dmoving += m_direction_speed[key];
			dmoving *= dtime / 2;
		}
		else {
			m_direction_speed[key] = m_speed;
		}

		return dmoving;
	};

	auto get_braking_moving = [&](Key key) -> float {
		float dmoving = 0.f;
		if (m_direction_speed[key] > 0) {
			dmoving = m_direction_speed[key];

			m_direction_speed[key] -= acceleration * dtime;

			dmoving += m_direction_speed[key];
			dmoving *= dtime / 2;
		}
		else {
			m_direction_speed[key] = 0.f;
		}

		return dmoving;
	};

	auto get_moving = [&](Key key) -> float {
		if (m_is_keys_pressed[key])
		{
			return get_oveclocking_moving(key);
		}
		else {
			return get_braking_moving(key);
		}
	};

	m_dpos.x = 0.f;
	m_dpos.y = 0.f;
	m_dpos.z = 0.f;

	// forward
	float dmoving = get_moving(Key::W)*1.5f;
	m_dpos.x += -sinf(m_camera_angle.x / 180 * PI) * dmoving;
	m_dpos.z += -cosf(m_camera_angle.x / 180 * PI) * dmoving;

	// back
	dmoving = get_moving(Key::S)*1.5f;
	m_dpos.x += +sinf(m_camera_angle.x / 180 * PI) * dmoving;
	m_dpos.z += +cosf(m_camera_angle.x / 180 * PI) * dmoving;
	
	// left
	dmoving = get_moving(Key::A)*1.5f;
	m_dpos.x += +sinf((m_camera_angle.x - 90) / 180 * PI) * dmoving;
	m_dpos.z += +cosf((m_camera_angle.x - 90) / 180 * PI) * dmoving;

	//rigth
	dmoving = get_moving(Key::D)*1.5f;
	m_dpos.x += +sinf((m_camera_angle.x + 90) / 180 * PI) * dmoving;
	m_dpos.z += +cosf((m_camera_angle.x + 90) / 180 * PI) * dmoving;

	// up(jump)
	if (m_is_keys_pressed[Key::Space])
	{
		if (m_flying) {
			dmoving = get_moving(Key::Space);

			m_dpos.y += dmoving;
			m_on_ground = false;
		}
		else {
			if (m_is_in_water) {
				m_dpos.y += 3.f *dtime;
				m_on_ground = false;
				m_direction_speed[Key::Space] = 2.5f;
			} else if (m_on_ground) {
				m_on_ground = false;
				m_direction_speed[Key::Space] = 3.f;
			}
		}
	}

	// lshift
	dmoving = get_moving(Key::LShift);
	if (m_is_keys_pressed[Key::LShift])
	{
		if (m_flying) {
			m_dpos.y -= dmoving;
		}
	}


	if (!m_flying) {
		if (!m_on_ground) {
			float s = m_direction_speed[Key::Space];

			if (s < 50.f) {


				if (m_is_in_water) {
					if (!m_is_keys_pressed[Key::Space]) {
						m_dpos.y -= 3.f*dtime;
					}
				}
				else {
					m_direction_speed[Key::Space] -= 9.8f * dtime;

					s += m_direction_speed[Key::Space];
					s *= dtime / 2.f;
					m_dpos.y += s*3;
				}
			}
			else {
				m_direction_speed[Key::Space] = 50.f;
				m_dpos.y += m_direction_speed[Key::Space];
			}

		}
	}
	m_on_ground = false; //reset

	m_pos.x += m_dpos.x;
	collision(m_dpos.x, 0, 0);

	m_pos.y += m_dpos.y;
	collision(0, m_dpos.y, 0);

	m_pos.z += m_dpos.z;
	collision(0, 0, m_dpos.z);
}

void Player::flight_on()
{
	if (!m_flying) {
		m_flying = true;
		m_speed = 2.5f*DEFAULT_PLAYER_SPEED;
		m_direction_speed[Key::Space] = 0;
	}
}

void Player::flight_off()
{
	if (m_flying) {
		m_flying = false;
		m_speed = DEFAULT_PLAYER_SPEED;
		m_direction_speed[Key::Space] = 0;
	}
}

void Player::put_block()
{
	float prev_x, prev_y, prev_z;
	float x = m_pos.x,
		y = m_pos.y+ m_size.y*0.8f,
		z = m_pos.z;

	prev_x = x; prev_y = y; prev_z = z;
	bool able_create = false;

	auto dist = [&]()->float { return std::sqrt((m_pos.x - x)*(m_pos.x - x) + (m_pos.y - y)*(m_pos.y - y) + (m_pos.y - y)*(m_pos.y - y)); };
	while (dist() < 6.f) {
		
		x += -sinf(m_camera_angle.x / 180 * PI)/20.F;
		y += tanf(m_camera_angle.y / 180 * PI)/20.F;
		z += -cosf(m_camera_angle.x / 180 * PI)/20.F;

		if (!m_map->is_air(Map::coord2block_coord(x), Map::coord2block_coord(y), Map::coord2block_coord(z)) &&
			!m_map->is_water(Map::coord2block_coord(x), Map::coord2block_coord(y), Map::coord2block_coord(z))
			) {
			// is we in this block
			for (int matrix_x = Map::coord2block_coord(m_pos.x - m_size.x); matrix_x < m_pos.x + m_size.x; matrix_x++) {
				for (int matrix_y = Map::coord2block_coord(m_pos.y - m_size.y); matrix_y < m_pos.y + m_size.y; matrix_y++) {
					for (int matrix_z = Map::coord2block_coord(m_pos.z - m_size.z); matrix_z < m_pos.z + m_size.z; matrix_z++) {
						int x = Map::coord2block_coord(prev_x);
						int y = Map::coord2block_coord(prev_y);
						int z = Map::coord2block_coord(prev_z);
						if (matrix_x == x && matrix_y == y && matrix_z == z) {
							able_create = false;
							break;
						}
					}
				}
			}

			if (able_create) {
				m_map->create_block(
					Map::coord2block_coord(prev_x),
					Map::coord2block_coord(prev_y),
					Map::coord2block_coord(prev_z),
					m_curr_block
				);
				break;
			}
		}

		prev_x = x; prev_y = y; prev_z = z;
		able_create = true;
	}
}

void Player::delete_block()
{
	float x = m_pos.x,
		y = m_pos.y + 0.8f,
		z = m_pos.z;

	bool able_create = false;

	auto dist = [&]()->float { return std::sqrt((m_pos.x - x)*(m_pos.x - x) + (m_pos.y - y)*(m_pos.y - y) + (m_pos.y - y)*(m_pos.y - y)); };
	while (dist() < 6) {
		x += -sinf(m_camera_angle.x / 180 * PI)/20.F;
		y += tanf(m_camera_angle.y / 180 * PI)/20.F;
		z += -cosf(m_camera_angle.x / 180 * PI)/20.F;
		if (!m_map->is_air(
			Map::coord2block_coord(x),
			Map::coord2block_coord(y),
			Map::coord2block_coord(z)) &&

			!m_map->is_water(
			Map::coord2block_coord(x),
			Map::coord2block_coord(y),
			Map::coord2block_coord(z))
			) {
			m_map->delete_block(
				Map::coord2block_coord(x),
				Map::coord2block_coord(y),
				Map::coord2block_coord(z)
			);
			break;
		}
	}
}

void Player::collision(float dx, float dy, float dz)
{
	m_is_in_water = false;

	//for  blocks in player's area
	for (int y = Map::coord2block_coord(m_pos.y - m_size.y); y < m_pos.y + m_size.y; y++) {

		for (int x = Map::coord2block_coord(m_pos.x - m_size.x); x < m_pos.x + m_size.x; x++) {
			for (int z = Map::coord2block_coord(m_pos.z - m_size.z); z < m_pos.z + m_size.z; z++) {
				if (m_map->is_solid(x, y, z)) { //if collided with block
					if (dx > 0)  m_pos.x = x - m_size.x;
					if (dx < 0)  m_pos.x = x + 1 + m_size.x;
					if (dy > 0)  m_pos.y = y - m_size.y;
					if (dy < 0) {
						m_pos.y = y + 1 + m_size.y;
						m_on_ground = true;
						m_direction_speed[Key::Space] = 0.f;
					}
					if (dz > 0)  m_pos.z = z - m_size.z;
					if (dz < 0)  m_pos.z = z + 1  + m_size.z;
				}

				if (m_map->is_water(x, y, z)) {
					m_is_in_water = true;
				}

				if (m_map->is_water(x, y, z)) {
					m_is_under_water = true;
				}
				else {
					m_is_under_water = false;
				}
			}
		}
	}
}


void Player::keyboard_input(sf::Event& e)
{
	static float start_direction_speed = m_speed/10;
	switch (e.type)
	{
	case sf::Event::KeyPressed: {
		switch (e.key.code)
		{
		case sf::Keyboard::W:
			m_is_keys_pressed[Key::W] = true;
			m_direction_speed[Key::W] = start_direction_speed;
			break;
		case sf::Keyboard::A:
			m_is_keys_pressed[Key::A] = true;
			m_direction_speed[Key::A] = start_direction_speed;
			break;
		case sf::Keyboard::S:
			m_is_keys_pressed[Key::S] = true;
			m_direction_speed[Key::S] = start_direction_speed;
			break;
		case sf::Keyboard::D:
			m_is_keys_pressed[Key::D] = true;
			m_direction_speed[Key::D] = start_direction_speed;
			break;
		case sf::Keyboard::Space:
			m_is_keys_pressed[Key::Space] = true;
			m_direction_speed[Key::Space] = start_direction_speed;

			break;
		case sf::Keyboard::LShift:
			m_is_keys_pressed[Key::LShift] = true;
			m_direction_speed[Key::LShift] = start_direction_speed;
			break;
		default:
			break;
		}
		break;
	}
	case sf::Event::KeyReleased: {
		switch (e.key.code)
		{
		case sf::Keyboard::W:
			m_is_keys_pressed[Key::W] = false;
			break;
		case sf::Keyboard::A:
			m_is_keys_pressed[Key::A] = false;
			break;
		case sf::Keyboard::S:
			m_is_keys_pressed[Key::S] = false;
			break;
		case sf::Keyboard::D:
			m_is_keys_pressed[Key::D] = false;
			break;
		case sf::Keyboard::Space:
			m_is_keys_pressed[Key::Space] = false;
			break;
		case sf::Keyboard::LShift:
			m_is_keys_pressed[Key::LShift] = false;
			break;
		default:
			break;
		}
		break;
	}
	}

	// rshift
	if (e.type == sf::Event::KeyReleased)
	{
		if (e.key.code == sf::Keyboard::RShift) {
			if (m_god) {
				if (m_flying)
					flight_off();
				else
					flight_on();
			}
		}
	}
}

void Player::mouse_input(sf::Event& e)
{
	static int curr_block_index = 0;

	if (e.type == sf::Event::MouseButtonReleased) {
		if (e.key.code == sf::Mouse::Right) {
			put_block();
		} else if (e.key.code == sf::Mouse::Left) {
			delete_block();
		}
	} else if (e.type == sf::Event::MouseWheelMoved) {
		// todo
		if (curr_block_index + e.mouseWheel.delta >= (int)m_inventory.size()) { 
			m_curr_block = m_inventory[0].first;
			curr_block_index = 0;
		}
		else if (curr_block_index + e.mouseWheel.delta < 0) {
			m_curr_block = m_inventory.back().first;
			curr_block_index = m_inventory.size()-1;
		}
		else { 
			m_curr_block = m_inventory[curr_block_index + e.mouseWheel.delta].first;
			curr_block_index += e.mouseWheel.delta;
		}
	}
}
