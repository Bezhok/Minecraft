#pragma once
#include "pch.h"
#include "block_db.h"
#include "Player.h"

class Menu
{
	enum MenuElement {
		tool_bar,
		curr_tool
	};

private:
	sf::RenderWindow &m_window;
	std::unordered_map<enum MenuElement, sf::Sprite> m_sprites;
	std::unordered_map<enum World::DB::block_id, sf::Sprite> m_side_sprites; // tool bar items
	std::unordered_map<enum MenuElement, sf::Texture> m_textures;

public:
	/* init MenuElements */
	Menu(sf::RenderWindow &window);
	~Menu();

	/* eponymous */
	void input(sf::Event &e);

	/* should be called when resized */
	void update();

	/* load sprites for tool bar */
	void update_players_blocks(Player &player);

	/* access */
	const auto& get_spites() { return m_sprites; };
	const auto& get_top_spites() { return m_side_sprites; };
private:
	/* eponymous */
	void keyboard_input(sf::Event &e);
	void mouse_input(sf::Event &e);
};
