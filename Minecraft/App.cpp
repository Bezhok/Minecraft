#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Map.h"
#include "Block.h"
#include "App.h"
#include "block_db.h"
#include "Menu.h"

using std::unordered_map;
using std::string;
using std::pair;
using std::to_string;
using namespace World;

void App::update_gllist(Block & block, const sf::Vector3i &c)
{
	// c - chunk_pos
	size_t hash = c.x + (c.y * SUPER_CHUNK_SIZE_HEIGHT) + (c.z * SUPER_CHUNK_SIZE_HEIGHT * SUPER_CHUNK_SIZE);

	glDeleteLists(m_world_list[hash], 1);
	m_world_list.erase(hash);

	create_gllist(block, c, hash);
}

void App::create_gllist(Block &block, const sf::Vector3i &c, size_t hash)
{
	m_world_list.insert(std::pair<size_t, GLuint>(hash, glGenLists(1)));

	glNewList(m_world_list[hash], GL_COMPILE);

	for (const auto &e : m_map.get_chunk(c.x, c.y, c.z)) {
		block.bind_textures(
			e.second.id,
			sf::Vector3f(
				e.second.x + c.x * CHUNK_SIZE + 0.5F,
				e.second.y + c.y * CHUNK_SIZE + 0.5F,
				e.second.z + c.z * CHUNK_SIZE + 0.5F
			)
		);
	}
	//for (auto it = m_map.get_chunk(c.x, c.y, c.z).begin(); it != m_map.get_chunk(c.x, c.y, c.z).end(); ++it) {
	//	block.bind_textures(
	//		it->second.id,
	//		sf::Vector3f(
	//			it->second.x + c.x * CHUNK_SIZE + 0.5F,
	//			it->second.y + c.y * CHUNK_SIZE + 0.5F,
	//			it->second.z + c.z * CHUNK_SIZE + 0.5F
	//		)
	//	);
	//}

	glEndList();
}

void App::create_all_gllists(Block &block)
{
	for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
		for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
			for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
				size_t hash = i + (j * SUPER_CHUNK_SIZE_HEIGHT) + (k * SUPER_CHUNK_SIZE_HEIGHT * SUPER_CHUNK_SIZE);

				sf::Vector3i pos = {i, j, k};
				create_gllist(block, pos, hash);
			}
		}
	}
}

App::App(sf::RenderWindow &window)
	: m_window {window}
{
	m_font.loadFromFile("resources/arial.ttf");
	m_text.setFont(m_font);
	m_text.setCharacterSize(20);
	m_text.setFillColor(sf::Color::Color(sf::Color(0, 0, 0, 200)));

	//m_window.setMouseCursorVisible(false);
}

void App::draw_SFML()
{
	// draw sfml
	if (m_debug_info) {
		m_text.setString(
			"fps: " +
			to_string(int(m_debug_data.get_fps())) + "\n" +
			"ft: " +
			to_string(int(m_debug_data.get_frame_time())) + "\n" +
			"x, y, z: " +
			to_string(m_player.get_position().x) + " " +
			to_string(m_player.get_position().y) + " " +
			to_string(m_player.get_position().z)
		);
		m_renderer.draw_SFML(m_text);
	}

	for (auto &e : m_menu->get_spites()) {
		m_renderer.draw_SFML(e.second);
	}

	for (auto &e : m_menu->get_top_spites()) {
		m_renderer.draw_SFML(e.second);
	}
}

void App::draw_openGL()
{
	int sx, sz, ex, ez, cx, cz;// start x, end x, current x ...
	cx = m_player.get_position().x / BLOCK_SIZE / CHUNK_SIZE;
	cz = m_player.get_position().z / BLOCK_SIZE / CHUNK_SIZE;

	sx = cx - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : cx - RENDER_DISTANCE_CHUNKS / 2;
	sz = cz - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : cz - RENDER_DISTANCE_CHUNKS / 2;

	ex = cx + RENDER_DISTANCE_CHUNKS / 2 > SUPER_CHUNK_SIZE ? SUPER_CHUNK_SIZE : cx + RENDER_DISTANCE_CHUNKS / 2;
	ez = cz + RENDER_DISTANCE_CHUNKS / 2 > SUPER_CHUNK_SIZE ? SUPER_CHUNK_SIZE : cz + RENDER_DISTANCE_CHUNKS / 2;

	for (int i = sx; i < ex; ++i) {
		for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
			for (int k = sz; k < ez; ++k) {
				size_t hash = i + (j * SUPER_CHUNK_SIZE_HEIGHT) + (k * SUPER_CHUNK_SIZE_HEIGHT * SUPER_CHUNK_SIZE);
				if (m_world_list.find(hash) != m_world_list.end()) {
					m_renderer.draw_chunk_gl_list(m_world_list[hash]);
				}
			}
		}
	}
}



void App::run()
{
	DB db; db.load_blocks();
	// after !!!
	m_player.init(&m_map);
	m_player.god_on();
	m_player.flight_on();

	Menu menu(m_window);
	menu.update_players_blocks(m_player);
	m_menu = &menu;

	Block block(&m_map);

	sf::Clock clock;

	create_all_gllists(block);
	while (m_window.isOpen())
	{
		m_debug_data.start();

		// update
		handle_events();
		update(clock);

		if (m_map.is_chunk_edited()) {
			const sf::Vector3i &chunk_pos = m_map.get_edited_chunk_pos();
			update_gllist(block, chunk_pos);

			m_map.cancel_chunk_editing_state();
		}

		// draw
		draw_SFML();
		draw_openGL();
		m_renderer.finish_render(m_window, m_player);


		if (m_debug_info) {
			m_debug_data.count();
		}
	}
}

void App::handle_events()
{
	input();

	//camera
	sf::Vector2i mouse_xy;
	if (m_handle_cursor) {
		mouse_xy = sf::Mouse::getPosition(m_window);
		
		// center coordinates
		int x = m_window.getSize().x / 2;
		int y = m_window.getSize().y / 2;

		m_player.m_camera_angle.x += float(x - mouse_xy.x) / 8;
		m_player.m_camera_angle.y += float(y - mouse_xy.y) / 8;

		if (m_player.m_camera_angle.y < -88) { m_player.m_camera_angle.y = -88; }
		if (m_player.m_camera_angle.y > 88) { m_player.m_camera_angle.y = 88; }

		sf::Mouse::setPosition(sf::Vector2i(x, y), m_window);
	}
}

void App::input()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			m_window.close();
			break;
		case sf::Event::KeyPressed: {
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_handle_cursor = !m_handle_cursor;
				break;
			case sf::Keyboard::F3:
				m_debug_info = !m_debug_info;
				break;
			default:
				break;
			}
			break;
		}
		case sf::Event::Resized: {
			sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
			m_window.setView(sf::View(visibleArea));

			m_renderer.reset_view({ static_cast<float>(event.size.width), static_cast<float>(event.size.height) });
			m_menu->update();
			break;
		}
		}


		// block input while ctrl pressed
		static bool input_ability = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
		{
			input_ability = false;
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::S) {
					m_debug_info = !m_debug_info;
					m_map.save();
					input_ability = true;
				}
				else if (event.key.code == sf::Keyboard::L) {
					m_debug_info = !m_debug_info;
					m_map.load();
					input_ability = true;
				}
			}
		}
		else {
			input_ability = true;
		}

		if (input_ability) {
			m_player.input(event);
			m_menu->input(event);
		}
	}
}

void App::update(sf::Clock &timer)
{
	int time = timer.getElapsedTime().asMilliseconds();
	timer.restart();

	m_player.update(time / 50.F);
}
