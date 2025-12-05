#include "mainScreen.hpp"
#include "saveLoad.hpp"
#include "app.hpp"

#include <functional>
#include <filesystem>
#include <fstream>
#include <print>

MainScreen::MainScreen(sf::RenderWindow& window, App& app) : titleText(AssetManager::getInstance().getFont("Roboto")),
	newProjectButton(window, window.getSize().y / 2.f, "New Project"), openProjectButton(window, window.getSize().y / 2.f + 150.f, "Open Project"),
	scrollMenu({700.f, 500.f}, window) {

	titleText.setCharacterSize(75);
	titleText.setString("Logic Simulator");
	titleText.setOrigin(titleText.getLocalBounds().getCenter());
	titleText.setPosition(sf::Vector2f(window.getSize().x / 2.f, 100.f));
	titleText.setFillColor(sf::Color::White);

	newProjectButton.onClick = [&app, &window]() {
		app.currentEditor = std::make_unique<Editor>(window, app);
		};
	openProjectButton.onClick = [this]() {
		scrollMenu.shouldDraw = !scrollMenu.shouldDraw;
		};

	for (const auto& entry : std::filesystem::directory_iterator(ASSETS_DIR + std::string("/saves"))) {
		if (entry.is_regular_file() && entry.path().extension() == ".ls") {
			std::ifstream in(entry.path(), std::ios::binary);
			if (!in) continue;
			auto saveName = readString(in);
			auto filePath = entry.path().string();
			scrollMenu.addItem(saveName, [filePath, &app, &window]() {
				app.currentEditor = std::make_unique<Editor>(window, app, filePath);
				loadCircuit(filePath, app.currentEditor->nodes, app.currentEditor->wires);
				app.currentEditor->currentSave = filePath;
				});
			scrollMenu.menuItems.back().onDelete = [this, entry]() {
				std::filesystem::remove(entry.path());
				};
		}
	}
}

void MainScreen::onEvent(sf::Event& event, sf::RenderWindow& window) {
	if (scrollMenu.shouldDraw) {
		scrollMenu.onEvent(event, window);
	} 
	else {
		newProjectButton.onEvent(event, window);
		openProjectButton.onEvent(event, window);
	}
}

void MainScreen::draw(sf::RenderWindow& window) {
	window.setView(window.getDefaultView());
	window.clear(sf::Color(100, 100, 100));

	window.draw(titleText);


	if (scrollMenu.shouldDraw) {
		scrollMenu.draw(window);
	}
	else {
		window.draw(newProjectButton);
		window.draw(openProjectButton);
	}

	window.display();
}