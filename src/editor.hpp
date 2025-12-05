#pragma once
#include "nodes/node.hpp"
#include "nodes/wire.hpp"
#include "nodes/chipViewer.hpp"
#include "ui/textInputBox.hpp"
#include "ui/contextMenu.hpp"
#include "ui/toolbar.hpp"
#include <SFML/Graphics.hpp>

class App;

class Editor {
public:
	Editor(sf::RenderWindow& window, App& app, std::string saveFile = "");
	void update(sf::Time& deltaTime, sf::RenderWindow& window);
	void onEvent(sf::Event& event, sf::RenderWindow& window);
	void makeChip(sf::RenderWindow& window);
	void save(sf::RenderWindow& window, std::string path = "");
	void draw(sf::RenderWindow& window);
	ChipViewer chipViewer;
	std::vector<std::unique_ptr<Node>> nodes;
	std::vector<Wire> wires;
	std::string currentSave = "";
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


	// UI
	std::unique_ptr<TextInputBox> textInputBox;
	ContextMenu contextMenu;
	sf::RectangleShape darkenOverlay;
	Toolbar toolbar;
};