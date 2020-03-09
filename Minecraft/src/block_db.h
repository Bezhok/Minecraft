#pragma once

#include "pch.h"
#include "BlockType.h"
#include "Side.h"

namespace World {
    struct DB {
        struct AtlasDb {
            std::array<sf::Vector2i,
                    static_cast<int>(BlockType::BLOCK_ID_COUNT) * static_cast<int>(Side::SIDES_COUNT)> m_atlas_db;

            sf::Vector2i &operator()(BlockType type, Side side) {
                return m_atlas_db.at(
                        static_cast<int>(type) * static_cast<int>(Side::SIDES_COUNT) + static_cast<int>(side));
            }
        };

    public:
        /* eponymous */
        static void load_blocks();

        static std::vector<BlockType> s_loaded_blocks;
        static AtlasDb s_atlas_db;

    private:
        /* eponymous */
        static void init_xyz(BlockType id, const sf::Vector2i &pos);

        static void load_block(BlockType id);

        static void init_xz(BlockType id, const sf::Vector2i &pos);

        static void init_y(BlockType id, const sf::Vector2i &pos);

        static void init_n_y(BlockType id, const sf::Vector2i &pos);

        static void init_p_y(BlockType id, const sf::Vector2i &pos);
    };
}
