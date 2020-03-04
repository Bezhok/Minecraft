#include "pch.h"
#include "Map.h"
#include "Chunk.h"
#include "block_db.h"
#include "TerrainGenerator.h"
#include "Converter.h"

using namespace World;


Map::Map() : m_terrain_generator(this), m_should_be_updated_neighbours(6) {
    int columns_count = RENDER_DISTANCE_IN_CHUNKS * RENDER_DISTANCE_IN_CHUNKS * 2;
    m_map.reserve(columns_count);
}

World::Map::~Map() = default;

inline void World::Map::generate_chunk_terrain(Column &column, int chunk_x, int chunk_y, int chunk_z) {
    m_terrain_generator.generate_chunk_terrain(column, chunk_x, chunk_y, chunk_z);
}

void Map::calculate_pos(sf::Vector3i &pos_rel2chunk, sf::Vector3i &chunk_pos) {
    if (pos_rel2chunk.y < 0) {
        pos_rel2chunk.y = BLOCKS_IN_CHUNK + pos_rel2chunk.y;
        chunk_pos.y -= 1;
    } else {
        chunk_pos.y += Converter::coord2chunk_coord(pos_rel2chunk.y);
    }

    if (pos_rel2chunk.z < 0) {
        pos_rel2chunk.z = BLOCKS_IN_CHUNK + pos_rel2chunk.z;
        chunk_pos.z -= 1;
    } else {
        chunk_pos.z += Converter::coord2chunk_coord(pos_rel2chunk.z);
    }

    if (pos_rel2chunk.x < 0) {
        pos_rel2chunk.x = BLOCKS_IN_CHUNK + pos_rel2chunk.x;
        chunk_pos.x -= 1;
    } else {
        chunk_pos.x += Converter::coord2chunk_coord(pos_rel2chunk.x);
    }
}

void Map::set_block_type(sf::Vector3i pos_in_chunk, Column &column, int chunk_y, block_id type) {
    Chunk &chunk = column[chunk_y];
    auto chunk_pos = chunk.get_pos();

    // if same column
    if (pos_in_chunk.x >= 0 && pos_in_chunk.z >= 0 && pos_in_chunk.x < BLOCKS_IN_CHUNK &&
        pos_in_chunk.z < BLOCKS_IN_CHUNK) {

        calculate_pos(pos_in_chunk, chunk_pos);

        if (chunk_pos.y < 0 || chunk_pos.y >= CHUNKS_IN_WORLD_HEIGHT)
            return;

        column[chunk_pos.y].set_block_type(pos_in_chunk.x, Converter::coord2block_coord_in_chunk(pos_in_chunk.y), pos_in_chunk.z,
                                       type);
    }
    else {
        calculate_pos(pos_in_chunk, chunk_pos);

        if (chunk_pos.x < 0 || chunk_pos.z < 0 || chunk_pos.y < 0 || chunk_pos.y >= CHUNKS_IN_WORLD_HEIGHT)
            return;

        Chunk &temp_chunk = get_chunk_or_generate(chunk_pos.x, chunk_pos.y, chunk_pos.z);

        temp_chunk.set_block_type(
                Converter::coord2block_coord_in_chunk(pos_in_chunk.x),
                Converter::coord2block_coord_in_chunk(pos_in_chunk.y),
                Converter::coord2block_coord_in_chunk(pos_in_chunk.z),
                type
        );
    }
}

block_id Map::get_type(int x, int y, int z) {
    int chunk_x = x / BLOCKS_IN_CHUNK,
            chunk_y = y / BLOCKS_IN_CHUNK,
            chunk_z = z / BLOCKS_IN_CHUNK;

    if (y < 0 || chunk_y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return block_id::EMPTY_TYPE;

    return get_chunk_or_generate(chunk_x, chunk_y, chunk_z).get_block_type(x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK,
                                                                           z % BLOCKS_IN_CHUNK);
}

bool Map::is_solid(int x, int y, int z) {
    return Chunk::is_block_type_solid(get_type(x,y,z));
}

bool Map::is_opaque(int x, int y, int z) {
    return !Chunk::is_block_type_transperent(get_type(x,y,z));
}

bool Map::is_air(int x, int y, int z) {
    return get_type(x,y,z) == block_id::Air;
}

bool Map::is_water(int x, int y, int z) {
    return get_type(x,y,z) == block_id::Water;
}

bool Map::create_block(int x, int y, int z, block_id id) {
    if (!m_should_redraw_chunk) {
        sf::Vector3i chunk_pos = {x / BLOCKS_IN_CHUNK, y / BLOCKS_IN_CHUNK, z / BLOCKS_IN_CHUNK};
        sf::Vector3i block_in_chunk_pos = {x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK, z % BLOCKS_IN_CHUNK};

        if (y < 0 || chunk_pos.y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

        Chunk &chunk = get_chunk_or_generate(chunk_pos.x, chunk_pos.y, chunk_pos.z);
        auto old_id = chunk.get_block_type(block_in_chunk_pos.x, block_in_chunk_pos.y, block_in_chunk_pos.z);
        if (old_id == block_id::Air || old_id == block_id::Water) {
            m_edited_block_type = id;
            m_edited_block_pos = block_in_chunk_pos;
            m_edited_chunk = &chunk;
            chunk.set_block_type(block_in_chunk_pos.x, block_in_chunk_pos.y, block_in_chunk_pos.z, id);

            m_should_redraw_chunk = true;
            play_sound(Sounds::SoundsNames::wood1);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void Map::find_neighbours(const sf::Vector3i &chunk_pos, const sf::Vector3i &block_in_chunk_pos, int x, int y, int z) {
    if (block_in_chunk_pos.x == 0 && is_opaque(x - 1, y, z)) {
        m_should_be_updated_neighbours.emplace_back((x - 1) / BLOCKS_IN_CHUNK, chunk_pos.y, chunk_pos.z);
    }

    if (block_in_chunk_pos.x == 15 && is_opaque(x + 1, y, z)) {
        m_should_be_updated_neighbours.emplace_back((x + 1) / BLOCKS_IN_CHUNK, chunk_pos.y, chunk_pos.z);
    }

    if (block_in_chunk_pos.y == 0 && is_opaque(x, y - 1, z)) {
        m_should_be_updated_neighbours.emplace_back(chunk_pos.x, (y - 1) / BLOCKS_IN_CHUNK, chunk_pos.z);
    }

    if (block_in_chunk_pos.y == 15 && is_opaque(x, y + 1, z)) {
        m_should_be_updated_neighbours.emplace_back(chunk_pos.x, (y + 1) / BLOCKS_IN_CHUNK, chunk_pos.z);
    }

    if (block_in_chunk_pos.z == 0 && is_opaque(x, y, z - 1)) {
        m_should_be_updated_neighbours.emplace_back(chunk_pos.x, chunk_pos.y, (z - 1) / BLOCKS_IN_CHUNK);
    }

    if (block_in_chunk_pos.z == 15 && is_opaque(x, y, z + 1)) {
        m_should_be_updated_neighbours.emplace_back(chunk_pos.x, chunk_pos.y, (z + 1) / BLOCKS_IN_CHUNK);
    }
}

bool Map::delete_block(int x, int y, int z) {
    if (!m_should_redraw_chunk) {
        sf::Vector3i chunk_pos = {x / BLOCKS_IN_CHUNK, y / BLOCKS_IN_CHUNK, z / BLOCKS_IN_CHUNK};
        sf::Vector3i block_in_chunk_pos = {x % BLOCKS_IN_CHUNK, y % BLOCKS_IN_CHUNK, z % BLOCKS_IN_CHUNK};

        if (y < 0 || chunk_pos.y >= CHUNKS_IN_WORLD_HEIGHT || x < 0 || z < 0) return false;

        Chunk &chunk = get_chunk_or_generate(chunk_pos.x, chunk_pos.y, chunk_pos.z);
        auto old_id = chunk.get_block_type(block_in_chunk_pos.x, block_in_chunk_pos.y, block_in_chunk_pos.z);

        if (old_id == block_id::Air || old_id == block_id::Water) {
            return false;
        } else {
            find_neighbours(chunk_pos, block_in_chunk_pos, x, y, z);
            m_edited_block_pos = {block_in_chunk_pos.x, block_in_chunk_pos.y, block_in_chunk_pos.z};
            m_edited_chunk = &chunk;
            m_edited_block_type = block_id::Air;
            chunk.set_block_type(block_in_chunk_pos.x, block_in_chunk_pos.y, block_in_chunk_pos.z,
                                 block_id::Air);//block_id::transperent_type

            play_sound(Sounds::SoundsNames::wood4);
            m_should_redraw_chunk = true;
            return true;
        }
    }
    return false;
}

void Map::play_sound(Sounds::SoundsNames name) {
    static sf::Sound sound;
    sound.stop();
    sound.setBuffer(m_sounds.m_sound_buffers[name]);
    sound.play();
}

void Map::apply_chunk_changes() {
    if (m_should_redraw_chunk) {
        m_should_redraw_chunk = false;
        m_should_be_updated_neighbours.clear();

        m_edited_chunk = nullptr;
    }
}

Map::Column &Map::get_column(int i, int k) {
    return m_map[hashXZ(i, k)];
}

Chunk &Map::get_chunk(int i, int j, int k) {
    return m_map[hashXZ(i, k)][j];
}

void Map::unload_column(int i, int k) {
    m_map.erase(hashXZ(i, k));
}

Chunk &Map::get_chunk_or_generate(int i, int j, int k) {
    auto &column = get_column_or_generate(i, k);

    Chunk &chunk = column[j];
    chunk.init({i, j, k}, this);
    return chunk;
}

Map::Column &Map::get_column_or_generate(int i, int k) {
    auto columnIt = m_map.find(hashXZ(i, k));

    if (columnIt != m_map.end()) {
        return columnIt->second;
    } else {
        auto &column = m_map[hashXZ(i, k)];

        for (int j = 0; j < CHUNKS_IN_WORLD_HEIGHT; ++j) {
            generate_chunk_terrain(column, i, j, k);
            column[j].init({i, j, k}, this);
        }
        return column;
    }
}

void Map::unload_columns(int start_x, int end_x, int start_z, int end_z) {
    for (auto it = m_map.begin(); it != m_map.end();) {
        auto &column = it->second;
        auto &pos = column[0].get_pos();
        if (pos.x < start_x || pos.z < start_z || pos.x > end_x || pos.z > end_z) {
            for (int i = 0; i < CHUNKS_IN_WORLD_HEIGHT; ++i) {
                column[i].free_buffers();
            }

            it = m_map.erase(it);
        } else {
            it++;
        }
    }
}