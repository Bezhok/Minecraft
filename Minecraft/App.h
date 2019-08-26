#pragma once
#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Player.h"
#include "MapMeshBuilder.h"


class Menu;

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
	//std::shared_ptr<World::Map> m_map;
	std::shared_ptr<World::Map> m_map;

	bool m_should_display_debug_info = true;
	bool m_should_fix_cursor = true;

	
	World::MapMeshBuilder m_map_mesh_builder;
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
	void update();


	/* send data to render */
	void draw_SFML();
	void draw_openGL();

};

