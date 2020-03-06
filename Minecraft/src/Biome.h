#pragma once

#include "pch.h"
#include "BlockType.h"

namespace World {
    class Biome {
     protected:
        BlockType m_top_block_type = BlockType::Air;
        BlockType m_below_block_type = BlockType::Air;

        int m_tree_frequency = 0;
     protected:
        // more is rare
        std::vector<sf::Vector3i> m_tree_positions;
        int m_water_level;
     private:
        Biome();
     public:
        void clear() { m_tree_positions.clear(); };
        const std::vector<sf::Vector3i> &get_tree_positions() const;

        void set_tree_positions(const std::vector<sf::Vector3i> &treePositions);
        void set_tree_frequency(int tree_frequency);

        virtual ~Biome() = default;

        Biome(int water_level);

        virtual void generate_tree(std::function<void(sf::Vector3i, BlockType)> setter_callback) = 0;
        BlockType generate_block(sf::Vector3i block_pos, int ground_h, int block_global_y);
    };
}
