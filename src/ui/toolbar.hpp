#pragma once
#include "../assetManager.hpp"

#include <SFML/Graphics.hpp>
#include <functional>
#include <print>

class App;

namespace ToolbarUI {
	struct Item : public sf::Drawable {
		sf::RectangleShape background;
		sf::Text text;
		std::function<void()> onClick;

		bool onToolbar = false;

		Item(sf::Vector2f size = sf::Vector2f(70.f, 30.f), bool onToolbar = true);
		void setPosition(sf::Vector2f pos);
		void onEvent(sf::Event& event, sf::RenderWindow& window);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};
};

//struct ChipsContainer : public sf::Drawable {
//	ChipsContainer();
//
//	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
//};

struct Menu : public sf::Drawable {
	sf::RectangleShape background;
	std::vector<ToolbarUI::Item> items;

	Menu(sf::RenderWindow& window, App& app);
	void onEvent(sf::Event& event, sf::RenderWindow& window);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};


class Toolbar : public sf::Drawable {
public:
	Toolbar(sf::RenderWindow& window, App& app);
	void onEvent(sf::Event& event, sf::RenderWindow& window);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	Menu menu;
private:
	sf::RectangleShape background;
	ToolbarUI::Item menuButton;

	bool drawMenu = false;
};