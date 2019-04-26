#include "pch.h"
#include "Menu.h"
#include "game_constants.h"
#include "block_db.h"

using namespace World;

Menu::Menu(sf::RenderWindow &window)
	: m_window{ window }
{
	//MenuElement
	m_textures[tool_bar].loadFromFile("resources/textures/gui/tool_bar.png");
	m_sprites[tool_bar].setTexture(m_textures[tool_bar]);

	m_sprites[tool_bar].setPosition(
		float(WINDOW_WIDTH/2 - int(m_textures[tool_bar].getSize().x)/2),
		float(WINDOW_HEIGTH - int(m_textures[tool_bar].getSize().y))
	);

	m_textures[curr_tool].loadFromFile("resources/textures/gui/curr_tool.png");
	m_sprites[curr_tool].setTexture(m_textures[curr_tool]);

	m_sprites[curr_tool].setPosition(
		float(m_sprites[tool_bar].getPosition().x - 2),
		float(WINDOW_HEIGTH - m_textures[curr_tool].getSize().y + 2)
	);

}

Menu::~Menu()
{
}

void Menu::input(sf::Event &e)
{
	keyboard_input(e);
	mouse_input(e);
}

void Menu::update_players_blocks(Player & player)
{
	//DB::side_textures[e.first]DB::side_textures
	m_side_sprites.clear();

	int i = 0;
	for (auto &e : player.m_inventory) {
		m_side_sprites[e.first].setTexture(DB::side_textures[e.first]);
		sf::Vector2f pos = m_sprites[tool_bar].getPosition();
		pos.y += 8; pos.x += 8;
		pos.x += float(m_textures[tool_bar].getSize().x - 2) / 9.F * i;

		m_side_sprites[e.first].setScale(0.8f, 0.8f);
		m_side_sprites[e.first].setPosition(pos);
		++i;
	}
}

void Menu::keyboard_input(sf::Event &e)
{
}

void Menu::mouse_input(sf::Event &e)
{
	// rshift
	if (e.type == sf::Event::MouseButtonReleased) {
		if (e.key.code == sf::Mouse::Left) {
			//if (is_on_rect()) {

			//}
		}
		else if (e.key.code == sf::Mouse::Right) {
			
		}
	} else if (e.type == sf::Event::MouseWheelMoved) {
		// toolbar
		sf::Vector2f pos = m_sprites[curr_tool].getPosition();

		float dx = e.mouseWheel.delta * float(m_textures[tool_bar].getSize().x-2)/9.F;
		float tool_bar_block_size_x = float(m_textures[tool_bar].getSize().x - 2) / 9.F;
		float curr_tool_x = m_sprites[curr_tool].getPosition().x;
		float tool_bar_x = m_sprites[tool_bar].getPosition().x;
		int tool_bar_size_x = m_textures[tool_bar].getSize().x;

		if (curr_tool_x + dx >= tool_bar_x + tool_bar_size_x - tool_bar_block_size_x / 2.F) {
			pos.x = tool_bar_x - 2;
		}
		else if (curr_tool_x + dx <= tool_bar_x - tool_bar_block_size_x / 2.F) {
			pos.x = tool_bar_x + tool_bar_size_x - tool_bar_block_size_x - 4;
		}
		else {
			pos.x += dx;
		}

		m_sprites[curr_tool].setPosition(pos);
	}
}

bool Menu::is_on_sprite(sf::Sprite obj)
{
	if (obj.getGlobalBounds().contains(
			(float)sf::Mouse::getPosition(m_window).x,
			(float)sf::Mouse::getPosition(m_window).y)
		) {
		return true;
	}

	return false;
}

bool Menu::is_on_rect(sf::RectangleShape obj)
{
	if (obj.getGlobalBounds().contains(
			(float)sf::Mouse::getPosition(m_window).x,
			(float)sf::Mouse::getPosition(m_window).y)
		) {
		return true;
	}

	return false;
}