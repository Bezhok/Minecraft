#pragma once

#include "Biome.h"

namespace World {
    class DesertBiome :
            public Biome {

    public:
        ~DesertBiome() override;

        void generate_tree(std::function<void(sf::Vector3i, BlockType)> setter_callback) override;

        DesertBiome(int water_level);
    };
}
