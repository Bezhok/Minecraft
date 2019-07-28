#include "pch.h"
#include "App.h"
#include "game_constants.h"


int main()
{
	int x = 0;
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGTH), "Minecraft", sf::Style::Default, sf::ContextSettings(24));
	window.setPosition(window.getPosition() + sf::Vector2i{200, 0});
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	App app(window);
	app.run();


	return 0;
}



