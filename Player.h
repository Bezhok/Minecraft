#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "Entity.h"
#include "World.h"
#include "game_constants.h"

class Player :
	public Entity
{
public:
	sf::Vector2f m_camera_angle;
	float m_speed = STANDART_PLAYER_SPEED;
	Player();
	void init(World *world);
	~Player();
	void input(sf::Event &e);
	void update(float time);

	void god_on() { m_god = true; }
	void god_off() { m_god = false; }

private:
	void collision(float dx, float dy, float dz);
	void flight_on();
	void flight_off();

	sf::Vector3f m_dpos;
	bool m_on_ground = false;
	bool m_flying = false;
	bool m_god = false;
	void keyboard_input(sf::Event &e);
	void mouse_input();
};

