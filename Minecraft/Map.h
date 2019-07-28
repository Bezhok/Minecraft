#pragma once
#include "pch.h"
#include "game_constants.h"
#include "Chunk.h"
#include "TerrainGenerator.h"

#include "parallel_hashmap/phmap.h"


namespace World {
	enum class block_id : uint8_t;

	class Map
	{
	public:
		using Column = std::array<Chunk, CHUNKS_IN_WORLD_HEIGHT>;

	private:
		phmap::parallel_node_hash_map<int, Column> m_map;
		sf::Vector3i m_edited_chunk_pos;
		bool m_redraw_chunk = false;
		TerrainGenerator m_terrain_generator;

	public:
		int get_size() { return m_map.size(); };
		void generate_chunk_terrain(sf::Vector3i& pos);
		void generate_chunk_terrain(int, int, int);

		/* load world */
		Map();

		/* eponymous */
		bool is_solid(int mx, int my, int mz);
		bool is_opaque(int mx, int my, int mz);
		bool is_air(int mx, int my, int mz);

		

		bool create_block(int x, int y, int z, block_id type);
		bool delete_block(int x, int y, int z);

		/* eponymous */ //TODO
		bool save();
		bool load();

		std::vector<sf::Vector3i> m_should_be_updated_neighbours;

		/* when you add/delete block */
		bool is_chunk_edited() { return m_redraw_chunk; };
		void cancel_chunk_editing_state() 
		{ 
			m_redraw_chunk = false;
			m_should_be_updated_neighbours.clear();
		};

		/* getters */
		sf::Vector3i m_edited_block_pos;
		const sf::Vector3i& get_edited_chunk_pos() { return m_edited_chunk_pos; };
		
		// TODO maybe hash collisions
		inline int hashXZ(int i, int k)
		{
			//i += 0x9e3779b9 + (k << 6) + (k >> 2);
			//k ^= i;
			k = i * 10000 + k;
			return k;
		}

		void unload_columns(int start_x, int end_x, int start_z, int end_z);

		void unload_column(int i, int k);
		Column& get_column(int i, int k);
		Chunk& get_chunk(int i, int j, int k);

		Chunk& get_chunk_n(int i, int j, int k);
		Column& get_column_n(int i, int k);

		sf::Mutex* m_mutex__chunks4vbo_generation;

		std::vector<std::pair<GLuint, GLuint>> m_global_vao_vbo_buffers;

		void set_block(sf::Vector3i& pos_in_chunk, Map::Column& column, int y, block_id type);

		template<typename T>
		static int coord2chunk_coord(T c);

		template<typename T>
		static int coord2block_coord(T c);

		template<typename T>
		static int coord2block_coord_in_chunk(T c);//block in chunk coord

		template<typename T>
		static int block_coord2coord(T c);

		template<typename T>
		static int chunk_coord2block_coord(T c);
	};
}

template<typename T>
int World::Map::coord2chunk_coord(T c)
{
	return static_cast<int>(c / BLOCKS_IN_CHUNK);
}

template<typename T>
int World::Map::coord2block_coord(T c)
{
	return static_cast<int>(static_cast<float>(c) / COORDS_IN_BLOCK);
}

template<typename T>
int World::Map::coord2block_coord_in_chunk(T c)
{
	return static_cast<int>(c % BLOCKS_IN_CHUNK);
}

template<typename T>
int World::Map::block_coord2coord(T c)
{
	return static_cast<int>(static_cast<float>(c) * COORDS_IN_BLOCK);
}

template<typename T>
inline int World::Map::chunk_coord2block_coord(T c)
{
	return static_cast<int>(c * static_cast<T>(BLOCKS_IN_CHUNK));
}
