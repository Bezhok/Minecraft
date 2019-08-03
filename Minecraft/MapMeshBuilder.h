#pragma once
#include "pch.h"

class Player;
namespace World {
	class Map;
	class Chunk;

	class MapMeshBuilder {
		struct RenderRange {
			int start_x = 0,
				end_x   = 0,
				start_z = 0,
				end_z   = 0,
				chunk_x = 0,
				chunk_z = 0;
		};
	private:
		Map* m_map = nullptr;
		Player* m_player = nullptr;
		sf::Window* m_window = nullptr;
		sf::Thread m_verticies_generator_thread;


		std::vector<World::Chunk*> m_chunks4verticies_generation;
		std::vector<World::Chunk*> m_chunks4vbo_generation;

	public:
		sf::Mutex m_mutex__chunks4vbo_generation, m_mutex__chunks4rendering;
		phmap::parallel_node_hash_set<World::Chunk*> m_chunks4rendering;

	private:
		void add_chunks2verticies_generation(RenderRange& range);
		void unload_columns(RenderRange& range);

	public:
		void generate_verticies();
		void add_new_chunks2rendering();
		void regenerate_edited_chunk_verticies();

		void launch(Map* map, Player* player, sf::Window* window);

		MapMeshBuilder();

		int get_chunks4verticies_generation_size() { return m_chunks4verticies_generation.size(); };
		int get_chunks4rendering_size() { return m_chunks4rendering.size(); };
	};
}