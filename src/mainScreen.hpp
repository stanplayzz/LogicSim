#pragma once
#include "assetManager.hpp"
#include "ui/scrollMenu.hpp"

#include <SFML/Graphics.hpp>
#include <functional>

struct Button : public sf::Drawable {
	sf::Vector2f buttonSize{ 500.f, 125.f };
	sf::Vector2f buttonPos{};
	sf::RectangleShape shape{ buttonSize };
	sf::Text text;

	Button(sf::RenderWindow& window, float y, std::string text) : text(AssetManager::getInstance().getFont("Roboto")) {
		shape.setFillColor(sf::Color(100, 100, 150));
		shape.setPosition(sf::Vector2f(window.getSize().x / 2.f, y) - shape.getSize() / 2.f);
		shape.setOutlineColor(sf::Color(80, 80, 120));
		shape.setOutlineThickness(5);
		buttonPos = shape.getPosition();
		this->text.setFillColor(sf::Color::White);
		this->text.setCharacterSize(shape.getSize().y * 0.6f);
		this->text.setString(text);
		this->text.setOrigin(this->text.getLocalBounds().getCenter());
		this->text.setPosition(shape.getPosition() + shape.getSize() / 2.f);
	}

	void onEvent(sf::Event& event, sf::RenderWindow& window) {
		if (auto mouse = event.getIf<sf::Event::MouseButtonReleased>()) {
			if (mouse->button == sf::Mouse::Button::Left) {
				if (shape.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
					onClick();
				}
			}
		}
		if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
			if (shape.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				shape.setSize(buttonSize * 1.05f);
				shape.setPosition(buttonPos - buttonSize * .025f);
			}else{
				shape.setSize(buttonSize);
				shape.setPosition(buttonPos);
			}
		}
	}
	std::function<void()> onClick;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(shape, states);
		target.draw(text, states);
	}
	
};

class App;

class MainScreen {
public:
	MainScreen(sf::RenderWindow& window, App& app);
	void onEvent(sf::Event& event, sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);

	ScrollMenu scrollMenu;
	bool drawit = false;

private:
	sf::Text titleText;
	Button newProjectButton;
	Button openProjectButton;
};