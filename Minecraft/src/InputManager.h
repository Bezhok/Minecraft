#pragma once
#include "pch.h"
#include "Directions.h"
#include "Subject.h"

class InputManager : public Subject {
 private:
  bool m_should_fix_cursor = true;
 public:
  void set_should_fix_cursor(bool should_fix_cursor);
 private:
  sf::RenderWindow &m_window;
 public:
  InputManager(sf::RenderWindow &window);
 public:
  void handle_mouse_movement(sf::Event &event);
  void handle_movement(sf::Event &event);
  static Directions get_direction(sf::Keyboard::Key key);
  void handle_keyboard(sf::Event &event);
  void handle_mouse(sf::Event &event);
  void handle_input(sf::Event &event);
};

