#pragma once
#include "chip.hpp"
#include "wire.hpp"


class ChipViewer {
public:
	ChipViewer();

	void viewChip(Chip& chip);
	void clear();
	void draw(sf::RenderWindow& window);

	bool isViewingChip = false;

private:
	Chip* currentChip = nullptr;
	sf::View chipView;
};