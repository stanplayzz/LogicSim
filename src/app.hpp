#pragma once
#include "editor.hpp"
#include "mainScreen.hpp"

#include <SFML/Graphics.hpp>

class App {
public:
	App();
	void run();


	bool toMainMenu = false;
	std::unique_ptr<Editor> currentEditor = nullptr;
	std::unique_ptr<MainScreen> mainScreen = nullptr;
private:
	sf::RenderWindow window;

};