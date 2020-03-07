#pragma once

#include "game_constants.h"

namespace World {
    class Converter {
     public:
        template<typename T>
        static int coord2chunk_coord(T c);

        template<typename T>
        static int coord2block_coord(T c);

        template<typename T>
        static sf::Vector3<int> coord2block_coord(sf::Vector3<T> c);

        template<typename T>
        static int coord2block_coord_in_chunk(T c);//block in chunk coord

        template<typename T>
        static int chunk_coord2block_coord(T c);
    };

    template<typename T>
    int Converter::coord2chunk_coord(T c) {
        return static_cast<int>(c / BLOCKS_IN_CHUNK);
    }

    template<typename T>
    int Converter::coord2block_coord(T c) {
        return static_cast<int>(c);
    }

    template<typename T>
    int Converter::coord2block_coord_in_chunk(T c) {
        return static_cast<int>(coord2block_coord(c) % BLOCKS_IN_CHUNK);
    }

    template<typename T>
    inline int Converter::chunk_coord2block_coord(T c) {
        return static_cast<int>(c * static_cast<T>(BLOCKS_IN_CHUNK));
    }

    template<typename T>
    sf::Vector3<int> Converter::coord2block_coord(sf::Vector3<T> c) {
        return {coord2block_coord(c.x), coord2block_coord(c.y), coord2block_coord(c.z)};
    }
}