#pragma once
#include "pch.h"
#include "Player.h"
#include "Block.h"
#include "Renderer.h"

class App
{
public:
	App(sf::RenderWindow &window);
	~App();

	void run();
private:
	sf::RenderWindow &m_window;
	Player m_player;
	World::Map m_map;
	Renderer m_renderer;

	void handle_events();
	void update(sf::Clock &timer);

	void recreate_gllist(World::Block &block, std::unordered_map<size_t, GLuint> &world_list);
	void update_gllist(
		Block &block,
		std::unordered_map<size_t, GLuint> &world_list,
		int old_chunk_x,
		int old_chunk_z,
		int new_chunk_x,
		int new_chunk_z
	);

	bool m_debug_info = false;
	bool m_handle_cursor = true;
};

