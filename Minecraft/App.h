#pragma once
#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Player.h"


class Menu;
class Player;

class App
{
private:
	sf::Text m_text;
	sf::Font m_font;
	DebugData m_debug_data;
	Menu *m_menu;
	sf::RenderWindow& m_window;
	Player m_player;
	Renderer m_renderer;
	std::shared_ptr<World::Map> m_map;

	bool m_should_display_debug_info = true;
	bool m_should_fix_cursor = true;

	std::unordered_set<World::Chunk*> m_chunks4rendering;
	std::vector<World::Chunk*> m_chunks4updating;
	std::vector<World::Chunk*> m_chunks4vbo_generation;
	
public:
	sf::Mutex m_mutex__chunks4vbo_generation, m_mutex__chunks4rendering, m_mutex_4rendering;
	/* init some objects */
	App(sf::RenderWindow& window);

	/* Run game cycle */
	void run();
private:
	/* input and camera */
	void handle_events();

	/* eponymous */
	void input();

	/* update entity activity */
	void update();


	/* send data to render */
	void draw_SFML();
	void draw_openGL();


	void generate_verticies();
};

