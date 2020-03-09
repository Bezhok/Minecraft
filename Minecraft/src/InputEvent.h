#pragma once

#include "Subject.h"
#include "Direction.h"
#include "EventType.h"

class InputEvent : public Subject {
public:
    Direction direction = Direction::NONE;
    EventType type;
    sf::Vector2f delta;
    int idx = -1;

    bool is_begin = false;
    bool is_end = false;
};
