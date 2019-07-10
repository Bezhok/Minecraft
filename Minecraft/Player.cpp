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

	m_size = { COORDS_IN_BLOCK / 4.F, COORDS_IN_BLOCK, COORDS_IN_BLOCK / 4.F };
	m_pos = { 60 * COORDS_IN_BLOCK + 160, 155*COORDS_IN_BLOCK, 15 * COORDS_IN_BLOCK+ 160 };

	for (auto& e : DB::s_side_textures)
		m_inventory.push_back({ e.first, 1 });

	m_curr_block = m_inventory[0].first;
}

void Player::input(sf::Event& e)
{
	keyboard_input(e);
	mouse_input(e);
}

void Player::update(float time)
{
	// forward
	if (m_is_keys_pressed[Key::W])
	{
		m_dpos.x = -sinf(m_camera_angle.x / 180 * PI) * m_speed;
		m_dpos.z = -cosf(m_camera_angle.x / 180 * PI) * m_speed;
	}

	// back
	if (m_is_keys_pressed[Key::S])
	{
		m_dpos.x = +sinf(m_camera_angle.x / 180 * PI) * m_speed;
		m_dpos.z = +cosf(m_camera_angle.x / 180 * PI) * m_speed;
	}

	// left
	if (m_is_keys_pressed[Key::A])
	{
		m_dpos.x = +sinf((m_camera_angle.x - 90) / 180 * PI) * m_speed;
		m_dpos.z = +cosf((m_camera_angle.x - 90) / 180 * PI) * m_speed;
	}

	//rigth
	if (m_is_keys_pressed[Key::D])
	{
		m_dpos.x = +sinf((m_camera_angle.x + 90) / 180 * PI) * m_speed;
		m_dpos.z = +cosf((m_camera_angle.x + 90) / 180 * PI) * m_speed;
	}

	// up(jump)
	if (m_is_keys_pressed[Key::Space])
	{
		if (m_flying) {
			m_dpos.y = m_speed;
			m_on_ground = false;
		}
		else {
			if (m_on_ground) {
				m_dpos.y = 5.F / 8.F * COORDS_IN_BLOCK;
				m_on_ground = false;
			}
		}
	}

	// lshift
	if (m_is_keys_pressed[Key::LShift])
	{
		if (m_flying) {
			m_dpos.y = -m_speed;
		}
	}


	//if (time > 1.F) time = 1.F;
	if (m_flying) {
		m_on_ground = false;

		m_pos.x += m_dpos.x * time;
		collision(m_dpos.x, 0, 0);

		m_pos.y += m_dpos.y * time;
		collision(0, m_dpos.y, 0);

		m_pos.z += m_dpos.z * time;
		collision(0, 0, m_dpos.z);

		m_dpos.x = m_dpos.y = m_dpos.z = 0;
	} else {
		if (!m_on_ground)
			m_dpos.y -= 1.5F/16.F*COORDS_IN_BLOCK*time;

		m_on_ground = false; //reset

		m_pos.x += m_dpos.x * time;
		collision(m_dpos.x, 0, 0);

		m_pos.y += m_dpos.y * time;
		collision(0, m_dpos.y, 0);

		m_pos.z += m_dpos.z * time;
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

		if (m_map->is_block(Map::coord2block_coord(x), Map::coord2block_coord(y), Map::coord2block_coord(z))) {
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
		if (m_map->is_block(
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
	for (int matrix_x = Map::coord2block_coord(m_pos.x - m_size.x); matrix_x < (m_pos.x + m_size.x) / COORDS_IN_BLOCK; matrix_x++) {
		for (int matrix_y = Map::coord2block_coord(m_pos.y - m_size.y); matrix_y < (m_pos.y + m_size.y) / COORDS_IN_BLOCK; matrix_y++) {
			for (int matrix_z = Map::coord2block_coord(m_pos.z - m_size.z); matrix_z < (m_pos.z + m_size.z) / COORDS_IN_BLOCK; matrix_z++) {
				if (m_map->is_block(matrix_x, matrix_y, matrix_z)) { //if collided with block
					if (dx > 0)  m_pos.x = matrix_x * COORDS_IN_BLOCK - m_size.x;
					if (dx < 0)  m_pos.x = matrix_x * COORDS_IN_BLOCK + COORDS_IN_BLOCK + m_size.x;
					if (dy > 0)  m_pos.y = matrix_y * COORDS_IN_BLOCK- m_size.y;
					if (dy < 0) {
						m_pos.y = matrix_y * COORDS_IN_BLOCK + COORDS_IN_BLOCK + m_size.y;
						m_on_ground = true;
						m_dpos.y = 0;
					}
					if (dz > 0)  m_pos.z = matrix_z * COORDS_IN_BLOCK  - m_size.z;
					if (dz < 0)  m_pos.z = matrix_z * COORDS_IN_BLOCK + COORDS_IN_BLOCK  + m_size.z;
				}
			}
		}
	}
}


void Player::keyboard_input(sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::KeyPressed: {
		switch (e.key.code)
		{
		case sf::Keyboard::W:
			m_is_keys_pressed[Key::W] = true;
			break;
		case sf::Keyboard::A:
			m_is_keys_pressed[Key::A] = true;
			break;
		case sf::Keyboard::S:
			m_is_keys_pressed[Key::S] = true;
			break;
		case sf::Keyboard::D:
			m_is_keys_pressed[Key::D] = true;
			break;
		case sf::Keyboard::Space:
			m_is_keys_pressed[Key::Space] = true;
			break;
		case sf::Keyboard::LShift:
			m_is_keys_pressed[Key::LShift] = true;
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
