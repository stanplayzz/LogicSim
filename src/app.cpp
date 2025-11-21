#include "app.hpp"
#include "assetManager.hpp"

App::App() {
	window.create(sf::VideoMode({ 1280, 720 }), "Logic Simulator", sf::Style::Close);
	AssetManager::getInstance().loadFont("Roboto", "/fonts/Roboto-Regular.ttf");
}

void App::run() {
	sf::Clock clock;
	while (window.isOpen()) {
		sf::Time deltaTime = clock.restart();
		while (auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (auto key = event->getIf<sf::Event::KeyReleased>()) {
				if (key->scancode == sf::Keyboard::Scancode::P) {
					currentEditor = std::make_unique<Editor>(window);
				}
			}
			if (currentEditor) {
				currentEditor->onEvent(*event, window);
			}
		}
		if (currentEditor) {
			currentEditor->update(deltaTime, window);
			currentEditor->draw(window);
		}
	}
}