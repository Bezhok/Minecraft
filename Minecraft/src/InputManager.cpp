#include "InputManager.h"
#include "Directions.h"
#include "EventType.h"
#include "InputEvent.h"

void InputManager::handle_input(sf::Event &event) {
    handle_keyboard(event);
    handle_mouse(event);
}

void InputManager::handle_mouse_movement(sf::Event &event) {
    InputEvent input_event;
    input_event.type = EventType::CAMERA_ROTATION;
    sf::Vector2i mouse_xy = sf::Mouse::getPosition(m_window);

    // center coordinates
    int x = m_window.getSize().x / 2;
    int y = m_window.getSize().y / 2;
    input_event.delta.x += float(x - mouse_xy.x) / 12;
    input_event.delta.y += float(y - mouse_xy.y) / 12;
    sf::Mouse::setPosition(sf::Vector2i(x, y), m_window);

    notify(&input_event);
}

void InputManager::handle_movement(sf::Event &event) {
    InputEvent input_event;
    Directions dir = Directions::NONE;
    input_event.type = EventType::MOVEMENT;
    switch (event.type) {
        case sf::Event::KeyPressed: {
            dir = get_direction(event.key.code);
            input_event.is_begin = true;
            break;
        }
        case sf::Event::KeyReleased: {
            dir = get_direction(event.key.code);
            input_event.is_end = true;
            break;
        }
    }
    if (dir != Directions::NONE) {
        input_event.direction = dir;
        notify(&input_event);
    }
}

void InputManager::handle_keyboard(sf::Event &event) {
    handle_movement(event);
    InputEvent input_event;

    switch (event.type) {
        case sf::Event::KeyReleased: {
            if (event.key.code == sf::Keyboard::RShift) {
                input_event.type = EventType::RSHIFT_RELEASED;
                notify(&input_event);
            }
            break;
        }
    }
}

void InputManager::handle_mouse(sf::Event &event) {
    InputEvent input_event;

    static sf::Clock mouse_timer;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        if (mouse_timer.getElapsedTime().asMilliseconds() > 300) {
            input_event = InputEvent();
            input_event.type = EventType::CREATE;
            notify(&input_event);
            mouse_timer.restart();
        }
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (mouse_timer.getElapsedTime().asMilliseconds() > 250) {
            input_event = InputEvent();
            input_event.type = EventType::REMOVE;
            notify(&input_event);
            mouse_timer.restart();
        }
    } else if (event.type == sf::Event::MouseWheelMoved) {
        input_event = InputEvent();
        input_event.type = EventType::CHANGE_INV_ITEM;
        input_event.delta.x = event.mouseWheel.delta;
        notify(&input_event);
    }

    if (m_should_fix_cursor) {
        handle_mouse_movement(event);
    }
}

Directions InputManager::get_direction(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::W:return Directions::FORWARD;
        case sf::Keyboard::A:return Directions::LEFT;
        case sf::Keyboard::S:return Directions::BACK;
        case sf::Keyboard::D:return Directions::RIGHT;
        case sf::Keyboard::Space:return Directions::UP;
        case sf::Keyboard::LShift:return Directions::DOWN;
        default:return Directions::NONE;
    }
}

InputManager::InputManager(sf::RenderWindow &window) : m_window(window) {

}
void InputManager::set_should_fix_cursor(bool should_fix_cursor) {
    m_should_fix_cursor = should_fix_cursor;
}
