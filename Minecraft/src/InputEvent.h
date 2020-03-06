#pragma once
#include "Subject.h"
#include "Directions.h"
#include "EventType.h"

class InputEvent: public Subject {
 public:
  Directions direction = Directions::NONE;
  EventType type;
  sf::Vector2f delta;
  int idx = -1;

  bool is_begin = false;
  bool is_end = false;
};
