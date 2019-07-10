#include "pch.h"
#include "Renderer.h"
#include "DebugData.h"
#include "Map.h"
#include "App.h"
#include "block_db.h"
#include "Menu.h"

using std::to_string;
using std::advance;
using std::make_shared;
using std::copy;
using std::inserter;
using std::fabsf;
using std::sqrtf;
using namespace World;


App::App(sf::RenderWindow& window)
	: m_window{ window }
{
	m_font.loadFromFile("resources/arial.ttf");
	m_text.setFont(m_font);
	m_text.setCharacterSize(20);
	m_text.setFillColor(sf::Color::Color(sf::Color(0, 0, 0, 200)));


	m_map = make_shared<World::Map>();
	//m_window.setMouseCursorVisible(false);
}

void App::draw_SFML()
{
	 //draw sfml
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
			to_string(m_map->m_free_vbo_chunks.size()) + " " +
			to_string(m_chunks4updating.size()) + " " +
			to_string(m_map->m_global_vao_vbo_buffers.size()) + " " +
			"- chunks rendering, local buffer, updates, global buffer\n" +

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
	static int counter = 0;

	if (m_map->is_chunk_edited()) {
		m_mutex__chunks4vbo_generation.lock();
		const sf::Vector3i& chunk_pos = m_map->get_edited_chunk_pos();

		int i = chunk_pos.x;
		int j = chunk_pos.y;
		int k = chunk_pos.z;


		Chunk& chunk = m_map->get_chunk(i, j, k);

		if (chunk.is_init()) {
			chunk.set_is_rendering(true);
			m_map->m_free_vbo_chunks.insert(&chunk);
			chunk.update_vertices();
			chunk.upate_vao();
			//TODO
			m_chunks4rendering.insert(&chunk);
		}
		else {
			chunk.init(chunk_pos, m_map.get());
			chunk.set_is_rendering(true);


			chunk.update_vertices();
			chunk.upate_vao();
			//TODO
			m_chunks4rendering.insert(&chunk);
		}



		/*
		for (sf::Vector3i& pos : m_map->m_should_be_updated_neighbours) {

			if (m_map->save_in_temp_column(pos.x, pos.z)) {
				Chunk& chunk = m_map->get_temp_column()[pos.y];

				m_map->m_free_vbo_chunks.insert(&chunk);
				chunk.update_vertices(*m_map);
				chunk.upate_vao();
			}
		}
		*/
		m_map->cancel_chunk_editing_state();

		m_mutex__chunks4vbo_generation.unlock();
	}



	++counter;
	if (counter > 20) {

		m_mutex__chunks4vbo_generation.lock();

		copy(
			m_chunks4vbo_generation.begin(),
			m_chunks4vbo_generation.end(),
			inserter(m_chunks4rendering, m_chunks4rendering.end())
		);

		for (auto& chunk : m_chunks4vbo_generation) {

			chunk->upate_vao();
		}

		m_chunks4vbo_generation.clear();

		m_mutex__chunks4vbo_generation.unlock();


		int new_chunk_x = Map::coord2chunk_coord(m_player.get_position().x);
		int new_chunk_z = Map::coord2chunk_coord(m_player.get_position().z);


		int new_sx = new_chunk_x - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : new_chunk_x - RENDER_DISTANCE_CHUNKS / 2;
		int new_sz = new_chunk_z - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : new_chunk_z - RENDER_DISTANCE_CHUNKS / 2;

		int new_ex = new_chunk_x + RENDER_DISTANCE_CHUNKS / 2;
		int new_ez = new_chunk_z + RENDER_DISTANCE_CHUNKS / 2;

		m_mutex__chunks4vbo_generation.lock();
		for (auto it = m_chunks4rendering.begin(); it != m_chunks4rendering.end();) {
			Chunk& chunk = *(*it);
			auto& pos = chunk.get_pos();
			if (pos.x < new_sx || pos.z < new_sz || pos.x > new_ex || pos.z > new_ez) {
				chunk.set_is_rendering(false);
				if (chunk.get_VAO()) {
					m_map->m_free_vbo_chunks.insert(&chunk);
				}

				it = m_chunks4rendering.erase(it);
			}
			else {
				it++;
			}
		}
		m_mutex__chunks4vbo_generation.unlock();

		counter = 0;
	}
}

#include <iostream>
void App::generate_verticies()
{
	int center_index;
	sf::Clock timer;
	sf::Clock timer2;

	sf::Thread second_thread([&]() {
		auto end = m_chunks4updating.begin();
		advance(end, center_index);

		for (auto it = m_chunks4updating.begin(); it != end; ++it) {
			(*it)->update_vertices();
			m_mutex__chunks4vbo_generation.lock();
			m_chunks4vbo_generation.push_back(*it);
			m_mutex__chunks4vbo_generation.unlock();
		}
	});

	const float SPHERE_DIAMETER = sqrtf(3 * COORDS_IN_BLOCK*COORDS_IN_BLOCK*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK);

	while (m_window.isOpen()) {
		timer2.restart();
		int new_chunk_x = Map::coord2chunk_coord(m_player.get_position().x);
		int new_chunk_z = Map::coord2chunk_coord(m_player.get_position().z);

		//new
		int new_sx = new_chunk_x - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : new_chunk_x - RENDER_DISTANCE_CHUNKS / 2;
		int new_sz = new_chunk_z - RENDER_DISTANCE_CHUNKS / 2 < 0 ? 0 : new_chunk_z - RENDER_DISTANCE_CHUNKS / 2;

		int new_ex = new_chunk_x + RENDER_DISTANCE_CHUNKS / 2;
		int new_ez = new_chunk_z + RENDER_DISTANCE_CHUNKS / 2;



		static glm::mat4 view;
		static glm::mat4 projection;
		static glm::vec4 norm_coords;

		projection = glm::perspective(45.0f, (GLfloat)m_window.getSize().x / (GLfloat)m_window.getSize().y, 0.1f, RENDER_DISTANCE);
		view = glm::lookAt(
			glm::vec3(
				m_player.get_position().x,
				m_player.get_position().y + COORDS_IN_BLOCK * 0.8f,//m_size.y
				m_player.get_position().z
			),
			glm::vec3(
				m_player.get_position().x - sin(m_player.m_camera_angle.x / 180 * PI),
				m_player.get_position().y + COORDS_IN_BLOCK * 0.8f + tan(m_player.m_camera_angle.y / 180 * PI),
				m_player.get_position().z - cos(m_player.m_camera_angle.x / 180 * PI)
			),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		int VISIBLE_COLUMNS_PER_LOOP = 20;
		int visible_columns_count = 0;


		for (int i = new_sx; i < new_ex; ++i) {
			if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
				break;
			for (int k = new_sz; k < new_ez; ++k) {
				if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
					break;
				for (int j = 0; j < CHUNKS_IN_WORLD_HEIGHT; ++j) {
					norm_coords = (projection * view) * glm::vec4(i * BLOCKS_IN_CHUNK, j * BLOCKS_IN_CHUNK, k * BLOCKS_IN_CHUNK, 1.F);
					norm_coords.x /= norm_coords.w;
					norm_coords.y /= norm_coords.w;

					bool is_chunk_visible = norm_coords.z >= -1 * SPHERE_DIAMETER / fabsf(norm_coords.w)
						&& fabsf(norm_coords.x) <= 1 + 1 * SPHERE_DIAMETER / fabsf(norm_coords.w)
						&& fabsf(norm_coords.y) <= 1 + 1 * SPHERE_DIAMETER / fabsf(norm_coords.w);
					//TODO invalid "rendering sphere"

					
						auto column_ptr = m_map->get_column_ptr(i, k);
						if (column_ptr != nullptr) {
							auto& column = *column_ptr;

							if (is_chunk_visible && !column[j].is_rendering() && column[j].is_init()) {
								bool is_new = false;
								for (int y = 0; y < CHUNKS_IN_WORLD_HEIGHT; ++y) {


									Chunk& chunk = column[y];
									//CRITICAL ERROR
									if (!chunk.is_empty() && !chunk.is_vertices_created() && !chunk.is_rendering() && chunk.is_init()) {
										is_new = true;
										chunk.set_is_rendering(true);
										m_chunks4updating.push_back(&chunk);
									}

								}

								if (is_new)
									++visible_columns_count;
								break;
							}
						}
						else {
							//TODO load from disk
						}
				}
			}
		}

		size_t order_size = m_chunks4updating.size();
		center_index =  order_size / 2;

		second_thread.launch();

		auto it = m_chunks4updating.begin();

		advance(it, center_index);

		int inftest = 0;
		for (; it != m_chunks4updating.end(); ++it) {
			timer.restart();
			(*it)->update_vertices();
			int time = timer.getElapsedTime().asMilliseconds();
			int verticies_count = (*it)->get_points_count()/3*2/4;
			std::cout << time << ' ' << verticies_count << std::endl;
			inftest += time;


			m_mutex__chunks4vbo_generation.lock();
			m_chunks4vbo_generation.push_back(*it);
			m_mutex__chunks4vbo_generation.unlock();
		}

		std::cout << "******************* " << inftest << std::endl;

		second_thread.wait();


		m_chunks4updating.clear();

		std::cout << "--- " << timer2.getElapsedTime().asMilliseconds() << std::endl;
	}
}

void App::run()
{
	DB db; db.load_blocks();
	// after !!!
	m_player.init(m_map.get());
	m_player.god_on();
	m_player.flight_on();

	Menu menu(m_window);
	menu.update_players_blocks(m_player);
	m_menu = &menu;
	m_map->m_mutex__chunks4vbo_generation = &m_mutex__chunks4vbo_generation;

	

	sf::Thread verticies_generator_thread(&App::generate_verticies, this);
	verticies_generator_thread.launch();
	while (m_window.isOpen())
	{
		m_debug_data.start();


		update();

		draw_openGL();
		draw_SFML();
		m_renderer.finish_render(m_window, m_player, *m_map, m_chunks4rendering, m_mutex__chunks4rendering);

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

			sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
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
					//m_map->save();
					input_ability = true;
				}
				else if (event.key.code == sf::Keyboard::L) {
					m_debug_info = !m_debug_info;
					//m_map->load();
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

void App::update()
{
	static sf::Clock timer;

	handle_events();
	m_player.update(timer.restart().asSeconds()*25);
}
