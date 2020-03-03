#pragma once

#include "pch.h"
#include "DebugData.h"
#include "Player.h"

class Menu;
class Renderer;
namespace World {
    class MapMeshBuilder;
    class Map;
}

class App {
private:
    sf::Text m_text;
    sf::Font m_font;
    DebugData m_debug_data;
    Player m_player;
    sf::RenderWindow &m_window;

    std::unique_ptr<Menu> m_menu;
    std::unique_ptr<Renderer> m_renderer;
    std::shared_ptr<World::Map> m_map;
    std::unique_ptr<World::MapMeshBuilder> m_map_mesh_builder;

    bool m_should_display_debug_info = true;
    bool m_should_fix_cursor = true;
public:
    /* init some objects */
    explicit App(sf::RenderWindow &window);

    /* Run game cycle */
    void run();

    ~App();
private:
    /* input and camera */
    void handle_events();

    /* eponymous */
    void input();

    /* update entity activity */
    void update();

    /* send data to render */
    void draw_SFML();

    void draw_openGL();

    std::string debug_text();

    void process_key(sf::Keyboard::Key code);

    void resize(uint32_t x, uint32_t y);

    void draw_under_water_filter();
};

