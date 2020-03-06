#pragma once

#include "pch.h"
#include "MenuElement.h"
#include "Observer.h"

class Player;
namespace World {
enum class block_id : uint8_t;
}

class Menu : public Observer {
 private:
    sf::RenderWindow &m_window;
    std::unordered_map<MenuElement, sf::Sprite> m_sprites;
    std::unordered_map<World::block_id, sf::Sprite> m_side_sprites; // tool bar items
    std::unordered_map<MenuElement, sf::Texture> m_textures;

 public:
    /* init MenuElements */
    Menu(sf::RenderWindow &window);

    ~Menu();

    /* should be called when resized */
    void update();

    /* load sprites for tool bar */
    void update_players_blocks(Player &player);

    /* access */
    const auto &get_spites() { return m_sprites; };

    const auto &get_top_spites() { return m_side_sprites; };
 private:
    void on_notify(const InputEvent *event) override;
    void change_inventory_item(int deltaX);
};
