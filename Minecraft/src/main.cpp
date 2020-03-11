#include "pch.h"
#include "App.h"
#include "game_constants.h"

int main() {
    sf::RenderWindow window(
            sf::VideoMode(
                    600,
                    600
            ),
            "Minecraft",
            sf::Style::Fullscreen,
            sf::ContextSettings(24, 0, 0)
    );

    //window.setPosition(window.getPosition() + sf::Vector2i{200, 0});
    window.setKeyRepeatEnabled(false);
    //window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings = window.getSettings();

    std::cout << "depth bits:" << settings.depthBits << std::endl;
    std::cout << "stencil bits:" << settings.stencilBits << std::endl;
    std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
    std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;

    App app(window);
    app.run();

    return 0;
}



