#pragma once
#include "nodes/node.hpp"
#include "nodes/wire.hpp"
#include "ui/textInputBox.hpp"
#include "ui/contextMenu.hpp"
#include <SFML/Graphics.hpp>

#include <map>

class Editor {
public:
	Editor(sf::RenderWindow& window);
	void update(sf::Time& deltaTime, sf::RenderWindow& window);
	void onEvent(sf::Event& event, sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
private:
	sf::View view;
	float currentZoom = 1.f;
	bool panning = false;
	sf::Vector2i prevMouse{};
	sf::Vector2f nodeDragOffset{};
	Node* currentNode = nullptr;
	Pin* currentPin = nullptr;
	bool isAddingWire = false;

	float tileSize = 32.f;
	sf::Vector2f worldSize{ 500.f, 500.f };
	std::vector<std::unique_ptr<Node>> nodes;
	std::vector<Wire> wires;

	// UI
	std::unique_ptr<TextInputBox> textInputBox;
	ContextMenu contextMenu;
	sf::RectangleShape darkenOverlay;
};