#pragma once
#include "pch.h"
#include "game_constants.h"
#include "Chunk.h"


namespace World {
	enum class block_id : uint8_t;
	//class Chunk;

	class Map
	{
		//google::dense_hash_map<int, std::array<Chunk, 16>> m_map;
		spp::sparse_hash_map<int, std::array<Chunk, 16>> m_map;
	private:
		sf::Vector3i m_edited_chunk_pos;
		

		bool m_redraw_chunk = false;

		
		typedef std::array<std::array<std::array<Chunk, CHUNKS_IN_WORLD>, CHUNKS_IN_WORLD_HEIGHT>, CHUNKS_IN_WORLD> map_type;
		typedef std::shared_ptr<map_type> map_ptr;
		//map_ptr m_map;
		
	public:
		/* load world */
		Map();

		/* eponymous */
		bool is_block(int mx, int my, int mz);
		bool create_block(int x, int y, int z, block_id type);
		bool delete_block(int x, int y, int z);

		/* eponymous */ //TODO rewrite to binary
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
		
		bool can_get_chunk(int i, int j, int k)
		{
			i += 0x9e3779b9 + (k << 6) + (k >> 2);
			k ^= i;

			auto column = m_map.find(k);

			return column != m_map.end();
		}

		auto& get_chunk(int i, int j, int k)
		{ 
			//auto& column = m_map[i * 100000 + k];

			i += 0x9e3779b9 + (k << 6) + (k >> 2);
			k ^= i;

			auto& column = m_map[k];

			////return m_map[(i << 16) || k ][j];
			return column[j];

			//return m_map->at(i).at(j).at(k);

			//return m_map(i,j,k);
		};

		//auto& get_column(int i, int k)
		//{
		//	return m_map[i * 10000 + k];
		//};
		sf::Mutex* m_mutex__chunks4vbo_generation;

		std::vector<std::pair<GLuint, GLuint>> m_global_vao_vbo_buffers;
		std::unordered_set<Chunk*> m_free_vbo_chunks;

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
	private:
		bool is_chunk_in_map(int x, int y, int z);
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
