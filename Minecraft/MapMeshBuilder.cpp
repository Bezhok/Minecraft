#include "MapMeshBuilder.h"
#include "pch.h"
#include "Chunk.h"
#include "Map.h"
#include "Player.h"

using namespace World;
using std::copy;
using std::inserter;
using std::advance;

MapMeshBuilder::MapMeshBuilder() :
	m_verticies_generator_thread(&MapMeshBuilder::generate_verticies, this)
{

}

void World::MapMeshBuilder::launch(Map* map, Player* player, sf::Window* window)
{
	m_player = player;
	m_map = map;
	m_window = window;

	/////////////////////////////////////
	int chunk_x = Map::coord2chunk_coord(m_player->get_position().x);
	int chunk_z = Map::coord2chunk_coord(m_player->get_position().z);

	int start_x = std::max(0, chunk_x - 3);
	int start_z = std::max(0, chunk_z - 3);

	int end_x = chunk_x + 3;
	int end_z = chunk_z + 3;

	for (int i = start_x; i < end_x; ++i)
		for (int j = start_z; j < end_z; ++j) {
			m_map->get_column_or_generate(i, j);
		}
	/////////////////////////////////////////

	m_verticies_generator_thread.launch();
}

void MapMeshBuilder::generate_verticies()
{
	int center_index;
	sf::Clock verticies_gen_timer;
	sf::Clock loop_timer;

	RenderRange range;

	sf::Thread second_thread([&]() {
		auto end = m_chunks4verticies_generation.begin();
		advance(end, center_index);

		for (auto it = m_chunks4verticies_generation.begin(); it != end; ++it) {
			(*it)->update_vertices(m_mutex__chunks4vbo_generation);
			m_mutex__chunks4vbo_generation.lock();
			m_chunks4vbo_generation.push_back(*it);
			m_mutex__chunks4vbo_generation.unlock();
		}
	});


	while (m_window->isOpen()) {

		loop_timer.restart();


		range.chunk_x = Map::coord2chunk_coord(m_player->get_position().x);
		range.chunk_z = Map::coord2chunk_coord(m_player->get_position().z);

		range.start_x = range.chunk_x - RENDER_DISTANCE_IN_CHUNKS / 2;
		range.start_z = range.chunk_z - RENDER_DISTANCE_IN_CHUNKS / 2;

		range.end_x = range.chunk_x + RENDER_DISTANCE_IN_CHUNKS / 2;
		range.end_z = range.chunk_z + RENDER_DISTANCE_IN_CHUNKS / 2;

		//RenderRange terrain_generation_range = range;

		//terrain_generation_range.end_x += 3;
		//terrain_generation_range.end_z += 3;

		//terrain_generation_range.start_x -= 3;
		//terrain_generation_range.start_z -= 3;


		unload_columns(range);
		add_chunks2verticies_generation(range);


		size_t order_size = m_chunks4verticies_generation.size();
		center_index = order_size / 2;

		//second_thread.launch();

		auto it = m_chunks4verticies_generation.begin();
		//advance(it, center_index);

		float inftest = 0;
		for (; it != m_chunks4verticies_generation.end(); ++it) {

			verticies_gen_timer.restart();
			(*it)->update_vertices(m_mutex__chunks4vbo_generation);
			float time = verticies_gen_timer.getElapsedTime().asMicroseconds() / 1000.0f;
			int faces_count = (*it)->m_blocks_mesh.get_current_faces_count();
			//std::cout << std::setw(9) << time << "ms - " << std::setw(4) << faces_count << " faces" << std::endl;
			inftest += time;

			m_mutex__chunks4vbo_generation.lock();
			m_chunks4vbo_generation.push_back(*it);
			m_mutex__chunks4vbo_generation.unlock();
		}

		//std::cout << "verticies generation time " << inftest << std::endl;

		//second_thread.wait();

		m_chunks4verticies_generation.clear();

		//std::cout << "thread loop time " << loop_timer.getElapsedTime().asMilliseconds() << std::endl << std::endl;
	}
}

void MapMeshBuilder::unload_columns(RenderRange& range)
{
	m_mutex__chunks4rendering.lock();
	for (auto it = m_chunks4rendering.begin(); it != m_chunks4rendering.end();) {
		Chunk& chunk = *(*it);
		auto& pos = chunk.get_pos();
		if (pos.x < range.start_x || pos.z < range.start_z || pos.x > range.end_x || pos.z > range.end_z) {
			chunk.set_is_rendering(false);
			chunk.free_buffers();
			it = m_chunks4rendering.erase(it);

		}
		else {
			it++;
		}
	}

	m_map->unload_columns(std::max(range.start_x - 5, 0), range.end_x + 5, std::max(range.start_z - 5, 0), range.end_z + 5);


	m_mutex__chunks4rendering.unlock();
}

void MapMeshBuilder::regenerate_edited_chunk_verticies()
{
	static bool is_thread_free = true;
	static bool should_update_priority_chunks = false;
	static std::vector<Chunk*> priority4_rendering;
	static bool is_new_chunk = false;

	auto update_edited_chunk = [&]() {

		Chunk& chunk = *m_map->get_edited_chunk();

		if (chunk.is_rendering()) {
			chunk.update_vertices_using_old_buffers();
		}
		else {
			chunk.set_is_rendering(true);
			chunk.update_vertices(m_mutex__chunks4vbo_generation);

			is_new_chunk = true;
		}


		priority4_rendering.push_back(&chunk);
		for (sf::Vector3i& pos : m_map->m_should_be_updated_neighbours) {
			Chunk& chunk = m_map->get_chunk_or_generate(pos.x, pos.y, pos.z);

			if (chunk.is_rendering()) {
				chunk.update_vertices_using_old_buffers();
				priority4_rendering.push_back(&chunk);
			}
		}
		should_update_priority_chunks = true;

		is_thread_free = true;
	};



	static sf::Thread update_edited_chunk_thread(update_edited_chunk);



	if (m_map->is_chunk_edited() && is_thread_free && !should_update_priority_chunks) {
		is_thread_free = false;

		update_edited_chunk_thread.launch();
	}

	if (should_update_priority_chunks) {
		//TODO set_block here not in map
		for (Chunk* chunk : priority4_rendering) {
			chunk->upate_vao();

			if (is_new_chunk) {
				m_mutex__chunks4rendering.lock();
				m_chunks4rendering.insert(chunk);
				m_mutex__chunks4rendering.unlock();

				is_new_chunk = false;
			}
		}
		priority4_rendering.clear();

		m_map->apply_chunk_changes();
		should_update_priority_chunks = false;	
	}
}

void MapMeshBuilder::add_new_chunks2rendering()
{
	m_mutex__chunks4vbo_generation.lock();
	m_mutex__chunks4rendering.lock();

	copy(
		m_chunks4vbo_generation.begin(),
		m_chunks4vbo_generation.end(),
		inserter(m_chunks4rendering, m_chunks4rendering.end())
	);

	for (Chunk* chunk : m_chunks4vbo_generation) {
		chunk->set_is_rendering(true);
		chunk->upate_vao();
	}

	m_chunks4vbo_generation.clear();

	m_mutex__chunks4rendering.unlock();
	m_mutex__chunks4vbo_generation.unlock();
}

void MapMeshBuilder::add_chunks2verticies_generation(RenderRange& range)
{
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)m_window->getSize().x / (GLfloat)m_window->getSize().y, 0.1f, RENDER_DISTANCE);
	glm::mat4 view = glm::lookAt(
		glm::vec3(
			m_player->get_position().x,
			m_player->get_position().y + 0.8f,//m_size.y
			m_player->get_position().z
		),
		glm::vec3(
			m_player->get_position().x - sin(m_player->m_camera_angle.x / 180 * PI),
			m_player->get_position().y +  0.8f + tan(m_player->m_camera_angle.y / 180 * PI),
			m_player->get_position().z - cos(m_player->m_camera_angle.x / 180 * PI)
		),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 pv = projection * view;

	static const float SPHERE_DIAMETER = sqrtf(3.f*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK);
	static const int VISIBLE_COLUMNS_PER_LOOP = 20;
	int visible_columns_count = 0;

	
	auto add2verticies_generation = [&](int i, int k) {
		if (i >= 0 && k >= 0) {
			for (int j = 0; j < CHUNKS_IN_WORLD_HEIGHT; ++j) {
				glm::vec4 norm_coords = pv * glm::vec4(i * BLOCKS_IN_CHUNK, j * BLOCKS_IN_CHUNK, k * BLOCKS_IN_CHUNK, 1.F);
				norm_coords.x /= norm_coords.w;
				norm_coords.y /= norm_coords.w;

				bool is_chunk_visible = norm_coords.z >= -1 * SPHERE_DIAMETER / fabsf(norm_coords.w)
					&& fabsf(norm_coords.x) <= 1 + 1 * SPHERE_DIAMETER / fabsf(norm_coords.w)
					&& fabsf(norm_coords.y) <= 1 + 1 * SPHERE_DIAMETER / fabsf(norm_coords.w);


				if (is_chunk_visible) {
					//TODO invalid "rendering sphere"
					auto& column = m_map->get_column_or_generate(i, k);

					if (!column[j].is_rendering()) {
						bool is_new = false;
						for (int y = 0; y < CHUNKS_IN_WORLD_HEIGHT; ++y) {
							Chunk& chunk = column[y];

							if (chunk.can_generate_verticies()) {
								is_new = true;
								m_chunks4verticies_generation.push_back(&chunk);
							}

						}

						if (is_new)
							++visible_columns_count;
						break;
					}
				}
			}
		}
	};

	int loc_start_x = range.chunk_x,
		loc_start_z = range.chunk_z,
		loc_end_x = range.chunk_x,
		loc_end_z = range.chunk_z;


	while (loc_start_x > range.start_x &&
		loc_start_z > range.start_z &&
		loc_end_x < range.end_x &&
		loc_end_z < range.end_z)
	{
		int i = loc_start_x;
		int k = loc_start_z;
		for (; k <= loc_end_z; ++k) {
			if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
				break;
			add2verticies_generation(i, k);
		}
		if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
			break;

		for (; i <= loc_end_x; ++i) {
			if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
				break;
			add2verticies_generation(i, k);
		}
		if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
			break;

		for (; k >= loc_start_z; --k) {
			if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
				break;
			add2verticies_generation(i, k);
		}
		if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
			break;

		for (; i >= loc_start_x; --i) {
			if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
				break;
			add2verticies_generation(i, k);
		}
		if (visible_columns_count >= VISIBLE_COLUMNS_PER_LOOP)
			break;

		--loc_start_x;
		--loc_start_z;
		++loc_end_x;
		++loc_end_z;
	}
}
