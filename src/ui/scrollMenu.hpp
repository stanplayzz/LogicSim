#pragma once
#include "assetManager.hpp"

#include <SFML/Graphics.hpp>
#include <functional>
#include <print>

class ScrollMenu {
public:
	struct MenuItem {
		sf::RectangleShape box;
		sf::Text text;

		std::function<void()> onUse;
		std::function<void()> onDelete;

		MenuItem() : text(AssetManager::getInstance().getFont("Roboto")) {
		}


	};

	ScrollMenu(sf::Vector2f size, sf::RenderWindow& window) : backText(AssetManager::getInstance().getFont("Roboto")), 
		deleteText(AssetManager::getInstance().getFont("Roboto")), openText(AssetManager::getInstance().getFont("Roboto")) {
		box.setSize(size);
		box.setFillColor(sf::Color(100, 100, 150));
		box.setOutlineColor(sf::Color(80, 80, 120));
		box.setOutlineThickness(5);
		auto pos = window.getDefaultView().getCenter() - size / 2.f + sf::Vector2f(box.getOutlineThickness(), box.getOutlineThickness());
		box.setPosition(pos);

		deleteButton.setPosition(box.getPosition() + sf::Vector2f(box.getSize().x / 2.f - backButton.getSize().x  / 2.f, box.getSize().y - backButton.getSize().y - 20.f));
		deleteButton.setFillColor(sf::Color(62, 62, 62));
		backButton.setPosition(deleteButton.getPosition() - sf::Vector2f(backButton.getSize().x + 5.f, 0.f));
		backButton.setFillColor(sf::Color(73, 73, 82));
		openButton.setPosition(deleteButton.getPosition() + sf::Vector2f(openButton.getSize().x + 5.f, 0.f));
		openButton.setFillColor(sf::Color(62, 62, 62));

		backText.setString("Back");
		backText.setCharacterSize(30);
		backText.setFillColor(sf::Color::White);
		backText.setOrigin(backText.getLocalBounds().getCenter());
		backText.setPosition(backButton.getPosition() + backButton.getSize() / 2.f);
		deleteText.setString("Delete");
		deleteText.setCharacterSize(30);
		deleteText.setFillColor(sf::Color::White);
		deleteText.setOrigin(deleteText.getLocalBounds().getCenter());
		deleteText.setPosition(deleteButton.getPosition() + deleteButton.getSize() / 2.f);
		openText.setString("Open");
		openText.setCharacterSize(30);
		openText.setFillColor(sf::Color::White);
		openText.setOrigin(openText.getLocalBounds().getCenter());
		openText.setPosition(openButton.getPosition() + openButton.getSize() / 2.f);

		menuView.setViewport(sf::FloatRect({ pos.x / window.getSize().x, pos.y / window.getSize().y }, 
			{size.x / window.getSize().x, (size.y - backButton.getSize().y - 25.f) / window.getSize().y }));
		menuView.setSize({size.x, size.y - backButton.getSize().y - 25.f });
		menuView.setCenter(sf::Vector2f(size.x, size.y - backButton.getSize().y - 25.f) / 2.f);
	}

	void refresh() {
		float y = 5.f;
		for (auto& it : menuItems) {
			it.box.setPosition({ (box.getSize().x - it.box.getSize().x - it.box.getOutlineThickness() * 2.f) / 2.f , y });
			it.text.setPosition(it.box.getGlobalBounds().getCenter());
			y += it.box.getSize().y + 5.f;
		}
	}

	void addItem(const std::string& label, std::function<void()> onUse) {
		MenuItem item;

		item.box.setSize(sf::Vector2f(box.getSize().x * 0.95f, box.getSize().x * 0.95f * 0.15f));
		item.box.setFillColor(sf::Color(80, 80, 120));
		item.box.setOutlineColor(sf::Color::Black);
		item.box.setOutlineThickness(2);

		item.text.setString(label);
		item.text.setFillColor(sf::Color::White);
		item.text.setCharacterSize(24);
		item.text.setOrigin(item.text.getLocalBounds().getCenter());

		item.onUse = onUse;

		menuItems.push_back(item);

		float y = 5.f;
		for (auto& it : menuItems) {
			it.box.setPosition({ (box.getSize().x - it.box.getSize().x - it.box.getOutlineThickness() * 2.f) / 2.f , y});
			it.text.setPosition(it.box.getGlobalBounds().getCenter());
			y += it.box.getSize().y + 5.f;
		}

		
	}

	void onEvent(sf::Event& event, sf::RenderWindow& window) {
		if (!shouldDraw) return;
		if (auto scroll = event.getIf<sf::Event::MouseWheelScrolled>()) {
			if (!menuItems.empty()) {
				scrollOffset -= scroll->delta * 30.f;

				float maxScroll = menuItems.size() * menuItems[0].box.getSize().y;
				if (scrollOffset < 0) scrollOffset = 0;
				if (scrollOffset > maxScroll) scrollOffset = maxScroll;

				menuView.setCenter({ menuView.getSize().x / 2.f, menuView.getSize().y / 2.f + scrollOffset });
			}
		}
		if (auto mouse = event.getIf<sf::Event::MouseButtonReleased>()) {
			if (mouse->button == sf::Mouse::Button::Left) {
				MenuItem* clicked = nullptr;

				for (auto& it : menuItems) {
					if (it.box.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, menuView))) {
						clicked = &it;
						break;
					}
				}

				if (clicked) {
					if (currentItem == clicked) {
						clicked->box.setFillColor(sf::Color(80, 80, 120));
						currentItem = nullptr;
						openButton.setFillColor(sf::Color(62, 62, 62));
						deleteButton.setFillColor(sf::Color(62, 62, 62));
					}
					else {
						if (currentItem)
							currentItem->box.setFillColor(sf::Color(80, 80, 120));

						clicked->box.setFillColor(sf::Color(125, 125, 200));
						currentItem = clicked;
						openButton.setFillColor(sf::Color(73, 73, 82));
						deleteButton.setFillColor(sf::Color(73, 73, 82));
					}
				}
				if (openButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
					if (currentItem) {
						currentItem->onUse();
						currentItem->box.setFillColor(sf::Color(80, 80, 120));
						currentItem = nullptr;
						openButton.setFillColor(sf::Color(62, 62, 62));
						deleteButton.setFillColor(sf::Color(62, 62, 62));
						shouldDraw = false;
					}
				}
				if (deleteButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
					if (currentItem) {
						auto it = std::find_if(menuItems.begin(), menuItems.end(), [&](const MenuItem& item) {
							return &item == currentItem;
							});
						if (it != menuItems.end()) {
							currentItem->onDelete();
							menuItems.erase(it);
							currentItem = nullptr;
							openButton.setFillColor(sf::Color(62, 62, 62));
							deleteButton.setFillColor(sf::Color(62, 62, 62));
							refresh();
						}
					}
				}
				if (backButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
					shouldDraw = false;
					if (currentItem) {
						currentItem->box.setFillColor(sf::Color(80, 80, 120));
						currentItem = nullptr;
						openButton.setFillColor(sf::Color(62, 62, 62));
						deleteButton.setFillColor(sf::Color(62, 62, 62));
					}
				}
			}
		}
		if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
			if (backButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				backButton.setFillColor(sf::Color(72, 108, 233));
			}
			else {
				backButton.setFillColor(sf::Color(73, 73, 82));
			}
			if (deleteButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position)) && currentItem) {
				deleteButton.setFillColor(sf::Color(72, 108, 233));
			}
			else {
				deleteButton.setFillColor(currentItem ? sf::Color(73, 73, 82) : sf::Color(62, 62, 62));
			}
			if (openButton.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position)) && currentItem) {
				openButton.setFillColor(sf::Color(72, 108, 233));
			}
			else {
				openButton.setFillColor(currentItem ? sf::Color(73, 73, 82) : sf::Color(62, 62, 62));
			}
		}
	}

	void draw(sf::RenderTarget& target) const {
		if (!shouldDraw) return;
		target.draw(box);

		sf::View old = target.getView();
		target.setView(menuView);

		for (const auto& item : menuItems) {
			target.draw(item.box);
			target.draw(item.text);
		}


		target.setView(old);
		target.draw(backButton);
		target.draw(backText);
		target.draw(deleteButton);
		target.draw(deleteText);
		target.draw(openButton);
		target.draw(openText);
	}
	
	std::vector<MenuItem> menuItems;
	bool shouldDraw = false;
	MenuItem* currentItem = nullptr;
private:
	sf::RectangleShape box;
	sf::RectangleShape backButton{ {200.f, 50.f} };
	sf::Text backText;
	sf::RectangleShape deleteButton{ {200.f, 50.f} };
	sf::Text deleteText;
	sf::RectangleShape openButton{ {200.f, 50.f} };
	sf::Text openText;


	sf::View menuView;

	float scrollOffset = 0.f;
};