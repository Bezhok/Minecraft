#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

class App
{
public:
	App(sf::RenderWindow &window);
	~App();

	void run();
private:
	sf::RenderWindow &m_window;
	Player m_player;

	void handle_events();
	void update(sf::Clock &timer);

	bool m_debug_info = false;
	bool m_handle_cursor = true;
};

