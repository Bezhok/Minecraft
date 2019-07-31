#pragma once
#include "pch.h"

class Player;
namespace World {
	class Map;
	class Chunk;

	class MapMeshBuilder {
		struct RenderRange {
			int start_x,
				end_x,
				start_z,
				end_z,
				chunk_x,
				chunk_z;
		};
	private:
		void add_chunks2verticies_generation(RenderRange& range);
		void unload_columns(RenderRange& range);
	public:
		void generate_verticies();

		void add_new_chunks2rendering();
		void regenerate_edited_chunk_verticies();

		void launch(Map* map, Player* player, sf::Window* window);


		MapMeshBuilder();




		std::vector<World::Chunk*> m_chunks4verticies_generation;
		phmap::parallel_node_hash_set<World::Chunk*> m_chunks4rendering;

		sf::Mutex m_mutex__chunks4vbo_generation, m_mutex__chunks4rendering;
	private:
		Map* m_map = nullptr;
		Player* m_player = nullptr;
		sf::Window* m_window = nullptr;
		sf::Thread m_verticies_generator_thread;

		
		
		std::vector<World::Chunk*> m_chunks4vbo_generation;
	};
}