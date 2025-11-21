#pragma once
#include "node.hpp"
#include <SFML/Graphics.hpp>

class WireSegment : public sf::Drawable {
public:
	WireSegment(float thickness = 16.f);
	void setFillColor(sf::Color color);
	void setPoints(sf::Vector2f A, sf::Vector2f B);
	sf::RectangleShape wire;
	sf::CircleShape capA, capB;
private:
	float thickness;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

struct WirePoint {
	sf::Vector2f pos;
	Pin* pin = nullptr;
};

class Wire : public sf::Drawable {
public:
	float thickness = 16.f;
	std::vector<WirePoint> points;

	void addPin(Pin* pin);
	void addPoint(sf::Vector2f point);
	void setLastPoint(sf::Vector2f point);
	void setLastPoint(Pin* pin);
	void propogate();
	bool contains(sf::Vector2f point);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};