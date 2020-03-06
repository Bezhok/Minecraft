#include "DesertBiome.h"

using namespace World;

void DesertBiome::generate_tree(std::function<void(sf::Vector3i, BlockType)> setter_callback) {
    int tree_height = glm::linearRand(3, 6);
    for (int y = 0; y < tree_height; ++y) {
        setter_callback(sf::Vector3i{0, y, 0}, BlockType::Cactus);
    }
}

DesertBiome::~DesertBiome() {

}

DesertBiome::DesertBiome(int water_level) : Biome(water_level) {
    m_tree_frequency = 1000;
    m_top_block_type = BlockType::Sand;
    m_below_block_type = BlockType::Sand;
}
