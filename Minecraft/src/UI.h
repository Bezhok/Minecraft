#pragma once

#include "pch.h"
#include "UIElement.h"
#include "Observer.h"
#include <optional>

class Player;
namespace World {
    enum class BlockType : uint8_t;
}

class UI : public Observer {
public:
    /* init MenuElements */
    UI(sf::RenderWindow &window);

    ~UI();

    /* should be called when resized */
    void update();

    /* load sprites for tool bar */
    void update_players_blocks(Player &player);

    /* access */
    const auto &get_spites() { return m_sprites; };

    const auto &get_top_spites() { return m_hot_bar_sprites; };
private:
    void on_notify(const InputEvent *event) override;

    void change_inventory_item(int deltaX);

    sf::RenderWindow &m_window;
    std::unordered_map<UIElement, sf::Sprite> m_sprites;
    std::vector<sf::Sprite> m_hot_bar_sprites;
    std::unordered_map<UIElement, sf::Texture> m_textures;
};
