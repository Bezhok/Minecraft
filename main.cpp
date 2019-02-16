#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>
#include <string>
#include <iostream>
#include "Block.h"
#include "game_constants.h"
#include "Player.h"
#include "SkyBox.h"
#include "Maths.h"
#include <unordered_map>
#include <vector>
using std::vector;
using std::string;
using std::to_string;
using std::unordered_map;

float translate_coordinate(int c) {
	return BLOCK_SIZE * c + BLOCK_SIZE / 2;
}

struct level {
	bool level[CHUNK_WIDTH][CHUNK_WIDTH];
};

struct Chunk {
	level chunks[CHUNK_HEIGHT];
};

Chunk chunk;

bool get_block(int mx, int my, int mz)
{
	if ((mx < 0) || (mx >= CHUNK_WIDTH) ||
		(my < 0) || (my >= CHUNK_HEIGHT) ||
		(mz < 0) || (mz >= CHUNK_WIDTH)) return false;
	return chunk.chunks[my].level[mx][mz];
}

int main()
{
	int disr = 200;
	srand(time(0));


			for (int i = 0; i < CHUNK_WIDTH; ++i) {
				for (int j = 0; j < CHUNK_HEIGHT; ++j) {
					for (int l = 0; l < CHUNK_WIDTH; ++l) {
						if (j == 0) {
							chunk.chunks[j].level[i][l] = true;
						}
					}
				}
			}

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Minecraft", sf::Style::Default, sf::ContextSettings(24));
	window.setVerticalSyncEnabled(true);

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();//reset matrix
	gluPerspective(90.f, 1.f, 1.f, RENDER_DISTANCE);
	glEnable(GL_TEXTURE_2D);

	ShowCursor(false);

	// text
	sf::Text text;
	sf::Font font;
	font.loadFromFile("arial.ttf");
	text.setFont(font);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::Blue);

	sf::Clock timer;
	sf::Clock timer1;
	sf::Clock clock;
	Player player;
	SkyBox sky_box;
	////
	Block grass_block;

	const int chunk_count = 25;
	GLuint world_list[chunk_count];
	for (int w = 0; w < chunk_count; ++w) {

		world_list[w] = glGenLists(w+1);
		glNewList(world_list[w], GL_COMPILE);
		for (int i = 0; i < CHUNK_WIDTH; ++i) {
			for (int j = 0; j < CHUNK_HEIGHT; ++j) {
				for (int k = 0; k < CHUNK_WIDTH; ++k) {
					if (get_block(i, j, k))
						grass_block.bind_textures(BLOCK_SIZE / 2, sf::Vector3f(i, j, k), sf::Vector3f(w, w, w));
				}
			}
		}

		glEndList();
	}

	////

	float last_time = 0;
	float fps = 0;
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMilliseconds();
		clock.restart();
		time = time / 50;
		if (time > 3) time = 3;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		player.input();
		player.update(time);

		////************************************************mouse//////
		POINT mouse_xy;
		GetCursorPos(&mouse_xy);
		// center coordinates
		int x = window.getPosition().x + WINDOW_WIDTH / 2;
		int y = window.getPosition().y + WINDOW_HEIGTH / 2;

		player.m_camera_angle.x += (x - mouse_xy.x)/8;
		player.m_camera_angle.y += (y - mouse_xy.y)/8;

		if (player.m_camera_angle.y < -89.) { player.m_camera_angle.y = -89.; }
		if (player.m_camera_angle.y > 89.) { player.m_camera_angle.y = 89.; }

		SetCursorPos(x, y);
		////*****************************************//////


		double current_time = timer1.getElapsedTime().asMilliseconds();
		double fps_timer = timer.getElapsedTime().asMilliseconds();



		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			player.get_position().x,
			player.get_position().y,
			player.get_position().z,
			player.get_position().x - sin(player.m_camera_angle.x / 180 * PI),
			player.get_position().y + tan(player.m_camera_angle.y / 180 * PI),
			player.get_position().z - cos(player.m_camera_angle.x / 180 * PI),
			0,
			1,
			0
		);

		// optimization, "inside"
		glEnable(GL_CULL_FACE);

		for (int w = 0; w < chunk_count; ++w) {
			glCallList(world_list[w]);
		}
		glDisable(GL_CULL_FACE);


		// debug info
		if (fps_timer >= 200) {
			timer.restart();

			fps = 1000.f / (current_time - last_time);
		}

		text.setString(sf::String(to_string(int(fps))));

		//skybox
		glTranslatef(player.get_position().x, player.get_position().y, player.get_position().z);
		sky_box.bind_textures(RENDER_DISTANCE / 2);
		glTranslatef(-player.get_position().x, -player.get_position().y, -player.get_position().z);

		window.pushGLStates();
		window.draw(text);
		window.popGLStates();

		glFlush();
		window.display(); // swap buffers

		last_time = current_time;
	}

	return 0;
}

