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
	ShowCursor(false);

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

				for (auto it = world.m_world[i][j][k].get_chunk().begin(); it != world.m_world[i][j][k].get_chunk().end(); ++it) {
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
				to_string(int(debug_data.fps)) + " " +
				to_string(debug_data.frame_time) + "\n" +
				to_string(m_player.get_position().x) + " " +
				to_string(m_player.get_position().y) + " " +
				to_string(m_player.get_position().z)
			);
			renderer.draw_SFML(text);
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


	////camera//////
	POINT mouse_xy;
	if (m_handle_cursor) {
		GetCursorPos(&mouse_xy);
		// center coordinates
		int x = m_window.getPosition().x + WINDOW_WIDTH / 2;
		int y = m_window.getPosition().y + WINDOW_HEIGTH / 2;

		m_player.m_camera_angle.x += (x - mouse_xy.x) / 8;
		m_player.m_camera_angle.y += (y - mouse_xy.y) / 8;

		if (m_player.m_camera_angle.y < -89.9) { m_player.m_camera_angle.y = -89.9; }
		if (m_player.m_camera_angle.y > 89.9) { m_player.m_camera_angle.y = 89.9; }

		SetCursorPos(x, y);
		//////////
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
