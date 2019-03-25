#include <cmath>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>
#include "Renderer.h"
#include "DebugData.h"
#include "World.h"
#include "Block.h"
#include "App.h"

using std::to_string;

App::App(sf::RenderWindow &window)
	: m_window {window}
{
}

App::~App()
{
}

void App::run()
{
	World world;
	m_player.init(&world);
	m_player.god_on();
	Renderer renderer;
	//m_window.setMouseCursorVisible(false);

	// text
	sf::Text text;
	sf::Font font;
	font.loadFromFile("arial.ttf");
	text.setFont(font);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::Blue);


	Block grass_block(&world);
	GLuint world_list[SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE];
	for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
		for (int j = 0; j < SUPER_CHUNK_SIZE; ++j) {
			for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
				world_list[i][j][k] = glGenLists(i + j * SUPER_CHUNK_SIZE + k * SUPER_CHUNK_SIZE * SUPER_CHUNK_SIZE+1);
				glNewList(world_list[i][j][k], GL_COMPILE);

				for (auto it = world.m_world[i][j][k].chunk().begin(); it != world.m_world[i][j][k].chunk().end(); ++it) {
					grass_block.bind_textures(
						sf::Vector3f(it->second.x + i * CHUNK_SIZE + 0.5, it->second.y + j * CHUNK_SIZE+0.5, it->second.z + k * CHUNK_SIZE+0.5));
				}

				glEndList();
			}
		}
	}


	sf::Clock clock;
	DebugData debug_data;
	while (m_window.isOpen())
	{
		handle_events();
		update(clock);

		// draw
		if (m_debug_info) {
			debug_data.start();

			text.setString(
				"fps: " +
				to_string(int(debug_data.fps)) + "\n" +
				"ft: " +
				to_string(int(debug_data.frame_time)) + "\n" +
				"x, y, z: " +
				to_string(m_player.get_position().x) + " " +
				to_string(m_player.get_position().y) + " " +
				to_string(m_player.get_position().z)
			);
			renderer.draw_SFML(text);
		}

		if (world.m_redraw_chunk) {
			
			sf::Vector3i c = world.m_edited_chunk_coord;
			glDeleteLists(world_list[c.x][c.y][c.z], 1);

			world_list[c.x][c.y][c.z] = glGenLists(c.x + c.y * SUPER_CHUNK_SIZE + c.z * SUPER_CHUNK_SIZE * SUPER_CHUNK_SIZE + 1);
			glNewList(world_list[c.x][c.y][c.z], GL_COMPILE);

			for (auto it = world.m_world[c.x][c.y][c.z].chunk().begin(); it != world.m_world[c.x][c.y][c.z].chunk().end(); ++it) {
				grass_block.bind_textures(
					sf::Vector3f(it->second.x + c.x * CHUNK_SIZE + 0.5, it->second.y + c.y * CHUNK_SIZE + 0.5, it->second.z + c.z * CHUNK_SIZE + 0.5));
			}

			glEndList();

			world.m_redraw_chunk = false;
		}

		for (int i = 0; i < SUPER_CHUNK_SIZE; ++i) {
			for (int j = 0; j < SUPER_CHUNK_SIZE; ++j) {
				for (int k = 0; k < SUPER_CHUNK_SIZE; ++k) {
					renderer.draw_chunk_gl_list(world_list[i][j][k]);
				}
			}
		}

		renderer.finish_render(m_window, m_player);

		if (m_debug_info) {
			debug_data.count();
		}
	}
}

void App::handle_events()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window.close();
		if (event.type == sf::Event::KeyPressed) {
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
		}
		m_player.input(event);
	}


	//camera
	sf::Vector2i mouse_xy;
	if (m_handle_cursor) {
		// get global mouse position
		mouse_xy = sf::Mouse::getPosition(m_window);
		
		// center coordinates
		int x = WINDOW_WIDTH / 2;
		int y = WINDOW_HEIGTH / 2+5;

		m_player.m_camera_angle.x += float(x - mouse_xy.x) / 8;
		m_player.m_camera_angle.y += float(y - mouse_xy.y) / 8;

		if (m_player.m_camera_angle.y < -88) { m_player.m_camera_angle.y = -88; }
		if (m_player.m_camera_angle.y > 88) { m_player.m_camera_angle.y = 88; }

		sf::Mouse::setPosition(sf::Vector2i(x, y), m_window);
	}
}

void App::update(sf::Clock &timer)
{
	float time = timer.getElapsedTime().asMilliseconds();
	timer.restart();
	time = time / 50;
	if (time > 3) time = 3;

	m_player.update(time);
}
