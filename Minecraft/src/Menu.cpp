#include "pch.h"
#include "Menu.h"
#include "game_constants.h"
#include "block_db.h"
#include "Player.h"
#include "MenuElement.h"
#include "InputEvent.h"

using namespace World;


Menu::Menu(sf::RenderWindow &window)
        : m_window{window} {
    //tool bar
    m_textures[MenuElement::tool_bar].loadFromFile("resources/textures/gui/tool_bar.png");
    m_sprites[MenuElement::tool_bar].setTexture(m_textures[MenuElement::tool_bar]);

    m_sprites[MenuElement::tool_bar].setPosition(
            float(m_window.getSize().x / 2 - int(m_textures[MenuElement::tool_bar].getSize().x) / 2),
            float(m_window.getSize().y - int(m_textures[MenuElement::tool_bar].getSize().y))
    );

    // curr tool
    m_textures[MenuElement::curr_tool].loadFromFile("resources/textures/gui/curr_tool.png");
    m_sprites[MenuElement::curr_tool].setTexture(m_textures[MenuElement::curr_tool]);

    m_sprites[MenuElement::curr_tool].setPosition(
            float(m_sprites[MenuElement::tool_bar].getPosition().x - 2),
            float(m_window.getSize().y - m_textures[MenuElement::curr_tool].getSize().y + 2)
    );

    //cross
    m_textures[MenuElement::cross].loadFromFile("resources/textures/gui/cross.png");
    m_sprites[MenuElement::cross].setTexture(m_textures[MenuElement::cross]);

    m_sprites[MenuElement::cross].setPosition(
            float(m_window.getSize().x / 2 - int(m_textures[MenuElement::cross].getSize().x / 2)),
            float(m_window.getSize().y / 2 - int(m_textures[MenuElement::cross].getSize().y / 2))
    );

    //atlas
    m_textures[MenuElement::atlas].loadFromFile(PATH2ATLAS);
}

Menu::~Menu() {
}

void Menu::update() {
    // tool bar
    sf::Vector2f bar_old_pos = m_sprites[MenuElement::tool_bar].getPosition();
    sf::Vector2f bar_new_pos = {
            float(m_window.getSize().x / 2 - int(m_textures[MenuElement::tool_bar].getSize().x) / 2),
            float(m_window.getSize().y - int(m_textures[MenuElement::tool_bar].getSize().y))
    };
    m_sprites[MenuElement::tool_bar].setPosition(bar_new_pos);

    sf::Vector2f tool_old_pos = m_sprites[MenuElement::curr_tool].getPosition();
    m_sprites[MenuElement::curr_tool].setPosition(tool_old_pos + bar_new_pos - bar_old_pos);

    for (auto &e : m_side_sprites) {
        e.second.setPosition(e.second.getPosition() + bar_new_pos - bar_old_pos);
    }

    m_sprites[MenuElement::cross].setPosition(
            float(m_window.getSize().x / 2 - int(m_textures[MenuElement::cross].getSize().x / 2)),
            float(m_window.getSize().y / 2 - int(m_textures[MenuElement::cross].getSize().y / 2))
    );
}

void Menu::update_players_blocks(Player &player) {
    m_side_sprites.clear();

    int i = 0;
    for (auto &e : player.get_inventory()) {

        if (e.first != block_id::EMPTY_TYPE) {
            m_side_sprites[e.first].setTexture(m_textures[MenuElement::atlas]);

            sf::Vector2i &p = DB::s_atlas_db(e.first, Side::positive_x);
            m_side_sprites[e.first].setTextureRect(sf::IntRect(
                    p.x * BLOCK_RESOLUTION,
                    p.y * BLOCK_RESOLUTION,
                    BLOCK_RESOLUTION,
                    BLOCK_RESOLUTION
            ));

            sf::Vector2f pos = m_sprites[MenuElement::tool_bar].getPosition();
            pos.y += 8;
            pos.x += 8;

            float x_shift = float(m_textures[MenuElement::tool_bar].getSize().x - 2) / 9.F * i;
            pos.x += x_shift;

            m_side_sprites[e.first].setScale(1.6f, 1.6f);
            m_side_sprites[e.first].setPosition(pos);
            ++i;
        }
    }
}

void Menu::on_notify(const InputEvent *event) {
  if (event->type == EventType::CHANGE_INV_ITEM) {
    change_inventory_item(event->delta.x);
  }
}

void Menu::change_inventory_item(int deltaX) {
  // toolbar
  sf::Vector2f pos = m_sprites[MenuElement::curr_tool].getPosition();

  float dx = deltaX * float(m_textures[MenuElement::tool_bar].getSize().x - 2) / 9.F;
  float tool_bar_block_size_x = float(m_textures[MenuElement::tool_bar].getSize().x - 2) / 9.F;
  float curr_tool_x = m_sprites[MenuElement::curr_tool].getPosition().x;
  float tool_bar_x = m_sprites[MenuElement::tool_bar].getPosition().x;
  int tool_bar_size_x = m_textures[MenuElement::tool_bar].getSize().x;

  // ring closure
  if (curr_tool_x + dx >= tool_bar_x + tool_bar_size_x - tool_bar_block_size_x / 2.F) {
    pos.x = tool_bar_x - 2;
  } else if (curr_tool_x + dx <= tool_bar_x - tool_bar_block_size_x / 2.F) {
    pos.x = tool_bar_x + tool_bar_size_x - tool_bar_block_size_x - 4;
  } else {
    pos.x += dx;
  }

  m_sprites[MenuElement::curr_tool].setPosition(pos);
}