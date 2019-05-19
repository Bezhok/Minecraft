#pragma once
#include "pch.h"
#include "game_constants.h"
#include "block_db.h"
#include "Chunk.h"

namespace World {
	class Map
	{
		typedef std::array<std::array<std::array<Chunk, SUPER_CHUNK_SIZE>, SUPER_CHUNK_SIZE_HEIGHT>, SUPER_CHUNK_SIZE> map_type;
		typedef std::shared_ptr<map_type> map_ptr;

	private:
		sf::Vector3i m_edited_chunk_pos;
		bool m_redraw_chunk = false;

		map_ptr m_map;

	public:
		/* load world */
		Map();

		/* eponymous */
		bool is_block_without_checking_range(const int &x, const int &y, const int &z);
		bool is_block(int mx, int my, int mz);
		bool create_block(int x, int y, int z, DB::block_id type);
		bool delete_block(int x, int y, int z);

		/* eponymous */ //TODO rewrite to binary
		bool save();
		bool load();

		/* when you add/delete block */
		bool is_chunk_edited() { return m_redraw_chunk; };
		void cancel_chunk_editing_state() { m_redraw_chunk = false; };

		/* getters */
		const sf::Vector3i& get_edited_chunk_pos() { return m_edited_chunk_pos; };
		auto& get_chunk(int i, int j, int k) { return m_map->operator[](i)[j][k]; };

		std::list<std::pair<GLuint, GLuint>> m_global_vao_vbo_buffers;
		std::unordered_set<Chunk*> m_free_vbo_chunks;
	private:
		bool is_chunk_in_map(const int &x, const int &y, const int &z);
	};
}


