#include "pch.h"
#include "block_db.h"
#include "Map.h"
#include "Chunk.h"
#include "ChunkMeshBasic.h"
#include "Converter.h"
using namespace World;


void Chunk::init(const sf::Vector3i &pos, World::Map *map) {
    if (!m_is_init) {
        m_is_init = true;
        m_pos = pos;
        m_map = map;
    }
}

block_id Chunk::get_block_type(int x, int y, int z) {
    if (m_blocks.empty()) {
        return block_id::Air;
    }

    return m_blocks.at(x + y * BLOCKS_IN_CHUNK + z * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK);
}

void Chunk::generate_block_vertices(int i, int j, int k, GLfloat x, GLfloat y, GLfloat z, block_id id) {
    if (!is_opaque__in_chunk(i - 1, j, k)) {
        m_blocks_mesh.generate_vertices4negative_x(x, y, z, id);
    }

    if (!is_opaque__in_chunk(i + 1, j, k)) {
        m_blocks_mesh.generate_vertices4positive_x(x, y, z, id);
    }

    if (!is_opaque__in_chunk(i, j - 1, k)) {
        m_blocks_mesh.generate_vertices4negative_y(x, y, z, id);
    }

    if (!is_opaque__in_chunk(i, j + 1, k)) {
        m_blocks_mesh.generate_vertices4positive_y(x, y, z, id);
    }

    if (!is_opaque__in_chunk(i, j, k - 1)) {
        m_blocks_mesh.generate_vertices4negative_z(x, y, z, id);
    }

    if (!is_opaque__in_chunk(i, j, k + 1)) {
        m_blocks_mesh.generate_vertices4positive_z(x, y, z, id);
    }
}

void Chunk::generate_water_vertices(int i, int j, int k, GLfloat x, GLfloat y, GLfloat z, block_id id) {
    if (!is_water__in_chunk(i - 1, j, k) && !is_opaque__in_chunk(i - 1, j, k)) {
        m_water_mesh.generate_vertices4negative_x(x, y, z, id);
    }

    if (!is_water__in_chunk(i + 1, j, k) && !is_opaque__in_chunk(i + 1, j, k)) {
        m_water_mesh.generate_vertices4positive_x(x, y, z, id);
    }

    if (!is_water__in_chunk(i, j - 1, k) && !is_opaque__in_chunk(i, j - 1, k)) {
        m_water_mesh.generate_vertices4negative_y(x, y, z, id);
    }

    if (!is_water__in_chunk(i, j + 1, k) && !is_opaque__in_chunk(i, j + 1, k)) {
        m_water_mesh.generate_vertices4positive_y(x, y, z, id);
    }

    if (!is_water__in_chunk(i, j, k - 1) && !is_opaque__in_chunk(i, j, k - 1)) {
        m_water_mesh.generate_vertices4negative_z(x, y, z, id);
    }

    if (!is_water__in_chunk(i, j, k + 1) && !is_opaque__in_chunk(i, j, k + 1)) {
        m_water_mesh.generate_vertices4positive_z(x, y, z, id);
    }
}

void Chunk::generate_cactus_vertices(GLfloat x, GLfloat y, GLfloat z, block_id id) {
    static GLfloat pixel = 1.005 / BLOCK_RESOLUTION;

    m_blocks_mesh.generate_vertices4negative_x(x + pixel, y, z, id);
    m_blocks_mesh.generate_vertices4positive_x(x - pixel, y, z, id);

    m_blocks_mesh.generate_vertices4negative_y(x, y, z, id);
    m_blocks_mesh.generate_vertices4positive_y(x, y, z, id);

    m_blocks_mesh.generate_vertices4negative_z(x, y, z + pixel, id);
    m_blocks_mesh.generate_vertices4positive_z(x, y, z - pixel, id);
}

void Chunk::generate_vertices() {
    for (int j = 0; j < BLOCKS_IN_CHUNK; ++j) {
    if (should_make_layer(j))
    for (int i = 0; i < BLOCKS_IN_CHUNK; ++i)
    for (int k = 0; k < BLOCKS_IN_CHUNK; ++k)

        if (!is_air__in_chunk(i, j, k)) {
            // local(in chunk) pos

            auto x = static_cast<GLfloat>(i);
            auto y = static_cast<GLfloat>(j);
            auto z = static_cast<GLfloat>(k);

            static GLfloat pixel = 1.005 / BLOCK_RESOLUTION;
            block_id id = get_block_type(i, j, k);


            if (id == block_id::Cactus) {
                generate_cactus_vertices(x,y,z,id);
            } else if (id == block_id::Water) {
                generate_water_vertices(i,j,k,x,y,z,id);
            } else {
                generate_block_vertices(i,j,k,x,y,z,id);
            }
        }
    }
}

void World::Chunk::update_vao() {
    m_blocks_mesh.update_vao();
    m_water_mesh.update_vao();

    m_is_blocked_vertices_generation = false;
}

void World::Chunk::update_vertices() {
    if (!m_is_blocked_vertices_generation) {
        m_is_blocked_vertices_generation = true;
        m_blocks_mesh.update_vertices();
        m_water_mesh.update_vertices();
        generate_vertices();
    }
}

void World::Chunk::free_buffers() {
    m_blocks_mesh.free_buffers(m_map);
    m_water_mesh.free_buffers(m_map);
}

void Chunk::set_block_type(int x, int y, int z, block_id type) {
    m_blocks.at(x + y * BLOCKS_IN_CHUNK + z * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK) = type;
    m_layers[y].update(type);
}

bool Chunk::is_empty() {
    for (block_id id : m_blocks) {
        if (id != block_id::Air) {
            return false;
        }
    }

    return true;
};

bool Chunk::is_air__in_chunk(int x, int y, int z) {
    if (y < 0 || x < 0 || z < 0 || y >= BLOCKS_IN_CHUNK || x >= BLOCKS_IN_CHUNK || z >= BLOCKS_IN_CHUNK) {
        return m_map->is_air(
                x + Converter::chunk_coord2block_coord(m_pos.x),
                y + Converter::chunk_coord2block_coord(m_pos.y),
                z + Converter::chunk_coord2block_coord(m_pos.z)
        );
    }

    return get_block_type(x, y, z) == block_id::Air;
}

bool Chunk::is_opaque__in_chunk(int x, int y, int z) {
    if (y < 0 || x < 0 || z < 0 || y >= BLOCKS_IN_CHUNK || x >= BLOCKS_IN_CHUNK || z >= BLOCKS_IN_CHUNK) {
        return m_map->is_opaque(
                x + Converter::chunk_coord2block_coord(m_pos.x),
                y + Converter::chunk_coord2block_coord(m_pos.y),
                z + Converter::chunk_coord2block_coord(m_pos.z)
        );
    }

    return !is_block_type_transparent(get_block_type(x, y, z));
}


bool Chunk::is_water__in_chunk(int x, int y, int z) {
    if (y < 0 || x < 0 || z < 0 || y >= BLOCKS_IN_CHUNK || x >= BLOCKS_IN_CHUNK || z >= BLOCKS_IN_CHUNK) {
        return m_map->is_water(
                x + Converter::chunk_coord2block_coord(m_pos.x),
                y + Converter::chunk_coord2block_coord(m_pos.y),
                z + Converter::chunk_coord2block_coord(m_pos.z)
        );
    }

    return get_block_type(x, y, z) == block_id::Water;
}


Chunk::Chunk() : m_blocks_mesh(this), m_water_mesh(this) {
    m_blocks.fill(block_id::Air);
}

Chunk::~Chunk() = default;

bool Chunk::should_make_layer(int y) {
    auto is_all_solid_check_range = [&](sf::Vector3i &pos, int y) {
        //check error value
        if (y == -1) {
            return is_layer_solid(pos + sf::Vector3i{0, -1, 0}, BLOCKS_IN_CHUNK - 1);
        } else if (y == BLOCKS_IN_CHUNK) {
            return is_layer_solid(pos + sf::Vector3i{0, 1, 0}, 0);
        } else {
            return is_layer_solid(pos, y);
        }
    };

    return !is_all_solid_check_range(m_pos, y) ||
           !is_all_solid_check_range(m_pos, y - 1) ||
           !is_all_solid_check_range(m_pos, y + 1) ||
           !is_layer_solid(m_pos + sf::Vector3i{1, 0, 0}, y) ||
           !is_layer_solid(m_pos + sf::Vector3i{-1, 0, 0}, y) ||
           !is_layer_solid(m_pos + sf::Vector3i{0, 0, 1}, y) ||
           !is_layer_solid(m_pos + sf::Vector3i{0, 0, -1}, y);

}

bool Chunk::is_layer_solid(sf::Vector3i pos, int y) {
    if (pos.y < 0 || pos.y >= CHUNKS_IN_WORLD_HEIGHT)
        return false;

    Chunk &chunk = m_map->get_chunk_or_generate(pos.x, pos.y, pos.z);

    if (chunk.m_is_init) {
        return chunk.m_layers[y].is_all_solid();
    } else {
        return false;
    }
}

void ChunkLayer::update(block_id type) {
    if (Chunk::is_block_type_transparent(type)) {
        --solid_block_count;
    } else {
        ++solid_block_count;
    }
}

bool World::Chunk::is_block_type_transparent(block_id type) {
    return type == block_id::Air || type == block_id::Cactus || type == block_id::Water;
    /*|| type == block_id::Oak_leafage*/// || type == block_id::Glass; //... || type == block_id::Oak_leafage
}

bool World::Chunk::is_block_type_solid(block_id type) {
    return type != block_id::Air && type != block_id::Water; //...
}