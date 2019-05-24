#pragma once
#include "pch.h"
#include "Player.h"
//#include "Block.h"
#include "Renderer.h"
#include "DebugData.h"
//#include "Menu.h"

class Menu;
//class Player;
////class Block;
//class Renderer;
//class DebugData;
namespace World { class Map; }

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
	World::Map m_map;
	std::unordered_map<size_t, Chunk*> m_chunks4rendering;


	std::vector<GLuint> m_vao_list;
	bool m_debug_info = true;
	bool m_handle_cursor = true;

	void update_gllist(
		int old_chunk_x,
		int old_chunk_z,
		int new_chunk_x,
		int new_chunk_z
	);

public:
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
	void update(sf::Clock& timer);

	/* create all gllists from memory */
	void create_all_gllists();

	/* send data to render */
	void draw_SFML();
	void draw_openGL();

	/* eponymous */
	void update_gllist(const sf::Vector3i& c);

	/* eponymous */
	void create_gllist(const sf::Vector3i& c, size_t hash);

	void update_vao_list();
	void update_game_logic();
};

