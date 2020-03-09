#pragma once

#include "pch.h"
#include "BlockType.h"

class Inventory {
public:
    std::vector<std::pair<World::BlockType, int>> &get_hot_bar_items();

    Inventory();

    World::BlockType get_curr_block() const;

    void set_cur_hot_bar_item(int idx);

    void change_curr_hot_bar_item(int delta);

private:
    std::vector<std::pair<World::BlockType, int>> m_hot_bar_items;
    std::vector<std::pair<World::BlockType, int>> m_main_items;
    World::BlockType m_curr_block;
    int m_curr_block_index = 0;
};
