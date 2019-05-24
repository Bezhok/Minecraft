#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Map.h"
#include "Block.h"
#include "App.h"
#include "block_db.h"
#include "Menu.h"
#include "Maths.h"

using std::unordered_map;
using std::string;
using std::pair;
using std::stack;
using std::pair;
using std::vector;
using std::to_string;
using namespace World;

// x, z hash, (VAO, Chunk*)
unordered_map<size_t, vector<pair<size_t, Chunk*>>> m_columns4updating;

// x, y, z hash, Chunk*
stack<pair<size_t, Chunk*>> m_chunks4vbo_generation;
sf::Mutex mutex, mutex_columns4updating;



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



		//
		for (auto it = m_chunks4rendering.begin(); it != m_chunks4rendering.end();) {
			auto &pos = it->second->get_pos();
			if (pos.x < new_sx || pos.z < new_sz || pos.x > new_ex || pos.z > new_ez) {

				Chunk *chunk = &m_map.get_chunk(pos.x, pos.y, pos.z);
				if (!chunk->is_empty()) {
					m_map.m_free_vbo_chunks.insert(chunk);
				}

				it = m_chunks4rendering.erase(it);
			}
			else
				it++;
		}


		// and render it
		vector<pair<int, int>> diff;
		set_difference(new_chunks_set.begin(), new_chunks_set.end(), old_chunks_set.begin(), old_chunks_set.end(), back_inserter(diff));

		//mutex_columns4updating.lock();
		for (auto& e : diff) {
			for (int j = 0; j < SUPER_CHUNK_SIZE_HEIGHT; ++j) {

				size_t hash_3d = calculate_3D_hash(e.first, j, e.second);

				bool is_founded = m_chunks4rendering.find(hash_3d) != m_chunks4rendering.end();
				if (!m_map.get_chunk(e.first, j, e.second).is_empty() && !is_founded) {

					m_chunks4rendering.erase(hash_3d);

					size_t hash_2d = calculate_2D_hash(e.first, e.second);

					m_columns4updating[hash_2d].push_back({ hash_3d,&m_map.get_chunk(e.first, j, e.second) });
				}
			}
		}
		//mutex_columns4updating.unlock();
	}
}


App::App(sf::RenderWindow& window)
	: m_window{ window }
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

			to_string(m_chunks4rendering.size()) + " " +
			to_string(m_map.m_free_vbo_chunks.size()) + " " +
			to_string(m_columns4updating.size()) + " " +
			to_string(m_map.m_global_vao_vbo_buffers.size()) + " " +
			"- chunks rendering, local buffer, updates(columns), global buffer\n" +

			to_string(verticies_wasnt_free) + " - chunks which veticies memory is not freed"
		);
		m_renderer.draw_SFML(m_text);
	}

	for (auto& e : m_menu->get_spites()) {
		m_renderer.draw_SFML(e.second);
	}

	for (auto& e : m_menu->get_top_spites()) {
		m_renderer.draw_SFML(e.second);
	}
}

void App::draw_openGL()
{
	//for (auto& e : m_chunks4rendering)
	//	m_renderer.draw_chunk(e.second);
}


void App::update_vao_list()
{
	//sf::Context context;
	while (m_window.isOpen()) {
		mutex.lock();
		auto local_update_order = m_columns4updating;
		mutex.unlock();

		while (local_update_order.size()) {

			auto it = local_update_order.begin();
			auto column_chunks = it->second;


			for (auto& e : column_chunks) {
				e.second->update_vertices(m_map);
			}


			mutex.lock();

			for (auto& e : column_chunks) {

				m_chunks4vbo_generation.push(e);
			}

			m_columns4updating.erase(it->first);
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
				size_t hash_3d = calculate_3D_hash(i, j, k);

				bool is_founded = m_chunks4rendering.find(hash_3d) != m_chunks4rendering.end();
				if (!m_map.get_chunk(i, j, k).is_empty() && !is_founded) {

					m_map.get_chunk(i, j, k).update_vertices(m_map);
					m_map.get_chunk(i, j, k).upate_vao();
					m_chunks4rendering[hash_3d] = &m_map.get_chunk(i, j, k);
				}
			}
		}
	}

	sf::Thread verticies_generator_thread(&App::update_vao_list, this);
	verticies_generator_thread.launch();

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
			const sf::Vector3i& chunk_pos = m_map.get_edited_chunk_pos();

			int i = chunk_pos.x;
			int j = chunk_pos.y;
			int k = chunk_pos.z;

			//update
			size_t hash_3d = calculate_3D_hash(i, j, k);


			// if buffer wasn't generated
			if (m_map.get_chunk(i, j, k).get_VAO()) {
				m_map.m_free_vbo_chunks.insert(&m_map.get_chunk(i, j, k));
			}

			m_map.get_chunk(i, j, k).update_vertices(m_map);
			m_map.get_chunk(i, j, k).upate_vao();
			//TODO
			m_chunks4rendering[hash_3d] = &m_map.get_chunk(i, j, k);

			m_map.cancel_chunk_editing_state();
		}



		current_chunk_x = m_player.get_position().x / BLOCK_SIZE / CHUNK_SIZE;
		current_chunk_z = m_player.get_position().z / BLOCK_SIZE / CHUNK_SIZE;



		mutex.lock();
		while (m_chunks4vbo_generation.size()) {

			auto& e = m_chunks4vbo_generation.top();

			e.second->upate_vao();
			m_chunks4rendering[e.first] = e.second;

			m_chunks4vbo_generation.pop();
		}

		if (prev_chunk_x != current_chunk_x || prev_chunk_z != current_chunk_z) {
			update_gllist(prev_chunk_x, prev_chunk_z, current_chunk_x, current_chunk_z);
		}
		// draw
		draw_SFML();
		draw_openGL();



		mutex.unlock();

		m_renderer.finish_render(m_window, m_player, m_map, m_chunks4rendering);








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

		if (m_player.m_camera_angle.y < -89) { m_player.m_camera_angle.y = -89; }
		if (m_player.m_camera_angle.y > 89) { m_player.m_camera_angle.y = 89; }

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

void App::update(sf::Clock& timer)
{
	int time = timer.getElapsedTime().asMilliseconds();
	timer.restart();

	m_player.update(time / 50.F);
}
