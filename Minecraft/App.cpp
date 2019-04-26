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
using std::hash;
using std::pair;
using std::set_difference;
using std::back_inserter;
using std::to_string;
using namespace World;

const int RENDER_DISTANCE_CHUNKS = 5;

void App::update_gllist(
	Block &block,
	unordered_map<size_t, GLuint> &world_list,
	int old_chunk_x,
	int old_chunk_z,
	int new_chunk_x,
	int new_chunk_z
)
{
	// old
	int old_sx = old_chunk_x - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : old_chunk_x - RENDER_DISTANCE_CHUNKS / 2;
	int old_sz = old_chunk_z - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : old_chunk_z - RENDER_DISTANCE_CHUNKS / 2;

	int old_ex = old_chunk_x + RENDER_DISTANCE_CHUNKS / 2 > SUPER_CHUNK_SIZE ? SUPER_CHUNK_SIZE : old_chunk_x + RENDER_DISTANCE_CHUNKS / 2;
	int old_ez = old_chunk_z + RENDER_DISTANCE_CHUNKS / 2 > SUPER_CHUNK_SIZE ? SUPER_CHUNK_SIZE : old_chunk_z + RENDER_DISTANCE_CHUNKS / 2;

	//new
	int new_sx = new_chunk_x - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : new_chunk_x - RENDER_DISTANCE_CHUNKS / 2;
	int new_sz = new_chunk_z - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : new_chunk_z - RENDER_DISTANCE_CHUNKS / 2;

	int new_ex = new_chunk_x + RENDER_DISTANCE_CHUNKS / 2 > SUPER_CHUNK_SIZE ? SUPER_CHUNK_SIZE : new_chunk_x + RENDER_DISTANCE_CHUNKS / 2;
	int new_ez = new_chunk_z + RENDER_DISTANCE_CHUNKS / 2 > SUPER_CHUNK_SIZE ? SUPER_CHUNK_SIZE : new_chunk_z + RENDER_DISTANCE_CHUNKS / 2;

	int x_diff = new_chunk_x - old_chunk_x;
	int z_diff = new_chunk_z - old_chunk_z;

	if (abs(x_diff) == 1 || abs(z_diff) == 1) {

		// old chunks for rendering
		vector<pair<int, int>> old_chunks_set;
		for (int oi = old_sx; oi < old_ex; ++oi) {
			for (int ok = old_sz; ok < old_ez; ++ok) {
				old_chunks_set.push_back({ oi, ok });
			}
		}

		////  new chunks for rendering
		vector<pair<int, int>> new_chunks_set;
		for (int i = new_sx; i < new_ex; ++i) {
			for (int k = new_sz; k < new_ez; ++k) {
				new_chunks_set.push_back({ i, k });
			}
		}

		vector<pair<int, int>> diff;
		// dont rednder it
		set_difference(old_chunks_set.begin(), old_chunks_set.end(), new_chunks_set.begin(), new_chunks_set.end(), back_inserter(diff));
		for (auto &e : diff) {
			for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
				string coord_str = to_string(e.first) + string(" ") + to_string(j) + string(" ") + to_string(e.second);
				hash<string> hash_fn;
				size_t str_hash = hash_fn(coord_str);

				
				glDeleteLists(world_list[str_hash], 1);
				world_list.erase(str_hash);
			}
		}

		diff.clear();
		// and render it
		set_difference(new_chunks_set.begin(), new_chunks_set.end(), old_chunks_set.begin(), old_chunks_set.end(), back_inserter(diff));
		for (auto &e : diff) {
			for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
				string coord_str = to_string(e.first) + string(" ") + to_string(j) + string(" ") + to_string(e.second);
				hash<string> hash_fn;
				size_t str_hash = hash_fn(coord_str);

				world_list.insert(std::pair<size_t, GLuint>(str_hash, glGenLists(1)));
				glNewList(world_list[str_hash], GL_COMPILE);


				for (auto it = m_map.m_world->at(e.first)[j][e.second].chunk().begin(); it != m_map.m_world->at(e.first)[j][e.second].chunk().end(); ++it) {
					block.bind_textures(
						it->second.id,
						sf::Vector3f(
							it->second.x + e.first * CHUNK_SIZE + 0.5F,
							it->second.y + j * CHUNK_SIZE + 0.5F,
							it->second.z + e.second * CHUNK_SIZE + 0.5F
						)
					);
				}

				glEndList();
			}
		}
	}
}


void App::recreate_gllist(Block &block, unordered_map<size_t, GLuint> &world_list)
{
	for (auto &e : world_list) {
		glDeleteLists(e.second, 1);
	}

	world_list.clear();

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
				string coord_str = to_string(i) + string(" ") + to_string(j) + string(" ") + to_string(k);
				hash<string> hash_fn;
				size_t str_hash = hash_fn(coord_str);

				world_list.insert(std::pair<size_t, GLuint>(str_hash, glGenLists(1)));
				glNewList(world_list[str_hash], GL_COMPILE);


				for (auto it = m_map.m_world->at(i)[j][k].chunk().begin(); it != m_map.m_world->at(i)[j][k].chunk().end(); ++it) {
					block.bind_textures(
						it->second.id,
						sf::Vector3f(
							it->second.x + i * CHUNK_SIZE + 0.5F,
							it->second.y + j * CHUNK_SIZE + 0.5F,
							it->second.z + k * CHUNK_SIZE + 0.5F
						)
					);
				}

				glEndList();
			}
		}
	}
}

App::App(sf::RenderWindow &window)
	: m_window {window}
{
}

App::~App()
{
}

Menu *menu_ref;

void App::run()
{
	Renderer renderer;
	//m_window.setMouseCursorVisible(false);

	// text
	sf::Text text;
	sf::Font font;
	font.loadFromFile("resources/arial.ttf");
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::Color(sf::Color(0, 0, 0, 200)));

	DB db; db.load_blocks();
	// after !!!
	m_player.init(&m_map);
	m_player.god_on();
	m_player.flight_on();

	Menu menu(m_window);
	menu.update_players_blocks(m_player);
	menu_ref = &menu;

	Block block(&m_map);

	unordered_map<size_t, GLuint> world_list;

	//recreate_gllist(block, world_list);

	sf::Texture tex;
	sf::Sprite spr;

	tex.loadFromFile("resources/textures/gui/tool_bar.png");
	spr.setTexture(tex);

	spr.setPosition(WINDOW_WIDTH / 2.F, WINDOW_HEIGTH / 2.F);

	sf::Clock clock;
	DebugData debug_data;

	recreate_gllist(block, world_list);

	int current_chunk_x;
	int current_chunk_z;
	int prev_chunk_x = -1;
	int prev_chunk_z = -1;

	while (m_window.isOpen())
	{
		handle_events();
		update(clock);

		current_chunk_x = m_player.get_position().x / BLOCK_SIZE / CHUNK_SIZE;
		current_chunk_z = m_player.get_position().z / BLOCK_SIZE / CHUNK_SIZE;



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

		for (auto &e : menu.m_sprites) {
			renderer.draw_SFML(e.second);
		}

		for (auto &e : menu.m_side_sprites) {
			renderer.draw_SFML(e.second);
		}


		if (m_map.m_redraw_chunk) {
			
			sf::Vector3i c = m_map.m_edited_chunk_coord;

			string coord_str = to_string(c.x) + string(" ") + to_string(c.y) + string(" ") + to_string(c.z);
			hash<string> hash_fn;
			size_t str_hash = hash_fn(coord_str);

			glDeleteLists(world_list[str_hash], 1);
			world_list.erase(str_hash);
			world_list.insert(std::pair<size_t, GLuint>(str_hash, glGenLists(1)));
			glNewList(world_list[str_hash], GL_COMPILE);

			for (auto it = m_map.m_world->at(c.x)[c.y][c.z].chunk().begin(); it != m_map.m_world->at(c.x)[c.y][c.z].chunk().end(); ++it) {
				block.bind_textures(
					it->second.id,
					sf::Vector3f(
						it->second.x + c.x * CHUNK_SIZE + 0.5F,
						it->second.y + c.y * CHUNK_SIZE + 0.5F,
						it->second.z + c.z * CHUNK_SIZE + 0.5F
					)
				);
			}

			glEndList();

			m_map.m_redraw_chunk = false;
		}

		if (prev_chunk_x != current_chunk_x || prev_chunk_z != current_chunk_z) {
			update_gllist(block, world_list, prev_chunk_x, prev_chunk_z, current_chunk_x, current_chunk_z);
		}

		prev_chunk_x = current_chunk_x;
		prev_chunk_z = current_chunk_z;

		for (auto &e : world_list) {
			if (e.second)
			renderer.draw_chunk_gl_list(e.second);
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

		if (event.key.code == sf::Keyboard::RControl && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			m_debug_info = !m_debug_info;
		}

		static bool player_input_ability = true;


		//if (event.type == sf::Event::KeyPressed )
		//{
		//	if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
		//		sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
		//		sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		//	{
		//		player_input_ability = false;
		//	}
		//}

		//if (event.type == sf::Event::KeyReleased &&
		//	event.key.code == sf::Keyboard::S )
		//{
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
		//		sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
		//	{

		//		m_debug_info = !m_debug_info;
		//		player_input_ability = true;



		//	}
		//}

		// save
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
		{
			player_input_ability = false;
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::S) {
					m_debug_info = !m_debug_info;
					m_map.save();
					player_input_ability = true;
				}
				else if (event.key.code == sf::Keyboard::L) {
					m_debug_info = !m_debug_info;
					m_map.load();
					player_input_ability = true;
				}
			}
		}
		else {
			player_input_ability = true;
		}


		if (player_input_ability) {
			m_player.input(event);
			menu_ref->input(event);
		}


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
	int time = timer.getElapsedTime().asMilliseconds();
	timer.restart();
	//time = time / 50.F;
	//if (time > 3) time = 3;

	m_player.update(time / 50.F);
}
