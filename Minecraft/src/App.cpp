#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Map.h"
#include "App.h"
#include "block_db.h"
#include "Menu.h"
#include "MapMeshBuilder.h"

using std::to_string;
using std::advance;
using std::make_shared;
using std::make_unique;
using std::copy;
using std::inserter;
using std::fabs;
using std::sqrt;
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

    m_window.setMouseCursorVisible(false);

    DB::load_blocks();
    // after !!!
    m_player.init(m_map.get());
    m_player.god_on();
    m_player.flight_on();

    m_menu = std::make_unique<Menu>(m_window);
    m_menu->update_players_blocks(m_player);
}

std::string App::debug_text() {
    std::string str =
            "fps: " +
          to_string(int(m_debug_data.get_fps())) + "\n" +
          "ft: " +
          to_string(int(m_debug_data.get_frame_time())) + "\n" +
          "x, y, z: " +
          to_string(m_player.get_position().x) + " " +
          to_string(m_player.get_position().y) + " " +
          to_string(m_player.get_position().z) + "\n" +
          to_string(m_map_mesh_builder->get_chunks4rendering_size()) + " " +
          to_string(m_map->get_size()) + " " +
          to_string(m_map_mesh_builder->get_chunks4vertices_generation_size()) + " " +
          "- chunks rendering, map size, updates, global buffer\n" +
          to_string(vertices_wasnt_free) + " - chunks which veticies memory is not freed";

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
        m_renderer->draw_SFML(e.second);
    }
}

void App::draw_openGL() {
    m_map_mesh_builder->regenerate_edited_chunk_vertices();
    m_map_mesh_builder->add_new_chunks2rendering();
}

void App::run() {


//	Sleep(1000.f);
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

    //camera
    sf::Vector2i mouse_xy;
    if (m_should_fix_cursor) {
        mouse_xy = sf::Mouse::getPosition(m_window);

        // center coordinates
        int x = m_window.getSize().x / 2;
        int y = m_window.getSize().y / 2;

        m_player.m_camera_angle.x += float(x - mouse_xy.x) / 12;
        m_player.m_camera_angle.y += float(y - mouse_xy.y) / 12;

        if (m_player.m_camera_angle.y < -89) { m_player.m_camera_angle.y = -89; }
        if (m_player.m_camera_angle.y > 89) { m_player.m_camera_angle.y = 89; }

        if (m_player.m_camera_angle.x > 361) {
            m_player.m_camera_angle.x -= 360;
        }

        if (m_player.m_camera_angle.x < -361) {
            m_player.m_camera_angle.x += 360;
        }

        sf::Mouse::setPosition(sf::Vector2i(x, y), m_window);
    }
}

void App::input() {
    if (!m_window.hasFocus())
        m_should_fix_cursor = false;

    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                m_window.close();
                break;
            case sf::Event::KeyPressed:
                process_key(event.key.code);
                break;
            case sf::Event::Resized:
                resize(event.size.width, event.size.height);
                break;
        }


        m_player.input(event);
        m_menu->input(event);
    }
}

void App::resize(uint32_t x, uint32_t y) {
    sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(x),
                              static_cast<float>(y));
    m_window.setView(sf::View(visibleArea));
    m_window.setSize({x, y});
    m_menu->update();
}

void App::process_key(sf::Keyboard::Key code) {
    switch (code) {
        case sf::Keyboard::Escape:
            m_window.setMouseCursorVisible(m_should_fix_cursor);
            m_should_fix_cursor = !m_should_fix_cursor;

            break;
        case sf::Keyboard::F3:
            m_should_display_debug_info = !m_should_display_debug_info;
            break;
        default:
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
