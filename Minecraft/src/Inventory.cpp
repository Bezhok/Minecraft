#include "Inventory.h"
#include "BlockType.h"
#include "game_constants.h"

Inventory::Inventory() {
    int size = 9;
    m_hot_bar_items.resize(HOTBAR_INV_SIZE, {World::BlockType::EMPTY_TYPE, 0});
    m_main_items.resize(MAIN_INV_SIZE, {World::BlockType::EMPTY_TYPE, 0});
}

World::BlockType Inventory::get_curr_block() const {
    return m_hot_bar_items[m_curr_block_index].first;
}

void Inventory::change_curr_hot_bar_item(int delta) {
    m_curr_block_index += delta;
    int vec_size = static_cast<int>(m_hot_bar_items.size());
    while (m_curr_block_index < 0) m_curr_block_index += vec_size;
    set_cur_hot_bar_item(m_curr_block_index);
}

void Inventory::set_cur_hot_bar_item(int idx) {
    int vec_size = static_cast<int>(m_hot_bar_items.size());
    m_curr_block_index = glm::max(0, idx) % vec_size;
}

std::vector<std::pair<World::BlockType, int>> &Inventory::get_hot_bar_items() {
    return m_hot_bar_items;
}
