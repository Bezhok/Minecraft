#pragma once

#include <cstdint>

namespace World {
    enum class BlockType : uint8_t {
        Air = 0,
        Glass,
        Water,

        Sand,
        Grass,
        Dirt,
        Stone,
        Oak,
        Oak_leafage,
        Cactus,
        Oak_wood,

        concrete_black,
        concrete_blue,
        concrete_brown,
        concrete_cyan,
        concrete_gray,
        concrete_green,

        transparent_type,
        EMPTY_TYPE,
        BLOCK_ID_COUNT
    };
}