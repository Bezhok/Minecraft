#pragma once
#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"

#include "Player.h"

//using spp::sparse_hash_map;
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

	/*
	spp::sparse_hash_map<glm::vec3i, World::Chunk*> m_chunks4rendering;
	std::vector<std::pair<glm::vec3i, World::Chunk*>> m_chunks4updating;

	std::vector<std::pair<glm::vec3i, World::Chunk*>> m_chunks4vbo_generation;
	*/

	//std::unordered_map<glm::vec3i, World::Chunk*> m_chunks4vbo_generation;



	bool m_debug_info = true;
	bool m_handle_cursor = true;


	
	std::unordered_set<World::Chunk*> m_chunks4rendering;
	std::vector<World::Chunk*> m_chunks4updating;

	std::vector<World::Chunk*> m_chunks4vbo_generation;
	
public:


	sf::Mutex m_mutex__chunks4vbo_generation, m_mutex__chunks4rendering;
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


	/* send data to render */
	void draw_SFML();
	void draw_openGL();


	void update_vao_list();

	void update_vao_list2();
};

