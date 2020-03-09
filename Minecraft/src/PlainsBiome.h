#pragma once

#include "Biome.h"

namespace World {
    class PlainsBiome :
            public Biome {
    public:
        ~PlainsBiome() override;

        void generate_tree(std::function<void(sf::Vector3i, BlockType)> setter_callback) override;

        PlainsBiome(int water_level);
    };
}

