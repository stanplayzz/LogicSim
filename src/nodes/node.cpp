#include "node.hpp"
#include "wire.hpp"
#include "../assetManager.hpp"
#include "../editor.hpp"

#include <print>
#include <thread>

void Pin::setPosition(sf::Vector2f position) {
	circle.setPosition(position);
}

bool Pin::contains(sf::Vector2f point) {
	return circle.getGlobalBounds().contains(point);
}

sf::Vector2f Pin::getPosition() const {
	return circle.getPosition();
}
float Pin::getRadius() const {
	return circle.getRadius();
}

void Pin::onEvent(sf::Event& event, sf::RenderWindow& window) {
	if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
		circle.setFillColor(sf::Color::Black);
		if (circle.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
			circle.setFillColor(sf::Color::White);
		}
	}
}

void Pin::draw(sf::RenderWindow& window) {
	window.draw(circle);
}

InputNode::InputNode() {
	name = "Input";
	nodeType = NodeType::Input;
	dragPart.setFillColor(sf::Color(125, 125, 125));
	line.setFillColor(sf::Color::Black);
	circle.setOutlineThickness(8.f);
	circle.setFillColor(sf::Color(70, 35, 35));
	circle.setOutlineColor(sf::Color::Black);
	circle.setOrigin(circle.getGeometricCenter());
	outPins.push_back(Pin(PinType::Output));

	setPosition({});
	boundingBox.setSize({ outPins.back().getPosition().x + outPins.back().getRadius() + boundingPadding * 2.f,
		dragPart.getSize().y + boundingPadding * 2.f});
	boundingBox.setFillColor(sf::Color::Transparent);
}

void InputNode::setPosition(sf::Vector2f position) {
	boundingBox.setPosition(position);
	dragPart.setPosition(position + sf::Vector2f(boundingPadding, boundingPadding));
	circle.setPosition(dragPart.getPosition() + sf::Vector2f(dragPart.getSize().x + circle.getRadius() + circle.getOutlineThickness() + 32.f, 
		dragPart.getSize().y / 2.f));
	line.setPosition(circle.getPosition() + sf::Vector2f(circle.getRadius(), -line.getSize().y / 2.f));
	outPins.back().setPosition({ line.getPosition().x + line.getSize().x,
		dragPart.getPosition().y + dragPart.getSize().y / 2.f});
}

sf::Vector2f InputNode::getPosition() const {
	return boundingBox.getPosition();
}

void InputNode::update(sf::Time& deltaTime) {
	if (outPins.back().isHigh) {
		circle.setFillColor(sf::Color(242, 76, 79));
	}
	else {
		circle.setFillColor(sf::Color(70, 35, 35));
	}
}

void InputNode::onEvent(sf::Event& event, sf::RenderWindow& window) {
	if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
		dragPart.setFillColor(sf::Color(125, 125, 125));
		if (dragPart.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
			dragPart.setFillColor(sf::Color(200, 200, 200));
		}
	}
	if (auto mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (mouse->button == sf::Mouse::Button::Right) {
			boundingBox.setFillColor(sf::Color::Transparent);
			if (dragPart.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				boundingBox.setFillColor(sf::Color(255, 255, 255, 50));
			}
		}
		if (mouse->button == sf::Mouse::Button::Left) {
			boundingBox.setFillColor(sf::Color::Transparent);
			dragPartClicked = false;
			if (dragPart.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				boundingBox.setFillColor(sf::Color(255, 255, 255, 50));
				dragPartClicked = true;
			}
			if (circle.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				outPins.back().isHigh = !outPins.back().isHigh;
			}
		}
	}
	outPins.back().onEvent(event, window);
}

void InputNode::draw(sf::RenderWindow& window) {
	window.draw(dragPart);
	window.draw(line);
	window.draw(circle);
	outPins.back().draw(window);
	window.draw(boundingBox);
}

OutputNode::OutputNode() {
	name = "Output";
	nodeType = NodeType::Output;
	dragPart.setFillColor(sf::Color(125, 125, 125));
	line.setFillColor(sf::Color::Black);
	circle.setOutlineThickness(8.f);
	circle.setFillColor(sf::Color(70, 35, 35));
	circle.setOutlineColor(sf::Color::Black);
	circle.setOrigin(circle.getGeometricCenter());
	inPins.push_back(Pin(PinType::Input));
	boundingBox.setSize({ 32.f + line.getSize().x + circle.getRadius() * 2.f + circle.getOutlineThickness() * 2.f + 32.f
		+ dragPart.getSize().x + boundingPadding * 2.f,
		dragPart.getSize().y + boundingPadding * 2.f });
	boundingBox.setFillColor(sf::Color::Transparent);

	setPosition({});
}

void OutputNode::setPosition(sf::Vector2f position) {
	boundingBox.setPosition(position);
	dragPart.setPosition(position + sf::Vector2f(boundingBox.getSize().x - dragPart.getSize().x - boundingPadding, boundingPadding));
	circle.setPosition(dragPart.getPosition() - sf::Vector2f(circle.getRadius() + circle.getOutlineThickness() + 32.f,
		-dragPart.getSize().y / 2.f));
	line.setPosition(circle.getPosition() - sf::Vector2f(circle.getRadius() + line.getSize().x, line.getSize().y / 2.f));
	inPins.back().setPosition({ line.getPosition().x,
		dragPart.getPosition().y + dragPart.getSize().y / 2.f });
}

sf::Vector2f OutputNode::getPosition() const {
	return boundingBox.getPosition();
}

void OutputNode::update(sf::Time& deltaTime) {
	if (inPins.back().isHigh) {
		circle.setFillColor(sf::Color(242, 76, 79));
	}
	else {
		circle.setFillColor(sf::Color(70, 35, 35));
	}
}

void OutputNode::onEvent(sf::Event& event, sf::RenderWindow& window) {
	if (auto mouse = event.getIf<sf::Event::MouseMoved>()) {
		dragPart.setFillColor(sf::Color(125, 125, 125));
		if (dragPart.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
			dragPart.setFillColor(sf::Color(200, 200, 200));
		}
	}
	if (auto mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (mouse->button == sf::Mouse::Button::Right) {
			boundingBox.setFillColor(sf::Color::Transparent);
			if (dragPart.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				boundingBox.setFillColor(sf::Color(255, 255, 255, 50));
			}
		}
		if (mouse->button == sf::Mouse::Button::Left) {
			boundingBox.setFillColor(sf::Color::Transparent);
			dragPartClicked = false;
			if (dragPart.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				boundingBox.setFillColor(sf::Color(255, 255, 255, 50)); 
				dragPartClicked = true;
			}
		}
	}

	inPins.back().onEvent(event, window);
}

void OutputNode::draw(sf::RenderWindow& window) {
	window.draw(dragPart);
	window.draw(line);
	window.draw(circle);
	inPins.back().draw(window);
	window.draw(boundingBox);
}

Gate::Gate(int in, int out, std::string name) : text(AssetManager::getInstance().getFont("Roboto")) {
	this->name = name;
	nodeType = NodeType::Gate;
	text.setCharacterSize(charSize);
	text.setString(name);
	body.setSize(text.getGlobalBounds().size + sf::Vector2f(textPadding, textPadding));
	body.setFillColor(sf::Color(255, 92, 74));
	body.setOutlineThickness(12.f);
	body.setOutlineColor(sf::Color(body.getFillColor().r * 0.7, body.getFillColor().g * 0.7, body.getFillColor().b * 0.7));
	boundingBox.setSize(body.getSize() + sf::Vector2f(boundingPadding + body.getOutlineThickness(), 
		boundingPadding + body.getOutlineThickness()) * 2.f);
	boundingBox.setFillColor(sf::Color::Transparent);

	for (size_t i = 0; i < in; i++) {
		inPins.push_back(Pin(PinType::Input));
	}
	for (size_t i = 0; i < out; i++) {
		outPins.push_back(Pin(PinType::Output));
	}

	int maxPins = std::max(in, out);
	if (body.getSize().y + body.getOutlineThickness() * 2.f < (maxPins + 1) * 32.f) {
		body.setSize({ body.getSize().x, (maxPins + 1) * 32.f });
		boundingBox.setSize(body.getSize() + sf::Vector2f(boundingPadding + body.getOutlineThickness(),
			boundingPadding + body.getOutlineThickness()) * 2.f);
	}
}

sf::Vector2f Gate::getPosition() const {
	return boundingBox.getPosition();
}

void Gate::setPosition(sf::Vector2f position) {
	boundingBox.setPosition(position);
	body.setPosition(position + sf::Vector2f(boundingPadding + body.getOutlineThickness(), boundingPadding + body.getOutlineThickness()));
	text.setOrigin(text.getLocalBounds().getCenter());
	text.setPosition(body.getPosition() + body.getSize() / 2.f);

	auto totalHeight = body.getSize().y + body.getOutlineThickness() * 2.f;
	auto spacing = totalHeight / (inPins.size() + 1);
	for (size_t i = 0; i < inPins.size(); i++) {
		inPins[i].setPosition(body.getPosition() + sf::Vector2f(-body.getOutlineThickness(),
			-body.getOutlineThickness() + spacing * (i + 1)));
	}
	spacing = totalHeight / (outPins.size() + 1);
	for (size_t i = 0; i < outPins.size(); i++) {
		outPins[i].setPosition(body.getPosition() + sf::Vector2f(body.getSize().x + body.getOutlineThickness(),
			-body.getOutlineThickness() + spacing * (i + 1)));
	}
}
void Gate::onEvent(sf::Event& event, sf::RenderWindow& window) {
	if (auto mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (mouse->button == sf::Mouse::Button::Right) {
			boundingBox.setFillColor(sf::Color::Transparent);
			if (body.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				boundingBox.setFillColor(sf::Color(255, 255, 255, 50));
			}
		}
		if (mouse->button == sf::Mouse::Button::Left) {
			boundingBox.setFillColor(sf::Color::Transparent);
			dragPartClicked = false;
			if (body.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
				boundingBox.setFillColor(sf::Color(255, 255, 255, 50));
				dragPartClicked = true;
			}
		}
	}
	for (auto& pin : inPins)
		pin.onEvent(event, window); 
	for (auto& pin : outPins)
		pin.onEvent(event, window);
}

void Gate::draw(sf::RenderWindow& window) {
	for (auto& pin : inPins)
		pin.draw(window);
	for (auto& pin : outPins)
		pin.draw(window);
	window.draw(body);
	window.draw(text);
	window.draw(boundingBox);
}

void NAND_Gate::update(sf::Time& deltaTime) {
	outPins[0].isHigh = !(inPins[0].isHigh && inPins[1].isHigh);
}
