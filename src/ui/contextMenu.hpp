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
	sf::Text inspectText;

public:
	Node* node = nullptr;
	Wire* wire = nullptr;
	bool shouldDraw = false;
	sf::Text titleText;
	sf::RectangleShape titleBox{ {width, labelHeight} };
	sf::RectangleShape inspectButton{ {width, labelHeight} };
	sf::RectangleShape deleteButton{ {width, labelHeight} };

	ContextMenu() : titleText(AssetManager::getInstance().getFont("Roboto")), deleteText(AssetManager::getInstance().getFont("Roboto")),
		inspectText(AssetManager::getInstance().getFont("Roboto")) {
		background.setSize({ width + padding * 2, 2 * labelHeight + 2 * padding });
		background.setFillColor(sf::Color(232, 232, 232));
		titleBox.setFillColor(sf::Color(46, 46, 46));
		inspectText.setString("Inspect");
		deleteText.setString("Delete");
		inspectText.setCharacterSize(35);
		deleteText.setCharacterSize(35);
		titleText.setCharacterSize(35);
		inspectText.setFillColor(sf::Color::Black);
		deleteText.setFillColor(sf::Color::Black);
	}

	void onEvent(sf::Event& event, sf::RenderWindow& window) {
		if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
			deleteButton.setFillColor(sf::Color::White);
			if (deleteButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
				deleteButton.setFillColor(sf::Color(130, 190, 245));
			}
			inspectButton.setFillColor(sf::Color::White);
			if (inspectButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView())) && node && node->nodeType == NodeType::Chip) {
				inspectButton.setFillColor(sf::Color(130, 190, 245));
			}
		}
	}

	void setPosition(sf::Vector2f pos) {
		background.setPosition(pos);
		titleBox.setPosition(pos + sf::Vector2f(padding, padding));
		inspectButton.setPosition(titleBox.getPosition() + sf::Vector2f(0.f, titleBox.getSize().y));
		deleteButton.setPosition(titleBox.getPosition() + sf::Vector2f(0.f, titleBox.getSize().y * 2.f));
		deleteText.setOrigin(deleteText.getLocalBounds().getCenter());
		titleText.setOrigin(titleText.getLocalBounds().getCenter());
		inspectText.setOrigin(inspectText.getLocalBounds().getCenter());
		deleteText.setPosition(deleteButton.getPosition() + deleteButton.getSize() / 2.f);
		inspectText.setPosition(inspectButton.getPosition() + inspectButton.getSize() / 2.f);
		titleText.setPosition(titleBox.getPosition() + titleBox.getSize() / 2.f);
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(background);
		target.draw(titleBox);
		target.draw(inspectButton);
		target.draw(inspectText);
		target.draw(deleteButton);
		target.draw(titleText);
		target.draw(deleteText);
	}
};