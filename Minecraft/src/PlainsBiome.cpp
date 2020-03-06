#include "PlainsBiome.h"

using namespace World;

PlainsBiome::~PlainsBiome() {

}

void PlainsBiome::generate_tree(std::function<void(sf::Vector3i, BlockType)> set) {
    int tree_height = glm::linearRand(5, 7);
    for (int y = 0; y < tree_height; ++y) {
        set(sf::Vector3i{0, y, 0}, BlockType::Oak);
    }

    set({0, tree_height, 0}, BlockType::Oak_leafage);
    set({-1, tree_height, 0}, BlockType::Oak_leafage);
    set({1, tree_height, 0}, BlockType::Oak_leafage);
    set({0, tree_height, 1}, BlockType::Oak_leafage);
    set({0, tree_height, -1}, BlockType::Oak_leafage);

    for (int x = -1; x <= 1; ++x)
        for (int z = -1; z <= 1; ++z) {
            if (x != 0 || z != 0) {
                set({x, tree_height - 1, z}, BlockType::Oak_leafage);
            }
        }

    for (int x = -2; x <= 2; ++x)
        for (int z = -2; z <= 2; ++z) {
            if (x != 0 || z != 0) {
                set({x, tree_height - 2, z}, BlockType::Oak_leafage);
                set({x, tree_height - 3, z}, BlockType::Oak_leafage);
            }
        }
}

PlainsBiome::PlainsBiome(int water_level) : Biome(water_level) {
    m_tree_frequency = 200;
    m_top_block_type = BlockType::Grass;
    m_below_block_type = BlockType::Dirt;
}
