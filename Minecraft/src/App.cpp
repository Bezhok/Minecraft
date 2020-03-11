#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Map.h"
#include "App.h"
#include "block_db.h"
#include "UI.h"
#include "MapMeshBuilder.h"
#include "InputManager.h"
#include "Inventory.h"

using namespace World;

App::App(sf::RenderWindow &window)
        : m_window{window} {
    m_font.loadFromFile("resources/arial.ttf");
    m_text.setFont(m_font);
    m_text.setCharacterSize(20);
    m_text.setFillColor(sf::Color(0, 0, 0, 200));

    m_map = std::make_shared<World::Map>();
    m_renderer = std::make_unique<Renderer>();
    m_map_mesh_builder = std::make_unique<MapMeshBuilder>();
    m_input_manager = std::make_unique<InputManager>(m_window);

    m_should_display_debug_info = false;
    m_window.setMouseCursorVisible(false);
    m_window.setActive(true);

    int x = m_window.getSize().x / 2;
    int y = m_window.getSize().y / 2;
    sf::Mouse::setPosition(sf::Vector2i(x, y), m_window);

    DB::load_blocks();
    m_player.init(m_map.get());
    m_player.god_on();
    m_player.flight_on();

    auto &hot_bar = m_player.get_inventory().get_hot_bar_items();
    for (int i = 0; i < hot_bar.size() && i < World::DB::s_loaded_blocks.size(); ++i) {
        hot_bar[i] = {World::DB::s_loaded_blocks[i], 1};
    }

    m_menu = std::make_unique<UI>(m_window);
    m_menu->update_players_blocks(m_player);

    m_input_manager->add(&m_player);
    m_input_manager->add(m_menu.get());
    m_input_manager->set_should_fix_cursor(m_should_fix_cursor);

}

std::string App::debug_text() {
    std::string str =
            "fps: " +
            std::to_string(int(m_debug_data.get_fps())) + "\n" +
            "ft: " +
            std::to_string(int(m_debug_data.get_frame_time())) + "\n" +
            "x, y, z: " +
            std::to_string(m_player.get_position().x) + " " +
            std::to_string(m_player.get_position().y) + " " +
            std::to_string(m_player.get_position().z) + "\n" +
            std::to_string(m_map_mesh_builder->get_chunks4rendering_size()) + " " +
            std::to_string(m_map->get_size()) + " " +
            std::to_string(m_map_mesh_builder->get_chunks4vertices_generation_size()) + " " +
            "- chunks rendering, map size, updates, global buffer\n" +
            std::to_string(vertices_wasnt_free) + " - chunks which veticies memory is not freed";

    return str;
}

void App::draw_under_water_filter() {
    static sf::RectangleShape underwater_filter;
    underwater_filter.setSize({static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)});
    underwater_filter.setFillColor({28, 40, 155, 150});
    underwater_filter.setPosition(0, 0);
    m_renderer->draw_SFML(underwater_filter);
}

void App::draw_SFML() {
    if (m_player.m_is_under_water) {
        draw_under_water_filter();
    }

    //draw sfml
    if (m_should_display_debug_info) {
        m_text.setString(debug_text());
        m_renderer->draw_SFML(m_text);
    }

    for (auto &e : m_menu->get_spites()) {
        m_renderer->draw_SFML(e.second);
    }

    for (auto &e : m_menu->get_top_spites()) {
        m_renderer->draw_SFML(e);
    }
}

void App::draw_openGL() {
    m_map_mesh_builder->regenerate_edited_chunk_vertices();
    m_map_mesh_builder->add_new_chunks2rendering();
}

void App::run() {
    handle_events();
    m_map_mesh_builder->launch(m_map.get(), &m_player, &m_window);
    while (m_window.isOpen()) {
        m_debug_data.start();
        //
        m_map_mesh_builder->m_mutex__chunks4rendering.lock();
        for (auto &e : m_map->m_should_be_freed_buffers) {
            glDeleteVertexArrays(1, &e.VAO);
            glDeleteBuffers(1, &e.VBO);
        }
        m_map->m_should_be_freed_buffers.clear();
        m_map_mesh_builder->m_mutex__chunks4rendering.unlock();
        //
        update();

        draw_openGL();
        draw_SFML();

        m_renderer->finish_render(m_window, m_player, m_map_mesh_builder->m_chunks4rendering,
                                  m_map_mesh_builder->m_mutex__chunks4rendering);

        if (m_should_display_debug_info) {
            m_debug_data.count();
        }
    }

    m_map_mesh_builder->wait();
}

void App::handle_events() {
    input();
}

void App::input() {
    if (!m_window.hasFocus()) {
        m_should_fix_cursor = false;
    }

    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                m_window.close();
                break;
            case sf::Event::KeyPressed:
                process_key4window(event.key.code);
                break;
            case sf::Event::Resized:
                resize(event.size.width, event.size.height);
                break;
        }

        if (!m_should_fix_cursor)
            return;

        m_input_manager->handle_input(event);
    }
}

void App::resize(uint32_t x, uint32_t y) {
    sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(x),
                              static_cast<float>(y));
    m_window.setView(sf::View(visibleArea));
    m_window.setSize({x, y});
    m_menu->update();
}

void App::process_key4window(sf::Keyboard::Key code) {
    switch (code) {
        case sf::Keyboard::Escape:
            m_window.setMouseCursorVisible(m_should_fix_cursor);
            m_should_fix_cursor = !m_should_fix_cursor;
            m_input_manager->set_should_fix_cursor(m_should_fix_cursor);
            break;
        case sf::Keyboard::F3:
            m_should_display_debug_info = !m_should_display_debug_info;
            break;
    }
}

void App::update() {
    static sf::Clock timer;

    handle_events();
    m_player.update(timer.restart().asSeconds());
}

App::~App() {

}
