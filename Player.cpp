#include "Player.h"
#include "Maths.h"
#include "game_constants.h"
bool get_block(int mx, int my, int mz);

Player::Player()
{
	m_size = {5, 20, 5};
}


Player::~Player()
{
}

void Player::input()
{
	keyboard_input();
	mouse_input();
}

void Player::update(float time)
{
	//if (!m_on_ground) m_dpos.y -= 1.5*time;
	//m_on_ground = false; //reset

	m_pos.x += m_dpos.x * time;
	//collision(m_dpos.x, 0, 0);

	m_pos.y += m_dpos.y * time;
	//collision(0, m_dpos.y, 0);

	m_pos.z += m_dpos.z * time;
	//collision(0, 0, m_dpos.z);
	m_dpos.y = 0;
	m_dpos.x = m_dpos.z = 0;
}

void Player::collision(float dx, float dy, float dz)
{
	// for  blocks in player's area
	for (int matrix_x = (m_pos.x - m_size.x) / BLOCK_SIZE; matrix_x < (m_pos.x + m_size.x) / BLOCK_SIZE; matrix_x++) {
		for (int matrix_y = (m_pos.y - m_size.y) / BLOCK_SIZE; matrix_y < (m_pos.y + m_size.y) / BLOCK_SIZE; matrix_y++) {
			for (int matrix_z = (m_pos.z - m_size.z) / BLOCK_SIZE; matrix_z < (m_pos.z + m_size.z) / BLOCK_SIZE; matrix_z++) {
				if (get_block(matrix_x, matrix_y, matrix_z)) {
					if (dx > 0)  m_pos.x = matrix_x * BLOCK_SIZE - m_size.x;
					if (dx < 0)  m_pos.x = matrix_x * BLOCK_SIZE + BLOCK_SIZE + m_size.x;
					if (dy > 0)  m_pos.y = matrix_y * BLOCK_SIZE - m_size.y;
					if (dy < 0) {
						m_pos.y = matrix_y * BLOCK_SIZE + BLOCK_SIZE + m_size.y;
						m_on_ground = true;
						m_dpos.y = 0;
					}
					if (dz > 0)  m_pos.z = matrix_z * BLOCK_SIZE - m_size.z;
					if (dz < 0)  m_pos.z = matrix_z * BLOCK_SIZE + BLOCK_SIZE + m_size.z;
				}
			}
		}
	}
}

void Player::keyboard_input()
{
	// forward
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_dpos.x = -sin(m_camera_angle.x / 180 * PI) * m_speed;
		m_dpos.z = -cos(m_camera_angle.x / 180 * PI) * m_speed;

		m_dpos.y += +tan(m_camera_angle.y / 180 * PI )* m_speed;

	}

	// back
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_dpos.x = +sin(m_camera_angle.x / 180 * PI) * m_speed;
		m_dpos.z = +cos(m_camera_angle.x / 180 * PI) * m_speed;

		m_dpos.y += -tan(m_camera_angle.y / 180 * PI)* m_speed;
	}

	// left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_dpos.x = +sin((m_camera_angle.x - 90) / 180 * PI) * m_speed;
		m_dpos.z = +cos((m_camera_angle.x - 90) / 180 * PI) * m_speed;
	}

	//rigth
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_dpos.x = +sin((m_camera_angle.x + 90) / 180 * PI) * m_speed;
		m_dpos.z = +cos((m_camera_angle.x + 90) / 180 * PI) * m_speed;
	}

	// up(jump)
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		//if (m_on_ground) {
			m_dpos.y = 12;
			m_on_ground = false;
		//}
	}
}

void Player::mouse_input()
{

}
