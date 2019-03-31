#include "pch.h"
#include "App.h"
#include "game_constants.h"

int main()
{
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Minecraft", sf::Style::Default, sf::ContextSettings(24));
	window.setVerticalSyncEnabled(true);

	App app(window);
	app.run();


	return 0;
}

