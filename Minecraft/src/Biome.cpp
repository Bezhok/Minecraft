#include "Biome.h"
#include "pch.h"

using namespace World;

BlockType Biome::generate_block(sf::Vector3i block_pos, int ground_h, int block_global_y) {
    BlockType id;
    if (ground_h < m_water_level && block_global_y >= ground_h && block_global_y < m_water_level) {
        id = BlockType::Water;
    } else if (block_global_y > ground_h) {
        id = BlockType::Air;
    } else if (ground_h - 4 < m_water_level && block_global_y > ground_h - 4) {
        id = BlockType::Sand;
    } else {
        if (block_global_y == ground_h) {
            if (glm::linearRand(0, m_tree_frequency) == 1) {
                m_tree_positions.emplace_back(block_pos.x, block_pos.y + 1, block_pos.z);
                id = m_below_block_type;
            } else {
                id = m_top_block_type;
            }
        } else if (block_global_y > ground_h - 5) {
            id = m_below_block_type;
        } else {
            id = BlockType::Stone;
        }
    }

    return id;
}

Biome::Biome() {

}

const std::vector<sf::Vector3i> &Biome::get_tree_positions() const {
    return m_tree_positions;
}

void Biome::set_tree_positions(const std::vector<sf::Vector3i> &treePositions) {
    m_tree_positions = treePositions;
}

Biome::Biome(int water_level) : m_water_level(water_level) {}

void Biome::set_tree_frequency(int tree_frequency) {
    m_tree_frequency = tree_frequency;
}
