#include "toolbar.hpp"
#include "app.hpp"

// toolbar
Toolbar::Toolbar(sf::RenderWindow& window, App& app) : menu(window, app) {
	background.setSize({ (float)window.getSize().x, 30.f });
	background.setPosition({ 0.f, (float)window.getSize().y - background.getSize().y });
	background.setFillColor(sf::Color(40, 40, 40));

	menuButton.text.setString("Menu");
	menuButton.setPosition(background.getPosition());
}

void Toolbar::onEvent(sf::Event& event, sf::RenderWindow& window) {
	menuButton.onEvent(event, window);
	menu.onEvent(event, window);

	if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
		if (menuButton.background.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView())) ||
			(drawMenu && menu.background.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView())))) {
			drawMenu = true;
		}
		else {
			drawMenu = false;
		}
	}
}

void Toolbar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (drawMenu)
		target.draw(menu, states);
	target.draw(background, states);
	target.draw(menuButton, states);
}

// toolbar items
namespace ToolbarUI {
	Item::Item(sf::Vector2f size, bool onToolbar) : text(AssetManager::getInstance().getFont("Roboto")) {
		background.setFillColor(sf::Color::Transparent);
		background.setSize(size);
		text.setCharacterSize(24);
		if (!onToolbar) {
			text.setCharacterSize(20);
		}
		this->onToolbar = onToolbar;
	}
	void Item::setPosition(sf::Vector2f pos) {
		background.setPosition(pos);
		if (onToolbar) {
			text.setOrigin(text.getLocalBounds().getCenter());
			text.setPosition(pos + background.getSize() / 2.f);
		}
		else {
			text.setPosition(pos + sf::Vector2f(3.f, 0.f));
		}
	}
	void Item::onEvent(sf::Event& event, sf::RenderWindow& window) {
		if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
			if (background.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
				background.setFillColor(sf::Color(255, 255, 255, 20));
			}
			else {
				background.setFillColor(sf::Color::Transparent);
			}
		}
		if (auto mouse = event.getIf<sf::Event::MouseButtonReleased>()) {
			if (mouse->button == sf::Mouse::Button::Left) {
				if (background.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position, window.getDefaultView()))) {
					if (onClick) onClick();
				}
			}
		}
	}
	void Item::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(background, states);
		target.draw(text, states);
	}
}

// menu
Menu::Menu(sf::RenderWindow& window, App& app) {
	background.setFillColor(sf::Color(40, 40, 40));

	ToolbarUI::Item saveAsNew{ {200.f, 30.f}, false };
	saveAsNew.text.setString("Save As New");
	saveAsNew.onClick = [&app, &window]() {
		if (app.currentEditor) app.currentEditor->save(window);
		};
	items.push_back(saveAsNew);
	ToolbarUI::Item makeNode{ {200.f, 30.f}, false };
	makeNode.text.setString("Make Chip");
	makeNode.onClick = [&app, &window]() {
		if (app.currentEditor) app.currentEditor->makeChip(window);
		};
	items.push_back(makeNode);
	ToolbarUI::Item quitToMainMenu{ {200.f, 30.f}, false };
	quitToMainMenu.text.setString("Quit To Main Menu");
	quitToMainMenu.onClick = [app = &app]() {
		app->toMainMenu = true;
		};
	items.push_back(quitToMainMenu);
	ToolbarUI::Item quitToDesktop{ {200.f, 30.f}, false };
	quitToDesktop.text.setString("Quit To Desktop");
	quitToDesktop.onClick = [&window]() {
		window.close();
		};
	items.push_back(quitToDesktop);

	background.setSize({ 200.f, items.size() * 30.f + 30.f });
	background.setPosition(sf::Vector2f(0.f, window.getSize().y - background.getSize().y));

	for (int i = 0; i < items.size(); i++) {
		items[i].setPosition(background.getPosition() + sf::Vector2f(0.f, i * 30.f));
	}

}
void Menu::onEvent(sf::Event& event, sf::RenderWindow& window) {
	for (auto& item : items) {
		item.onEvent(event, window);
	}
}
void Menu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(background, states);
	for (auto& item : items) {
		target.draw(item, states);
	}
}