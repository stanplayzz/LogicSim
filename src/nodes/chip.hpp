#pragma once
#include "node.hpp"

class Wire;

class Chip : public Node {
public:
	Chip(std::vector<std::unique_ptr<Node>>& nodes, std::vector<Wire>& wires, std::string name);
	Chip();
	void initChip();
	sf::Vector2f getPosition() const override;
	void setPosition(sf::Vector2f position) override;
	void update(sf::Time& deltaTime) override;
	void onEvent(sf::Event& event, sf::RenderWindow& window) override;
	void draw(sf::RenderWindow& window) override;

	std::vector<std::unique_ptr<Node>> internalNodes;
	std::vector<Wire> internalWires;
	std::vector<PinMap> inputMaps;
	std::vector<PinMap> outputMaps;
	bool hasBeenInit = false;
private:
	sf::RectangleShape body;
	float boundingPadding = 32.f;
	float textPadding = 128.f;
	int charSize = 128.f;
	sf::Text text;
	sf::Time dt;
};