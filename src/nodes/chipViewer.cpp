#include "chipViewer.hpp"

#include <print>

ChipViewer::ChipViewer() {


}

void ChipViewer::viewChip(Chip& chip) {
	currentChip = &chip;
	sf::Vector2f TL = chip.internalNodes[0]->getPosition();
	sf::Vector2f BR = TL;

	for (auto& node : chip.internalNodes) {
		sf::Vector2f pos = node->getPosition();
		TL.x = std::min(TL.x, pos.x);
		TL.y = std::min(TL.y, pos.y);
		BR.x = std::max(BR.x, pos.x + node->boundingBox.getSize().x);
		BR.y = std::max(BR.y, pos.y + node->boundingBox.getSize().y);
	}

	TL -= sf::Vector2f(64.f, 64.f); // padding
	BR += sf::Vector2f(64.f, 64.f);

	chipView.setCenter((TL + BR) / 2.f);
	chipView.setSize(BR - TL);

	isViewingChip = true;
}

void ChipViewer::clear() {
	currentChip = nullptr;
	isViewingChip = false;
}


void ChipViewer::draw(sf::RenderWindow& window) {
	if (!currentChip) return;
	window.clear(sf::Color(66, 66, 69));
	auto view = window.getView();
	window.setView(chipView);

	for (auto& wire : currentChip->internalWires) {
		window.draw(wire);
	}
	for (auto& node : currentChip->internalNodes) {
		node->draw(window);
	}

	window.setView(view);
	window.display();
}