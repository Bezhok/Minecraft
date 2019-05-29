#include "pch.h"
#include "Player.h"
#include "game_constants.h"
#include "Map.h"
#include "block_db.h"

using namespace World;


bool is_keys_pressed[6] = { false };

enum Key
{
	W = 0, A, S, D, Space, LShift
};

void Player::init(Map *map)
{
	m_map = map;

	m_size = { BLOCK_SIZE / 4.F, BLOCK_SIZE, BLOCK_SIZE / 4.F };
	m_pos = { 60 * BLOCK_SIZE, 155*BLOCK_SIZE, 15 * BLOCK_SIZE };
	//m_pos = { 10,10,10 };
	//m_pos = { 1800, 2900, 1800 };
	//m_pos = { 0, 20, 0 };
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
	if (is_keys_pressed[Key::W])
	{
		m_dpos.x = -sinf(m_camera_angle.x / 180 * PI) * m_speed;
		m_dpos.z = -cosf(m_camera_angle.x / 180 * PI) * m_speed;
	}

	// back
	if (is_keys_pressed[Key::S])
	{
		m_dpos.x = +sinf(m_camera_angle.x / 180 * PI) * m_speed;
		m_dpos.z = +cosf(m_camera_angle.x / 180 * PI) * m_speed;
	}

	// left
	if (is_keys_pressed[Key::A])
	{
		m_dpos.x = +sinf((m_camera_angle.x - 90) / 180 * PI) * m_speed;
		m_dpos.z = +cosf((m_camera_angle.x - 90) / 180 * PI) * m_speed;
	}

	//rigth
	if (is_keys_pressed[Key::D])
	{
		m_dpos.x = +sinf((m_camera_angle.x + 90) / 180 * PI) * m_speed;
		m_dpos.z = +cosf((m_camera_angle.x + 90) / 180 * PI) * m_speed;
	}

	// up(jump)
	if (is_keys_pressed[Key::Space])
	{
		if (m_flying) {
			m_dpos.y = m_speed;
			m_on_ground = false;
		}
		else {
			if (m_on_ground) {
				m_dpos.y = 5.F / 8.F * BLOCK_SIZE;
				m_on_ground = false;
			}
		}
	}

	// lshift
	if (is_keys_pressed[Key::LShift])
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
			m_dpos.y -= 1.5F/16.F*BLOCK_SIZE*time;

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
		m_speed = STANDART_PLAYER_SPEED;
	}
}

void Player::put_block()
{
	float prev_x, prev_y, prev_z;
	float x = m_pos.x,
		y = m_pos.y,
		z = m_pos.z;

	prev_x = x; prev_y = y; prev_z = z;
	bool able_create = false;
	for (int distance = 0; distance < 50 * BLOCK_SIZE; ++distance) {
		
		x += -sinf(m_camera_angle.x / 180 * PI)/10.F;
		y += tanf(m_camera_angle.y / 180 * PI)/10.F;
		z += -cosf(m_camera_angle.x / 180 * PI)/10.F;

		if (m_map->is_block(x/BLOCK_SIZE, y/BLOCK_SIZE, z/BLOCK_SIZE)) {
			// is we in this block
			for (int matrix_x = (m_pos.x - m_size.x) / BLOCK_SIZE; matrix_x < (m_pos.x + m_size.x) / BLOCK_SIZE; matrix_x++) {
				for (int matrix_y = (m_pos.y - m_size.y) / BLOCK_SIZE; matrix_y < (m_pos.y + m_size.y) / BLOCK_SIZE; matrix_y++) {
					for (int matrix_z = (m_pos.z - m_size.z) / BLOCK_SIZE; matrix_z < (m_pos.z + m_size.z) / BLOCK_SIZE; matrix_z++) {
						int x = prev_x / BLOCK_SIZE;
						int y = prev_y / BLOCK_SIZE;
						int z = prev_z / BLOCK_SIZE;
						if (matrix_x == x && matrix_y == y && matrix_z == z) {
							able_create = false;
							break;
						}
					}
				}
			}

			if (able_create) {
				m_map->create_block(
					prev_x / BLOCK_SIZE,
					prev_y / BLOCK_SIZE,
					prev_z / BLOCK_SIZE,
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
		y = m_pos.y,
		z = m_pos.z;

	bool able_create = false;
	for (int distance = 0; distance < 50 * BLOCK_SIZE; ++distance) {
		x += -sinf(m_camera_angle.x / 180 * PI)/10.F;
		y += tanf(m_camera_angle.y / 180 * PI)/10.F;
		z += -cosf(m_camera_angle.x / 180 * PI)/10.F;
		if (m_map->is_block(
			x / BLOCK_SIZE,
			y / BLOCK_SIZE,
			z / BLOCK_SIZE
		)) {
			m_map->delete_block(
				x / BLOCK_SIZE,
				y / BLOCK_SIZE,
				z / BLOCK_SIZE
			);
			break;
		}
	}
}

void Player::collision(float dx, float dy, float dz)
{
	 //for  blocks in player's area
	for (int matrix_x = (m_pos.x - m_size.x) / BLOCK_SIZE; matrix_x < (m_pos.x + m_size.x) / BLOCK_SIZE; matrix_x++) {
		for (int matrix_y = (m_pos.y - m_size.y) / BLOCK_SIZE; matrix_y < (m_pos.y + m_size.y) / BLOCK_SIZE; matrix_y++) {
			for (int matrix_z = (m_pos.z - m_size.z) / BLOCK_SIZE; matrix_z < (m_pos.z + m_size.z) / BLOCK_SIZE; matrix_z++) {
				if (m_map->is_block(matrix_x, matrix_y, matrix_z)) { //if collided with block
					if (dx > 0)  m_pos.x = matrix_x * BLOCK_SIZE - m_size.x;
					if (dx < 0)  m_pos.x = matrix_x * BLOCK_SIZE + BLOCK_SIZE + m_size.x;
					if (dy > 0)  m_pos.y = matrix_y * BLOCK_SIZE- m_size.y;
					if (dy < 0) {
						m_pos.y = matrix_y * BLOCK_SIZE + BLOCK_SIZE + m_size.y;
						m_on_ground = true;
						m_dpos.y = 0;
					}
					if (dz > 0)  m_pos.z = matrix_z * BLOCK_SIZE  - m_size.z;
					if (dz < 0)  m_pos.z = matrix_z * BLOCK_SIZE + BLOCK_SIZE  + m_size.z;
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
			is_keys_pressed[Key::W] = true;
			break;
		case sf::Keyboard::A:
			is_keys_pressed[Key::A] = true;
			break;
		case sf::Keyboard::S:
			is_keys_pressed[Key::S] = true;
			break;
		case sf::Keyboard::D:
			is_keys_pressed[Key::D] = true;
			break;
		case sf::Keyboard::Space:
			is_keys_pressed[Key::Space] = true;
			break;
		case sf::Keyboard::LShift:
			is_keys_pressed[Key::LShift] = true;
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
			is_keys_pressed[Key::W] = false;
			break;
		case sf::Keyboard::A:
			is_keys_pressed[Key::A] = false;
			break;
		case sf::Keyboard::S:
			is_keys_pressed[Key::S] = false;
			break;
		case sf::Keyboard::D:
			is_keys_pressed[Key::D] = false;
			break;
		case sf::Keyboard::Space:
			is_keys_pressed[Key::Space] = false;
			break;
		case sf::Keyboard::LShift:
			is_keys_pressed[Key::LShift] = false;
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
