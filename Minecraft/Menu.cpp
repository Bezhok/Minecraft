#include "pch.h"
#include "Menu.h"
#include "game_constants.h"
#include "block_db.h"

using namespace World;

Menu::Menu(sf::RenderWindow &window)
	: m_window{ window }
{
	//MenuElement
	auto &bar = m_textures[tool_bar];
	auto &border = m_textures[curr_tool];

	m_textures[tool_bar].loadFromFile("resources/textures/gui/tool_bar.png");
	m_sprites[tool_bar].setTexture(m_textures[tool_bar]);

	m_sprites[tool_bar].setPosition(
		float(m_window.getSize().x /2 - int(m_textures[tool_bar].getSize().x)/2),
		float(m_window.getSize().y - int(m_textures[tool_bar].getSize().y))
	);

	m_textures[curr_tool].loadFromFile("resources/textures/gui/curr_tool.png");
	m_sprites[curr_tool].setTexture(m_textures[curr_tool]);

	m_sprites[curr_tool].setPosition(
		float(m_sprites[tool_bar].getPosition().x - 2),
		float(m_window.getSize().y - m_textures[curr_tool].getSize().y + 2)
	);

}

Menu::~Menu()
{
}

void Menu::update()
{
	// tool bar
	sf::Vector2f bar_old_pos = m_sprites[tool_bar].getPosition();
	sf::Vector2f bar_new_pos = {
		float(m_window.getSize().x / 2 - int(m_textures[tool_bar].getSize().x) / 2),
		float(m_window.getSize().y - int(m_textures[tool_bar].getSize().y))
	};

	m_sprites[tool_bar].setPosition(bar_new_pos);

	sf::Vector2f tool_old_pos = m_sprites[curr_tool].getPosition();
	m_sprites[curr_tool].setPosition(tool_old_pos + bar_new_pos - bar_old_pos);


	for (auto &e : m_side_sprites) {
		e.second.setPosition(e.second.getPosition() + bar_new_pos - bar_old_pos);
	}
}

void Menu::input(sf::Event &e)
{
	keyboard_input(e);
	mouse_input(e);
}

void Menu::update_players_blocks(Player & player)
{
	m_side_sprites.clear();

	int i = 0;
	for (auto &e : player.m_inventory) {
		m_side_sprites[e.first].setTexture(DB::side_textures[e.first]);
		sf::Vector2f pos = m_sprites[tool_bar].getPosition();
		pos.y += 8; pos.x += 8;

		float x_shift = float(m_textures[tool_bar].getSize().x - 2) / 9.F * i;
		pos.x += x_shift;

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

		float dx                    = e.mouseWheel.delta * float(m_textures[tool_bar].getSize().x-2)/9.F;
		float tool_bar_block_size_x = float(m_textures[tool_bar].getSize().x - 2) / 9.F;
		float curr_tool_x           = m_sprites[curr_tool].getPosition().x;
		float tool_bar_x            = m_sprites[tool_bar].getPosition().x;
		int   tool_bar_size_x       = m_textures[tool_bar].getSize().x;

		// ring closure
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