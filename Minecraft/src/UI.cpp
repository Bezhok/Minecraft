#include "pch.h"
#include "UI.h"
#include "game_constants.h"
#include "block_db.h"
#include "Player.h"
#include "UIElement.h"
#include "InputEvent.h"

using namespace World;

UI::UI(sf::RenderWindow &window)
        : m_window{window} {
    //tool bar
    m_textures[UIElement::tool_bar].loadFromFile("resources/textures/gui/tool_bar.png");
    m_sprites[UIElement::tool_bar].setTexture(m_textures[UIElement::tool_bar]);

    m_sprites[UIElement::tool_bar].setPosition(
            float(m_window.getSize().x / 2 - int(m_textures[UIElement::tool_bar].getSize().x) / 2),
            float(m_window.getSize().y - int(m_textures[UIElement::tool_bar].getSize().y))
    );

    // curr tool
    m_textures[UIElement::curr_tool].loadFromFile("resources/textures/gui/curr_tool.png");
    m_sprites[UIElement::curr_tool].setTexture(m_textures[UIElement::curr_tool]);

    m_sprites[UIElement::curr_tool].setPosition(
            float(m_sprites[UIElement::tool_bar].getPosition().x - 2),
            float(m_window.getSize().y - m_textures[UIElement::curr_tool].getSize().y + 2)
    );

    //cross
    m_textures[UIElement::cross].loadFromFile("resources/textures/gui/cross.png");
    m_sprites[UIElement::cross].setTexture(m_textures[UIElement::cross]);

    m_sprites[UIElement::cross].setPosition(
            float(m_window.getSize().x / 2 - int(m_textures[UIElement::cross].getSize().x / 2)),
            float(m_window.getSize().y / 2 - int(m_textures[UIElement::cross].getSize().y / 2))
    );

    //atlas
    m_textures[UIElement::atlas].loadFromFile(PATH2ATLAS);
}

UI::~UI() {
}

void UI::update() {
    // tool bar
    sf::Vector2f bar_old_pos = m_sprites[UIElement::tool_bar].getPosition();
    sf::Vector2f bar_new_pos = {
            float(m_window.getSize().x / 2 - int(m_textures[UIElement::tool_bar].getSize().x) / 2),
            float(m_window.getSize().y - int(m_textures[UIElement::tool_bar].getSize().y))
    };
    m_sprites[UIElement::tool_bar].setPosition(bar_new_pos);

    sf::Vector2f tool_old_pos = m_sprites[UIElement::curr_tool].getPosition();
    m_sprites[UIElement::curr_tool].setPosition(tool_old_pos + bar_new_pos - bar_old_pos);

    for (auto &e : m_hot_bar_sprites) {
        e.setPosition(e.getPosition() + bar_new_pos - bar_old_pos);
    }

    m_sprites[UIElement::cross].setPosition(
            float(m_window.getSize().x / 2 - int(m_textures[UIElement::cross].getSize().x / 2)),
            float(m_window.getSize().y / 2 - int(m_textures[UIElement::cross].getSize().y / 2))
    );
}

void UI::update_players_blocks(Player &player) {
    m_hot_bar_sprites.clear();

    for (int i = 0; i < player.get_inventory().get_hot_bar_items().size(); ++i) {
        const auto &e = player.get_inventory().get_hot_bar_items()[i];

        if (e.first != BlockType::EMPTY_TYPE) {
            sf::Sprite sprite;
            sprite.setTexture(m_textures[UIElement::atlas]);

            sf::Vector2i &p = DB::s_atlas_db(e.first, Side::positive_x);
            sprite.setTextureRect(sf::IntRect(
                    p.x * BLOCK_RESOLUTION,
                    p.y * BLOCK_RESOLUTION,
                    BLOCK_RESOLUTION,
                    BLOCK_RESOLUTION
            ));

            sf::Vector2f pos = m_sprites[UIElement::tool_bar].getPosition();
            pos.y += 8;
            pos.x += 8;

            float x_shift = float(m_textures[UIElement::tool_bar].getSize().x - 2) / 9.F * i;
            pos.x += x_shift;

            sprite.setScale(1.6f, 1.6f);
            sprite.setPosition(pos);

            m_hot_bar_sprites.push_back(sprite);
        }
    }
}

void UI::on_notify(const InputEvent *event) {
    if (event->type == EventType::CHANGE_INV_ITEM) {
        change_inventory_item(event->delta.x);
    }
}

void UI::change_inventory_item(int deltaX) {
    // toolbar
    static int idx = 0;
    sf::Vector2f pos = m_sprites[UIElement::curr_tool].getPosition();
    float tool_bar_x = m_sprites[UIElement::tool_bar].getPosition().x;

    idx = (idx + deltaX + HOTBAR_INV_SIZE) % HOTBAR_INV_SIZE;
    float nx = idx * float(m_textures[UIElement::tool_bar].getSize().x - 1) / HOTBAR_INV_SIZE;
    pos.x = tool_bar_x + nx - 2;
    m_sprites[UIElement::curr_tool].setPosition(pos);
}