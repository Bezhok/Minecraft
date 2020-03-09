#pragma once

#include "pch.h"
#include "Direction.h"
#include "Subject.h"

class InputManager : public Subject {
public:
    InputManager(sf::RenderWindow &window);

    void handle_mouse_movement(sf::Event &event);

    void handle_movement(sf::Event &event);

    static Direction get_direction(sf::Keyboard::Key key);

    void handle_keyboard(sf::Event &event);

    void handle_mouse(sf::Event &event);

    void handle_input(sf::Event &event);

    void set_should_fix_cursor(bool should_fix_cursor);

private:
    bool m_should_fix_cursor = true;
    sf::RenderWindow &m_window;
};

