#pragma once

#include "pch.h"
#include "block_types.h"
#include "Side.h"

namespace World {
    struct DB {
        struct AtlasDb {
            std::array<sf::Vector2i,
                    static_cast<int>(block_id::BLOCK_ID_COUNT) * static_cast<int>(Side::SIDES_COUNT)> m_atlas_db;

            sf::Vector2i &operator()(block_id type, Side side) {
                return m_atlas_db.at(
                        static_cast<int>(type) * static_cast<int>(Side::SIDES_COUNT) + static_cast<int>(side));
            }
        };

    public:
        static std::vector<block_id> s_loaded_blocks;
        static AtlasDb s_atlas_db;

    private:
        /* eponymous */
        static void init_xyz(block_id id, const sf::Vector2i &pos);

        static void load_block(block_id id);

        static void init_xz(block_id id, const sf::Vector2i &pos);

        static void init_y(block_id id, const sf::Vector2i &pos);

        static void init_n_y(block_id id, const sf::Vector2i &pos);

        static void init_p_y(block_id id, const sf::Vector2i &pos);

    public:
        /* eponymous */
        static void load_blocks();
    };
}
