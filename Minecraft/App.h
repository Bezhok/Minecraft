#pragma once
#include "pch.h"
#include "Player.h"
#include "Block.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Menu.h"

class App
{
private:
	sf::Text m_text;
	sf::Font m_font;
	DebugData m_debug_data;

	Menu *m_menu;
	sf::RenderWindow &m_window;
	Player m_player;
	World::Map m_map;
	Renderer m_renderer;
	std::unordered_map<size_t, GLuint> m_world_list;

	bool m_debug_info = false;
	bool m_handle_cursor = true;

public:
	/* init some objects */
	App(sf::RenderWindow &window);

	/* Run game cycle */
	void run();
private:
	/* input and camera */
	void handle_events();

	/* eponymous */
	void input();

	/* update entity activity */
	void update(sf::Clock &timer);

	/* create all gllists from memory */
	void create_all_gllists(Block &block);

	/* send data to render */
	void draw_SFML();
	void draw_openGL();

	/* eponymous */
	void update_gllist(Block &block, const sf::Vector3i &c);

	/* eponymous */
	void create_gllist(Block &block, const sf::Vector3i &c, size_t hash);
};

