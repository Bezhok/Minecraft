#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "Entity.h"

class Player :
	public Entity
{
public:
	sf::Vector2f m_camera_angle;
	float m_speed = 5.0;
	Player();
	~Player();
	void input();
	void update(float time);
private:
	void collision(float dx, float dy, float dz);

	sf::Vector3f m_dpos;
	bool m_on_ground = false;
	void keyboard_input();
	void mouse_input();
};

