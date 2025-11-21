#pragma once
#include "../assetManager.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <print>

class TextInputBox : public sf::Drawable {
public:
	bool confirmed = false;

	TextInputBox(sf::Vector2f size = {}, std::string name = "") : text(AssetManager::getInstance().getFont("Roboto")),
		confirmText(AssetManager::getInstance().getFont("Roboto")), cancelText(AssetManager::getInstance().getFont("Roboto")) {
		setSize(size);
		shape.setFillColor(sf::Color(37, 37, 43));
		innerTextBox.setFillColor(sf::Color(20, 20, 20));
		innerTextBox.setOutlineThickness(2.f);
		innerTextBox.setOutlineColor(sf::Color::Black);
		confirmButton.setFillColor(sf::Color(62, 62, 62));
		cancelButton.setFillColor(sf::Color(73, 73, 82));
		confirmText.setString("Confirm");
		cancelText.setString("Cancel");
		cursor.setFillColor(sf::Color::Transparent);
	}

	void setPosition(sf::Vector2f position) {
		shape.setPosition(position);
		innerTextBox.setPosition(position + sf::Vector2f(shape.getSize().x / 2.f - innerTextBox.getSize().x / 2.f - 4.f, 
			shape.getSize().x / 2.f - innerTextBox.getSize().x / 2.f - 4.f));
		text.setPosition(innerTextBox.getGlobalBounds().getCenter() - sf::Vector2f(innerTextBox.getSize().x / 2.f - text.getGlobalBounds().size.x / 2.f - 10.f,
			0.f));
		cursor.setPosition(text.getPosition() + sf::Vector2f(text.getGlobalBounds().size.x / 2.f + cursor.getSize().x, -cursor.getSize().y / 2.f));
		cancelButton.setPosition(sf::Vector2f(innerTextBox.getPosition().x, innerTextBox.getPosition().y + innerTextBox.getSize().y + 22.f));
		confirmButton.setPosition(sf::Vector2f(innerTextBox.getPosition().x + innerTextBox.getSize().x - confirmButton.getSize().x,
			innerTextBox.getPosition().y + innerTextBox.getSize().y + 22.f));
		cancelText.setOrigin(cancelText.getLocalBounds().getCenter());
		confirmText.setOrigin(cancelText.getLocalBounds().getCenter());
		cancelText.setPosition(cancelButton.getGlobalBounds().getCenter());
		confirmText.setPosition(confirmButton.getGlobalBounds().getCenter());
	}

	void setSize(sf::Vector2f size) {
		shape.setSize(size);
		innerTextBox.setSize(size - sf::Vector2f(44.f, 22.f + size.y * 0.45f));
		text.setCharacterSize(innerTextBox.getSize().y * 0.5f);
		auto y = shape.getPosition().y + shape.getSize().y - 20.f - innerTextBox.getPosition().y - innerTextBox.getSize().y - 20.f;
		cancelButton.setSize({ (size.x - 60.f) / 2.f, y });
		confirmButton.setSize({ (size.x - 60.f) / 2.f, y  });
		cancelText.setCharacterSize(cancelButton.getSize().y * 0.75f);
		confirmText.setCharacterSize(confirmButton.getSize().y * 0.75f);
		cursor.setSize(sf::Vector2f(8.f, innerTextBox.getSize().y * 0.7f));

		setPosition(shape.getPosition());
	}

	sf::Vector2f getSize() const {
		return shape.getSize();
	}

	void update(sf::Time deltaTime) {
		dt += deltaTime;
		if (dt >= sf::seconds(0.75)) {
			dt = sf::seconds(0);
			if (cursor.getFillColor() == sf::Color::Transparent) {
				cursor.setFillColor(sf::Color::White);
			}
			else {
				cursor.setFillColor(sf::Color::Transparent);

			}
		}
	}

	void onEvent(sf::Event& event, sf::RenderWindow& window) {
		if (auto mouse = event.getIf<sf::Event::MouseButtonReleased>()) {
			if (mouse->button == sf::Mouse::Button::Left) {
				if (innerTextBox.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
					textBoxFocused = true;
					dt = sf::seconds(0);
					cursor.setPosition(text.getPosition() + sf::Vector2f(text.getGlobalBounds().size.x / 2.f + cursor.getSize().x / 2.f, -cursor.getSize().y / 2.f));
					cursor.setFillColor(sf::Color::White);
				}
				else {
					cursor.setFillColor(sf::Color::Transparent);
					textBoxFocused = false;
				}
				if (confirmButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
					confirmed = true;
					shouldDraw = false;
					text.setString("");
				}
				if (cancelButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
					shouldDraw = false;
					content.clear();
					text.setString("");
				}
			}
		}
		if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
			confirmButton.setFillColor(content.empty() ? sf::Color(62, 62, 62) : sf::Color(73, 73, 82));
			cancelButton.setFillColor(sf::Color(73, 73, 82));
			if (cancelButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
				cancelButton.setFillColor(sf::Color(72, 108, 233));
			}
			if (confirmButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
				confirmButton.setFillColor(sf::Color(72, 108, 233));
			}
		}

		if (auto entered = event.getIf<sf::Event::TextEntered>()) {
			if (textBoxFocused) {
				if (entered->unicode == 8) {
					if (!content.empty()) content.pop_back();
				}
				else if (entered->unicode < 128) {
					if (text.getString().getSize() <= 20) {
						content += static_cast<char>(entered->unicode);
					}
				}
				if (content.length() < 1) {
					confirmButton.setFillColor(sf::Color(62, 62, 62));
				}
				else {
					confirmButton.setFillColor(sf::Color(73, 73, 82));
				}
				text.setString(content);
				text.setOrigin(text.getLocalBounds().getCenter());
				text.setPosition(innerTextBox.getGlobalBounds().getCenter() - sf::Vector2f(innerTextBox.getSize().x / 2.f - text.getGlobalBounds().size.x / 2.f - 10.f,
					0.f));
				cursor.setPosition(text.getPosition() + sf::Vector2f(text.getGlobalBounds().size.x / 2.f + cursor.getSize().x / 2.f, -cursor.getSize().y / 2.f));
			}
			
		}
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(shape, states);
		target.draw(innerTextBox, states);
		target.draw(text);
		if (textBoxFocused) {
			target.draw(cursor);
		}
		target.draw(cancelButton);
		target.draw(confirmButton);
		target.draw(cancelText);
		target.draw(confirmText);
	}

	bool shouldDraw = false;
	std::string content;

private:
	sf::RectangleShape shape;
	sf::RectangleShape innerTextBox;
	sf::RectangleShape cursor;
	sf::RectangleShape confirmButton;
	sf::RectangleShape cancelButton;
	sf::Text confirmText;
	sf::Text cancelText;
	sf::Text text;
	bool textBoxFocused = false;
	sf::Time dt;
};