#include "chip.hpp"
#include "wire.hpp"
#include "../assetManager.hpp"
#include "../ui/textInputBox.hpp"

Chip::Chip(std::vector<std::unique_ptr<Node>>& nodes, std::vector<Wire>& wires, std::string name) : text(AssetManager::getInstance().getFont("Roboto")) {
	this->name = name;
	dragging = true;

	internalNodes.reserve(nodes.size());
	for (auto& node : nodes) {
		internalNodes.push_back(std::move(node));
	}
	nodes.clear();
	internalWires.reserve(wires.size());
	for (auto& wire : wires) {
		internalWires.push_back(std::move(wire));
	}
	wires.clear();

	initChip();

	hasBeenInit = true;
}

Chip::Chip() : text(AssetManager::getInstance().getFont("Roboto")) {}

void Chip::initChip() {
	nodeType = NodeType::Chip;
	text.setCharacterSize(charSize);
	text.setString(name);
	body.setSize(text.getGlobalBounds().size + sf::Vector2f(textPadding, textPadding));
	body.setFillColor(sf::Color(255, 92, 74));
	body.setOutlineThickness(12.f);
	body.setOutlineColor(sf::Color(body.getFillColor().r * 0.7, body.getFillColor().g * 0.7, body.getFillColor().b * 0.7));
	boundingBox.setSize(body.getSize() + sf::Vector2f(boundingPadding + body.getOutlineThickness(),
		boundingPadding + body.getOutlineThickness()) * 2.f);
	boundingBox.setFillColor(sf::Color::Transparent);

	int in = 0;
	int out = 0;
	for (auto& node : internalNodes) {
		if (node->nodeType == NodeType::Input) {
			in++;
		}
		else if (node->nodeType == NodeType::Output) {
			out++;
		}
	}

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

	size_t inIndex = 0, outIndex = 0;
	for (auto& node : internalNodes) {
		if (node->nodeType == NodeType::Input) {
			Pin* internalPin = &node->outPins[0];
			Pin* chipPin = &inPins[inputMaps.size()];

			inputMaps.push_back({ chipPin, internalPin });
		}
		if (node->nodeType == NodeType::Output) {
			Pin* internalPin = &node->inPins[0];
			Pin* chipPin = &outPins[outputMaps.size()];

			outputMaps.push_back({ chipPin, internalPin });
		}
	}
}

sf::Vector2f Chip::getPosition() const {
	return boundingBox.getPosition();
}

void Chip::setPosition(sf::Vector2f position) {
	if (!hasBeenInit) return;
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

void Chip::update(sf::Time& deltaTime) {
	if (!hasBeenInit) return;
	// transfer chip data -> internal pins
	for (auto& map : inputMaps) {
		map.nodePin->isHigh = map.chipPin->isHigh;
	}
	// update nodes
	for (auto& node : internalNodes) {
		node->update(deltaTime);
	}
	// propogate wires
	for (auto& wire : internalWires) {
		wire.propogate();
	}

	// transfer internal data -> chip
	for (auto& map : outputMaps) {
		map.chipPin->isHigh = map.nodePin->isHigh;
	}
}


void Chip::onEvent(sf::Event& event, sf::RenderWindow& window) {
	if (!hasBeenInit) return;
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

void Chip::draw(sf::RenderWindow& window) {
	if (!hasBeenInit) return;
	for (auto& pin : inPins)
		pin.draw(window);
	for (auto& pin : outPins)
		pin.draw(window);
	window.draw(body);
	window.draw(text);
	window.draw(boundingBox);
}