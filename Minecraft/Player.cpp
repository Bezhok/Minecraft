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
	m_speed = 5*COORDS_IN_BLOCK;
	m_size = { COORDS_IN_BLOCK / 4.F, COORDS_IN_BLOCK, COORDS_IN_BLOCK / 4.F };
	m_pos = { 320, 51+30, 360 };

	for (auto& e : DB::s_side_textures)
		m_inventory.emplace_back(e.first, 1);

	m_curr_block = m_inventory[0].first;
}

void Player::input(sf::Event& e)
{
	keyboard_input(e);
	mouse_input(e);
}

void Player::update(float dtime)
{
	static float acceleration = 5.f*DEFAULT_PLAYER_SPEED;
	
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
	if (m_flying)
		m_dpos.y = 0.f;

	m_dpos.z = 0.f;

	// forward
	float dmoving = get_moving(Key::W)*5;
	m_dpos.x += -sinf(m_camera_angle.x / 180 * PI) * dmoving;
	m_dpos.z += -cosf(m_camera_angle.x / 180 * PI) * dmoving;

	// back
	dmoving = get_moving(Key::S)*5;
	m_dpos.x += +sinf(m_camera_angle.x / 180 * PI) * dmoving;
	m_dpos.z += +cosf(m_camera_angle.x / 180 * PI) * dmoving;
	
	// left
	dmoving = get_moving(Key::A)*5;
	m_dpos.x += +sinf((m_camera_angle.x - 90) / 180 * PI) * dmoving;
	m_dpos.z += +cosf((m_camera_angle.x - 90) / 180 * PI) * dmoving;

	//rigth
	dmoving = get_moving(Key::D)*5;
	m_dpos.x += +sinf((m_camera_angle.x + 90) / 180 * PI) * dmoving;
	m_dpos.z += +cosf((m_camera_angle.x + 90) / 180 * PI) * dmoving;

	// up(jump)
	dmoving = get_moving(Key::Space);
	if (m_is_keys_pressed[Key::Space])
	{
		if (m_flying) {
			m_dpos.y += dmoving*2;
			m_on_ground = false;
		}
		else {
			if (m_on_ground) {
				m_direction_speed[Key::Space] = 2*COORDS_IN_BLOCK;
				m_on_ground = false;
			}
			else {
				m_direction_speed[Key::Space] = 0;
			}
		}
	}

	// lshift
	dmoving = get_moving(Key::LShift);
	if (m_is_keys_pressed[Key::LShift])
	{
		if (m_flying) {
			m_dpos.y -= dmoving*2;
		}
	}


	if (m_flying) {
		m_on_ground = false;

		m_pos.x += m_dpos.x;
		collision(m_dpos.x, 0, 0);

		m_pos.y += m_dpos.y;
		collision(0, m_dpos.y, 0);

		m_pos.z += m_dpos.z;
		collision(0, 0, m_dpos.z);

		m_dpos.x = m_dpos.y = m_dpos.z = 0;
	} else {
		if (!m_on_ground) {
			float s = m_direction_speed[Key::Space];

			m_direction_speed[Key::Space] -= 9.8f * dtime;

			s += m_direction_speed[Key::Space];
			s *= dtime / 2;

			m_dpos.y += s*5;
		}

		m_on_ground = false; //reset

		m_pos.x += m_dpos.x;
		collision(m_dpos.x, 0, 0);

		m_pos.y += m_dpos.y;
		collision(0, m_dpos.y, 0);

		m_pos.z += m_dpos.z;
		collision(0, 0, m_dpos.z);

		m_dpos.x = m_dpos.z = 0;
	}
}

void Player::flight_on()
{
	if (!m_flying) {
		m_flying = true;
		m_speed *= 1.5;
	}
}

void Player::flight_off()
{
	if (m_flying) {
		m_flying = false;
		m_speed = DEFAULT_PLAYER_SPEED;
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
	for (int distance = 0; distance < 50 * COORDS_IN_BLOCK; ++distance) {
		
		x += -sinf(m_camera_angle.x / 180 * PI)/10.F;
		y += tanf(m_camera_angle.y / 180 * PI)/10.F;
		z += -cosf(m_camera_angle.x / 180 * PI)/10.F;

		if (!m_map->is_air(Map::coord2block_coord(x), Map::coord2block_coord(y), Map::coord2block_coord(z))) {
			// is we in this block
			for (int matrix_x = Map::coord2block_coord(m_pos.x - m_size.x); matrix_x <(m_pos.x + m_size.x) / COORDS_IN_BLOCK; matrix_x++) {
				for (int matrix_y = Map::coord2block_coord(m_pos.y - m_size.y); matrix_y < (m_pos.y + m_size.y) / COORDS_IN_BLOCK; matrix_y++) {
					for (int matrix_z = Map::coord2block_coord(m_pos.z - m_size.z); matrix_z < (m_pos.z + m_size.z) / COORDS_IN_BLOCK; matrix_z++) {
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
		y = m_pos.y+ m_size.y*0.8f,
		z = m_pos.z;

	bool able_create = false;
	for (int distance = 0; distance < 50 * COORDS_IN_BLOCK; ++distance) {
		x += -sinf(m_camera_angle.x / 180 * PI)/10.F;
		y += tanf(m_camera_angle.y / 180 * PI)/10.F;
		z += -cosf(m_camera_angle.x / 180 * PI)/10.F;
		if (!m_map->is_air(
			Map::coord2block_coord(x),
			Map::coord2block_coord(y),
			Map::coord2block_coord(z)
		)) {
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
	//for  blocks in player's area
	for (int x = Map::coord2block_coord(m_pos.x - m_size.x); x < (m_pos.x + m_size.x) / COORDS_IN_BLOCK; x++) {
		for (int y = Map::coord2block_coord(m_pos.y - m_size.y); y < (m_pos.y + m_size.y) / COORDS_IN_BLOCK; y++) {
			for (int z = Map::coord2block_coord(m_pos.z - m_size.z); z < (m_pos.z + m_size.z) / COORDS_IN_BLOCK; z++) {
				if (m_map->is_solid(x, y, z)) { //if collided with block
					if (dx > 0)  m_pos.x = x * COORDS_IN_BLOCK - m_size.x;
					if (dx < 0)  m_pos.x = x * COORDS_IN_BLOCK + COORDS_IN_BLOCK + m_size.x;
					if (dy > 0)  m_pos.y = y * COORDS_IN_BLOCK - m_size.y;
					if (dy < 0) {
						m_pos.y = y * COORDS_IN_BLOCK + COORDS_IN_BLOCK + m_size.y;
						m_on_ground = true;
						m_dpos.y = 0;
					}
					if (dz > 0)  m_pos.z = z * COORDS_IN_BLOCK  - m_size.z;
					if (dz < 0)  m_pos.z = z * COORDS_IN_BLOCK + COORDS_IN_BLOCK  + m_size.z;
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
		if (e.key.code == sf::Mouse::Left) {
			put_block();
		} else if (e.key.code == sf::Mouse::Right) {
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
