#pragma once
#include "../nodes/chip.hpp"
#include "../nodes/wire.hpp"
#include "../assetManager.hpp"


class ContextMenu : public sf::Drawable {
private:
	float width = 196.f;
	float labelHeight = 42.f;
	float padding = 4.f;

	sf::RectangleShape background;
	sf::Text deleteText;

public:
	Node* node = nullptr;
	Wire* wire = nullptr;
	bool shouldDraw = false;
	sf::Text titleText;
	sf::RectangleShape titleBox{ {width, labelHeight} };
	sf::RectangleShape deleteButton{ {width, labelHeight} };

	ContextMenu() : titleText(AssetManager::getInstance().getFont("Roboto")), deleteText(AssetManager::getInstance().getFont("Roboto")) {
		background.setSize({ width + padding * 2, 2 * labelHeight + 2 * padding });
		background.setFillColor(sf::Color(232, 232, 232));
		titleBox.setFillColor(sf::Color(46, 46, 46));
		deleteText.setString("Delete");
		deleteText.setCharacterSize(35);
		titleText.setCharacterSize(35);
		deleteText.setFillColor(sf::Color::Black);
	}

	void onEvent(sf::Event& event, sf::RenderWindow& window) {
		if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
			deleteButton.setFillColor(sf::Color::White);
			if (deleteButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
				deleteButton.setFillColor(sf::Color(130, 190, 245));
			}
		}
	}

	void setPosition(sf::Vector2f pos) {
		background.setPosition(pos);
		titleBox.setPosition(pos + sf::Vector2f(padding, padding));
		deleteButton.setPosition(titleBox.getPosition() + sf::Vector2f(0.f, titleBox.getSize().y));
		deleteText.setOrigin(deleteText.getLocalBounds().getCenter());
		titleText.setOrigin(titleText.getLocalBounds().getCenter());
		deleteText.setPosition(deleteButton.getPosition() + deleteButton.getSize() / 2.f);
		titleText.setPosition(titleBox.getPosition() + titleBox.getSize() / 2.f);
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(background);
		target.draw(titleBox);
		target.draw(deleteButton);
		target.draw(titleText);
		target.draw(deleteText);
	}
};