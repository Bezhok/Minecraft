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

unordered_map < size_t, std::vector < std::pair<size_t, Chunk*> >> m_update_order;
unordered_map<size_t, Chunk*> local_update_order;
sf::Mutex mutex;


void App::update_gllist(
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

		/*
		for (auto &e : m_world_list) {
			bool should_delete = true;
				
			for (auto &xz : new_chunks_set) {
				//if (!should_delete) break;
				
					//string coord_str = to_string(e.first) + string(" ") + to_string(j) + string(" ") + to_string(e.second);
					//std::hash<string> hash_fn;
					//size_t str_hash = hash_fn(coord_str);

					//bool is_founded = m_world_list.find(str_hash) != m_world_list.end();

					//if (m_map.get_chunk(e.first, j, e.second).chunk().size() && is_founded) {

					//	m_map.m_free_vbo_chunks.insert(&m_map.get_chunk(e.first, j, e.second));

					//	m_world_list.erase(str_hash);

					//	//m_update_order.erase(str_hash);
					//}

					if (e.second.second.x == xz.first && e.second.second.z == xz.second) {
						should_delete = false;
						break;
					}
				
			}
			if (should_delete) {
				for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {
					string coord_str = to_string(e.second.second.x) + string(" ") + to_string(j) + string(" ") + to_string(e.second.second.z);
					std::hash<string> hash_fn;
					size_t str_hash = hash_fn(coord_str);

					bool is_founded = m_world_list.find(str_hash) != m_world_list.end();
					if (is_founded) {
						m_map.m_free_vbo_chunks.insert(&m_map.get_chunk(e.second.second.x, j, e.second.second.z));

						m_world_list.erase(str_hash);
					}
				}
			}
			auto prev = e.second;
		}*/


				for (auto it = m_world_list.begin(); it != m_world_list.end();) {
					if (it->second.second.x < new_sx || it->second.second.z < new_sz || it->second.second.x > new_ex || it->second.second.z > new_ez) {
						m_map.m_free_vbo_chunks.insert(&m_map.get_chunk(it->second.second.x, it->second.second.y, it->second.second.z));

						it = m_world_list.erase(it);
					}
					else
						it++;
				}


		// and render it
		vector<pair<int, int>> diff;
		set_difference(new_chunks_set.begin(), new_chunks_set.end(), old_chunks_set.begin(), old_chunks_set.end(), back_inserter(diff));

		//mutex.lock();
		for (auto &e : diff) {
			for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {

				string coord_str = to_string(e.first) + string(" ") + to_string(j) + string(" ") + to_string(e.second);
				std::hash<string> hash_fn;
				size_t str_hash = hash_fn(coord_str);

				bool is_founded = m_world_list.find(str_hash) != m_world_list.end();
				if (m_map.get_chunk(e.first, j, e.second).chunk().size() && !is_founded) {

					m_world_list.erase(str_hash);

					string coord_str = to_string(e.first) + string(" ") + to_string(e.second);
					std::hash<string> hash_fn;
					size_t str_hash2 = hash_fn(coord_str);


					m_update_order[str_hash2].push_back({ str_hash, &m_map.get_chunk(e.first, j, e.second) });// = &m_map.get_chunk(e.first, j, e.second);

					//m_map.get_chunk(e.first, j, e.second).update(m_map);
					//m_world_list[str_hash] = { m_map.get_chunk(e.first, j, e.second).VAO, {e.first, j, e.second} };
				}
			}
		}
		//mutex.unlock();
	}
}

void App::create_gllist(const sf::Vector3i &c, size_t hash)
{

}
	

void App::create_all_gllists()
{

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
			to_string(m_player.get_position().z) + "\n" +
			to_string(m_world_list.size()) + " " +
			to_string(m_map.m_free_vbo_chunks.size()) + " " +
			to_string(m_update_order.size()) + " " +
			to_string(m_map.m_global_vao_vbo_buffers.size()) + "\n"
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
	for (auto &e: m_world_list)
		m_renderer.draw_chunk_gl_list(e.second);
}
#include <stack>
std::stack<std::pair<size_t, Chunk*>> update_vao_chunks;
//#include <iostream>
void App::update_vao_list()
{
	//sf::Context context;
	while (m_window.isOpen()) {
		mutex.lock();
		auto local_update_order = m_update_order;
		mutex.unlock();

		while (local_update_order.size()) {
			
			auto it = local_update_order.begin();
			auto column_chunks = it->second;
			
			//int i = local_update_order.begin()->second->get_pos().x;
			//int k = local_update_order.begin()->second->get_pos().z;

			//unordered_map<size_t, Chunk*> column_chunks;
			
			for (auto &e : column_chunks) {
				//if (i == e.second->get_pos().x && k == e.second->get_pos().z) {
				//	e.second->update(m_map);

				//	column_chunks.insert(e);
				//	local_update_order.erase(e.first);
				//}

				e.second->update(m_map);
			}



			mutex.lock();

			for (auto &e : column_chunks) {
				
				update_vao_chunks.push(e);
			}

			m_update_order.erase(it->first);
			mutex.unlock();

			local_update_order.erase(it);
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
				std::hash<string> hash_fn;
				size_t str_hash = hash_fn(coord_str);

				bool is_founded = m_world_list.find(str_hash) != m_world_list.end();
				if (m_map.get_chunk(i, j, k).chunk().size() && !is_founded) {

					m_map.get_chunk(i, j, k).update(m_map);
					m_map.get_chunk(i, j, k).upate_vao();
					m_world_list[str_hash] = { m_map.get_chunk(i, j, k).VAO, {i,j,k} };
				}
			}
		}
	}

	sf::Thread vbo_thread(&App::update_vao_list, this);
	vbo_thread.launch();

	int current_chunk_x;
	int current_chunk_z;
	int prev_chunk_x = -1;
	int prev_chunk_z = -1;

	//sf::Context context;
	//m_window.setActive(true);
	//m_window.setActive(true);
	

	while (m_window.isOpen())
	{
		
		//m_window.setActive(true);
		m_debug_data.start();
		
		// update
		handle_events();
		update(clock);

		if (m_map.is_chunk_edited()) {
			const sf::Vector3i &chunk_pos = m_map.get_edited_chunk_pos();

			int i = chunk_pos.x;
			int j = chunk_pos.y;
			int k = chunk_pos.z;

			//update
			string coord_str = to_string(i) + string(" ") + to_string(j) + string(" ") + to_string(k);
			std::hash<string> hash_fn;
			size_t str_hash = hash_fn(coord_str);
			

			// if buffer wasn't generated
			if (m_map.get_chunk(i, j, k).VAO && m_map.get_chunk(i, j, k).VBO) {
				m_map.m_free_vbo_chunks.insert(&m_map.get_chunk(i, j, k));
			}

			m_map.get_chunk(i, j, k).update(m_map);
			m_map.get_chunk(i, j, k).upate_vao();
			//TODO
			m_world_list[str_hash] = { m_map.get_chunk(i, j, k).VAO , {i, j, k} };

			m_map.cancel_chunk_editing_state();
		}



		current_chunk_x = m_player.get_position().x / BLOCK_SIZE / CHUNK_SIZE;
		current_chunk_z = m_player.get_position().z / BLOCK_SIZE / CHUNK_SIZE;

		

		mutex.lock();
		while (update_vao_chunks.size()) {

			auto &e = update_vao_chunks.top();

			e.second->upate_vao();
			m_world_list[e.first] = { e.second->VAO, e.second->get_pos() };

			update_vao_chunks.pop();
		}

		if (prev_chunk_x != current_chunk_x || prev_chunk_z != current_chunk_z) {
			update_gllist(prev_chunk_x, prev_chunk_z, current_chunk_x, current_chunk_z);
		}



		// draw
		draw_SFML();
		draw_openGL();

		
		m_renderer.finish_render(m_window, m_player, m_map);


		mutex.unlock();

		prev_chunk_x = current_chunk_x;
		prev_chunk_z = current_chunk_z;

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
	if (!m_window.hasFocus())
		m_handle_cursor = false;

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
			m_window.setSize({ event.size.width, event.size.height });
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
