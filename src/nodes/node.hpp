#pragma once
#include <SFML/Graphics.hpp>

#include <vector>

enum class PinType { Input, Output };
enum class NodeType{ None, Input, Output, Gate, Chip };


class Wire;
class Node;

class Pin {
public:
	Pin(PinType type) {
		circle.setOrigin(circle.getGeometricCenter());
		circle.setFillColor(sf::Color::Black);
		pinType = type;
	}
	sf::Vector2f getPosition() const;
	float getRadius() const;
	bool contains(sf::Vector2f point);
	void setPosition(sf::Vector2f position);
	void onEvent(sf::Event& event, sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);

	bool isHigh = false;
	PinType pinType;
	int id = 0; // used for saving/loading
private:
	sf::CircleShape circle{ 32.f };
};

struct PinMap {
	Pin* chipPin;
	Pin* nodePin;
};

class Node {
public:
	NodeType nodeType{NodeType::None};
	std::vector<Pin> inPins;
	std::vector<Pin> outPins;
	bool dragging = false;
	bool dragPartClicked = false;
	sf::RectangleShape boundingBox;
	std::string name;
	int id = 0; // used for saving/loading

	virtual sf::Vector2f getPosition() const = 0;
	virtual void setPosition(sf::Vector2f position) = 0;
	virtual void onEvent(sf::Event& event, sf::RenderWindow& window) = 0;
	virtual void update(sf::Time& deltaTime) {}
	virtual void draw(sf::RenderWindow& window) = 0;
};

class InputNode : public Node {
public:
	InputNode();
	sf::Vector2f getPosition() const override;
	void setPosition(sf::Vector2f position) override;
	void update(sf::Time& deltaTime) override;
	void onEvent(sf::Event& event, sf::RenderWindow& window) override;
	void draw(sf::RenderWindow& window) override;
private:
	sf::RectangleShape dragPart{ {64.f, 196.f} };
	sf::RectangleShape line{ {96.f, 32.f} };
	sf::CircleShape circle{ 90.f };
	sf::Vector2f dragOffset{};
	float boundingPadding = 16.f;
};

class OutputNode : public Node {
public:
	OutputNode();
	sf::Vector2f getPosition() const override;
	void setPosition(sf::Vector2f position) override;
	void update(sf::Time& deltaTime) override;
	void onEvent(sf::Event& event, sf::RenderWindow& window) override;
	void draw(sf::RenderWindow& window) override;
private:
	sf::RectangleShape dragPart{ {64.f, 196.f} };
	sf::RectangleShape line{ {96.f, 32.f} };
	sf::CircleShape circle{ 90.f };
	sf::Vector2f dragOffset{};
	bool dragging = false;
	float boundingPadding = 16.f;
};

class Gate : public Node {
public:
	Gate(int in, int out, std::string name);
	sf::Vector2f getPosition() const override;
	void setPosition(sf::Vector2f position) override;
	void onEvent(sf::Event& event, sf::RenderWindow& window) override;
	void draw(sf::RenderWindow& window) override;
private:
	sf::RectangleShape body;
	float boundingPadding = 32.f;
	float textPadding = 128.f;
	int charSize = 128.f;
protected:
	sf::Text text;
};

class NAND_Gate : public Gate {
public:
	NAND_Gate() : Gate(2, 1, "NAND") {}
	void update(sf::Time& deltaTime) override;
};

