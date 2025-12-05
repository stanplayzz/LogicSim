#include "app.hpp"
#include "assetManager.hpp"

App::App() {
	window.create(sf::VideoMode({ 1280, 720 }), "Logic Simulator", sf::Style::Close);
	AssetManager::getInstance().loadFont("Roboto", "/fonts/Roboto-Regular.ttf");
	window.setVerticalSyncEnabled(true);
}

void App::run() {

	mainScreen = std::make_unique<MainScreen>(window, *this);

	sf::Clock clock;
	while (window.isOpen()) {
		sf::Time deltaTime = clock.restart();
		while (auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (currentEditor) {
				currentEditor->onEvent(*event, window);
			}
			else {
				mainScreen->onEvent(*event, window);
			}
		}
		if (currentEditor) {
			currentEditor->update(deltaTime, window);
			currentEditor->draw(window);
		}
		else {
			mainScreen->draw(window);
		}

		if (toMainMenu) {
			toMainMenu = false;
			currentEditor.reset();
			mainScreen = std::make_unique<MainScreen>(window, *this);
		}
	}
}