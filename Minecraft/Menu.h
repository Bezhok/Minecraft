#pragma once
#include "pch.h"
#include "block_db.h"
#include "Player.h"

using std::vector;
using std::unordered_map;

class Menu
{
public:
	enum MenuState {
		main, statistics
	};

	enum MenuElement {
		tool_bar,
		curr_tool
	};

	Menu(sf::RenderWindow &window);
	~Menu();

	/* handle input */
	void input(sf::Event &e);

	void update_players_blocks(Player &player);

	unordered_map<enum World::DB::block_id, sf::Sprite> m_side_sprites;
	unordered_map<enum MenuElement, sf::Sprite> m_sprites;
private:
	sf::RenderWindow &m_window;
	MenuState m_state = MenuState::main;
	unordered_map<enum MenuElement, sf::Texture> m_textures;

	void keyboard_input(sf::Event &e);
	void mouse_input(sf::Event &e);

	/* check mouse position */
	bool is_on_sprite(sf::Sprite obj);
	bool is_on_rect(sf::RectangleShape obj);
};
