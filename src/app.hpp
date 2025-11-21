#pragma once
#include "editor.hpp"
#include <SFML/Graphics.hpp>

class App {
public:
	App();
	void run();
private:
	sf::RenderWindow window;

	std::unique_ptr<Editor> currentEditor = nullptr;
};