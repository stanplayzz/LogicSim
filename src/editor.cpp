#include "editor.hpp"
#include "app.hpp"
#include "saveLoad.hpp"
#include <nodes/chip.hpp>

#include <print>

Editor::Editor(sf::RenderWindow& window, App& app, std::string saveFile) : toolbar(window, app), currentSave(saveFile) {
	
	view.setCenter(sf::Vector2f(window.getSize()) / 2.f);
	currentZoom = 3.f;
	view.setSize(window.getDefaultView().getSize() * currentZoom);
	textInputBox = std::make_unique<TextInputBox>();
	textInputBox->setSize({ 700.f, 200.f });
	auto half = view.getSize() / 2.f;
	auto center = view.getCenter();

	center.x = std::max(half.x, std::min(center.x, worldSize.x * tileSize - half.x));
	center.y = std::max(half.y, std::min(center.y, worldSize.y * tileSize - half.y));

	view.setCenter(center);
	window.setView(view);

	// add a save to override the current save file, only if a save file is opened
	if (saveFile != "") {
		ToolbarUI::Item save{ {200.f, 30.f}, false };
		save.text.setString("Save");
		save.onClick = [this, &window]() {
			saveCircuit(currentSave, "", nodes, wires); };
		toolbar.menu.items.insert(toolbar.menu.items.begin(), save);
		toolbar.menu.background.setSize({ 200.f, toolbar.menu.items.size() * 30.f + 30.f });
		toolbar.menu.background.setPosition(sf::Vector2f(0.f, window.getSize().y - toolbar.menu.background.getSize().y));

		for (int i = 0; i < toolbar.menu.items.size(); i++) {
			toolbar.menu.items[i].setPosition(toolbar.menu.background.getPosition() + sf::Vector2f(0.f, i * 30.f));
		}
	}
}

void Editor::update(sf::Time& deltaTime, sf::RenderWindow& window) {
	if (isAddingWire) {
		wires.back().setLastPoint(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	}

	for (auto& node : nodes) {
		if (node->dragging) {
			node->setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)) - nodeDragOffset);
		}
		node->update(deltaTime);
	}
	for (auto& wire : wires) {
		wire.propogate();
	}

	if (textInputBox->shouldDraw) {
		textInputBox->update(deltaTime);
	}
}

void Editor::onEvent(sf::Event& event, sf::RenderWindow& window) {
	if (auto mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (mouse->button == sf::Mouse::Button::Left) {
		}
		if (mouse->button == sf::Mouse::Button::Middle) {
			panning = true;
			prevMouse = sf::Mouse::getPosition(window);
		}
	}
	if (auto mouse = event.getIf<sf::Event::MouseButtonReleased>()) {
		if (mouse->button == sf::Mouse::Button::Left) {
			if (contextMenu.shouldDraw) {
				auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window),
					window.getDefaultView());
				auto bounds = contextMenu.deleteButton.getGlobalBounds();
				if (contextMenu.deleteButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView()))) {
					if (contextMenu.node) {
						for (auto& wire : wires) {
							wires.erase(
								std::remove_if(wires.begin(), wires.end(), [&](const Wire& wire) {
									for (auto& wp : wire.points) {
										if (!wp.pin) continue;
										for (auto& pin : contextMenu.node->inPins)
											if (wp.pin == &pin) return true;
										for (auto& pin : contextMenu.node->outPins)
											if (wp.pin == &pin) return true;
									}
									return false;
									}),
								wires.end()
							);
						}
						nodes.erase(
							std::remove_if(nodes.begin(), nodes.end(),
								[&](const std::unique_ptr<Node>& ptr) {
									return ptr.get() == contextMenu.node;
								}),
							nodes.end());
						contextMenu.shouldDraw = false;
						contextMenu.node = nullptr;
					}
					else if (contextMenu.wire) {
						wires.erase(
							std::remove_if(wires.begin(), wires.end(),
								[&](const Wire& wire) {
									return &wire == contextMenu.wire;
								}),
							wires.end());
						contextMenu.shouldDraw = false;
						contextMenu.wire = nullptr;
					}
				}
				if (contextMenu.inspectButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView()))) {
					if (contextMenu.node ) {
						if (contextMenu.node->nodeType == NodeType::Chip) {
							chipViewer.viewChip(dynamic_cast<Chip&>(*contextMenu.node));
							contextMenu.shouldDraw = false;
						}
					}
				}
			}
		}
		if (mouse->button == sf::Mouse::Button::Middle) 
			panning = false;
	}

	if (auto scroll = event.getIf<sf::Event::MouseWheelScrolled>()) {
		contextMenu.shouldDraw = false;
		float zoomStep = (scroll->delta > 0) ? 0.9f : 1.1f;
		float newZoom = currentZoom * zoomStep;

		sf::Vector2f newSize = window.getDefaultView().getSize() * newZoom;
		if (newSize.x > worldSize.x * tileSize) newZoom = worldSize.x * tileSize / window.getDefaultView().getSize().x;
		if (newSize.y > worldSize.y * tileSize) newZoom = worldSize.y * tileSize / window.getDefaultView().getSize().y;

		if (newZoom < 1.f) newZoom = 1.f;
		currentZoom = newZoom;
		auto before = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
		view.setSize(window.getDefaultView().getSize() * currentZoom);
		view.move(before - window.mapPixelToCoords(sf::Mouse::getPosition(window), view));
	}
	if (panning) {
		auto mouse = sf::Mouse::getPosition(window);
		view.move(window.mapPixelToCoords(prevMouse) - window.mapPixelToCoords(mouse));
		prevMouse = mouse;
	}
	auto half = view.getSize() / 2.f;
	auto center = view.getCenter();

	center.x = std::max(half.x, std::min(center.x, worldSize.x * tileSize - half.x));
	center.y = std::max(half.y, std::min(center.y, worldSize.y * tileSize - half.y));


	view.setCenter(center);
	window.setView(view);

	for (auto& node : nodes) {
		node->onEvent(event, window);
	}
	if (auto mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (!contextMenu.deleteButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView())) &&
			!contextMenu.inspectButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView()))) {
			contextMenu.shouldDraw = false;
		}
		if (mouse->button == sf::Mouse::Button::Right) {
			if (!currentNode && !currentPin && !isAddingWire) {
				for (auto& node : nodes) {
					if (node->dragging) {
						break;
					}
					if (node->boundingBox.getGlobalBounds().contains(window.mapPixelToCoords(mouse->position))) {
						contextMenu.node = node.get();
						contextMenu.wire = nullptr;
						contextMenu.shouldDraw = true;
						contextMenu.setPosition(window.mapPixelToCoords(mouse->position, window.getDefaultView()));
						contextMenu.titleText.setString(node->name);
						contextMenu.titleText.setOrigin(contextMenu.titleText.getLocalBounds().getCenter());
						contextMenu.titleText.setPosition(contextMenu.titleBox.getPosition() + contextMenu.titleBox.getSize() / 2.f);
						contextMenu.inspectButton.setFillColor(node->nodeType == NodeType::Chip ? sf::Color::White : sf::Color(200, 200, 200));
						break;
					}
				}
				
				for (auto& wire : wires) {
					if (wire.contains(window.mapPixelToCoords(mouse->position))) {
						contextMenu.wire = &wire;
						contextMenu.node = nullptr;
						contextMenu.shouldDraw = true;
						contextMenu.setPosition(window.mapPixelToCoords(mouse->position, window.getDefaultView()));
						contextMenu.titleText.setString("Wire");
						contextMenu.titleText.setOrigin(contextMenu.titleText.getLocalBounds().getCenter());
						contextMenu.titleText.setPosition(contextMenu.titleBox.getPosition() + contextMenu.titleBox.getSize() / 2.f);
						contextMenu.inspectButton.setFillColor(sf::Color(200, 200, 200));
						break;
					}
				}
			}	
			else {
				isAddingWire = false;
				currentNode = nullptr;
				currentPin = nullptr;
			}
		}
		if (mouse->button == sf::Mouse::Button::Left) {
			bool addedWire = false;
			if (currentNode && currentPin) {
				Pin* releasedOn = nullptr;
				for (size_t i = 0; i < nodes.size(); i++) {
					if (nodes[i].get() != currentNode) {
						for (auto* list : { &nodes[i]->inPins, &nodes[i]->outPins }) {
							for (auto& pin : *list) {
								if (pin.contains(window.mapPixelToCoords(mouse->position))) {
									releasedOn = &pin;
								}
							}
						}
					}
				}
				Wire wire;
				wire.addPin(currentPin);
				isAddingWire = true;
				if (releasedOn) {
					wire.addPin(releasedOn);
					isAddingWire = false;
				}
				else {
					wire.addPoint(window.mapPixelToCoords(mouse->position));
					wire.addPoint(window.mapPixelToCoords(mouse->position));
				}
				wires.push_back(wire);
				currentNode = nullptr;
				currentPin = nullptr;
				addedWire = true;
			}
			if (isAddingWire) {
				Pin* releasedOn = nullptr;
				for (size_t i = 0; i < nodes.size(); i++) {
					if (nodes[i].get() != currentNode) {
						for (auto* list : { &nodes[i]->inPins, &nodes[i]->outPins }) {
							for (auto& pin : *list) {
								if (pin.contains(window.mapPixelToCoords(mouse->position))) {
									releasedOn = &pin;
								}
							}
						}
					}
				}
				if (releasedOn) {
					wires.back().setLastPoint(releasedOn);
					isAddingWire = false;
					addedWire = true;
				}
				else {
					wires.back().setLastPoint(window.mapPixelToCoords(mouse->position));
					wires.back().addPoint(window.mapPixelToCoords(mouse->position));
				}
			}

			bool isDragging = false;
			for (auto& node : nodes) {
				if (node->dragging) {
					isDragging = true;
					break;
				}
			}
			if (!isDragging) {
				for (size_t i = 0; i < nodes.size(); i++) {
					if (nodes[i]->dragPartClicked) {
						std::unique_ptr<Node> draggedNode = std::move(nodes[i]);
						nodes.erase(nodes.begin() + i);
						nodes.insert(nodes.end(), std::move(draggedNode));

						nodeDragOffset = window.mapPixelToCoords(mouse->position) - nodes.back()->getPosition();
						nodes.back()->dragging = true;
					}
					if (!addedWire) {
						for (auto* list : { &nodes[i]->inPins, &nodes[i]->outPins }) {
							for (auto& pin : *list) {
								if (pin.contains(window.mapPixelToCoords(mouse->position))) {
									currentNode = nodes[i].get();
									currentPin = &pin;
								}
							}
						}
					}
				}
			}
		}
	}
	if (auto mouse = event.getIf<sf::Event::MouseButtonReleased>()) {
		if (mouse->button == sf::Mouse::Button::Left) {
			for (auto& node : nodes) {
				if (node->dragging) {
					bool overlap = false;
					for (auto& n : nodes) {
						if (n != node) {
							if (node->boundingBox.getGlobalBounds().findIntersection(n->boundingBox.getGlobalBounds()).has_value()) {
								overlap = true;
								break;
							}
						}
					}
					if (!overlap) {
						node->dragging = false;
					}
					break;
				}
			}
		}
	}
	if (auto key = event.getIf<sf::Event::KeyPressed>()) {
		bool isDragging = false;
		for (auto& node : nodes) {
			if (node->dragging) {
				isDragging = true;
				break;
			}
		}
		if (!isDragging) {
			if (key->scancode == sf::Keyboard::Scancode::Num1) {
				nodes.push_back(std::make_unique<InputNode>());
				nodes.back()->dragging = true; 
				nodeDragOffset = {32.f, 32.f};
			}
			if (key->scancode == sf::Keyboard::Scancode::Num2) {
				nodes.push_back(std::make_unique<OutputNode>());
				nodes.back()->dragging = true;
				nodeDragOffset = { 32.f, 32.f };
			}
			if (key->scancode == sf::Keyboard::Scancode::Num3) {
				nodes.push_back(std::make_unique<NAND_Gate>());
				nodes.back()->dragging = true;
				nodeDragOffset = { 32.f, 32.f };
			}
		}
		if (key->scancode == sf::Keyboard::Scancode::Escape) {
			if (chipViewer.isViewingChip) {
				chipViewer.clear();
			}
		}
	}

	if (textInputBox->shouldDraw) {
		textInputBox->onEvent(event, window);
	}
	if (contextMenu.shouldDraw) {
		contextMenu.onEvent(event, window);
	}
	toolbar.onEvent(event, window);
}

void Editor::save(sf::RenderWindow& window, std::string path) {
	if (path == "") {
		textInputBox = std::make_unique<TextInputBox>();
		textInputBox->setSize({ 700.f, 200.f });
		textInputBox->shouldDraw = true;
		textInputBox->setPosition(sf::Vector2f(window.getSize()) / 2.f - textInputBox->getSize() / 2.f);
		textInputBox->onConfirm = [this, &window]() {
			if (currentSave.empty()) {
				auto saveCount = 0;
				for (auto& entry : std::filesystem::directory_iterator(ASSETS_DIR + std::string("/saves"))) {
					saveCount++;
				}
				currentSave = std::format(ASSETS_DIR, "/saves", "/SaveFile", saveCount, ".ls");

				ToolbarUI::Item save{ {200.f, 30.f}, false };
				save.text.setString("Save");
				save.onClick = [this, &window]() {
					saveCircuit(currentSave, "", nodes, wires); };
				toolbar.menu.items.insert(toolbar.menu.items.begin(), save);
				toolbar.menu.background.setSize({ 200.f, toolbar.menu.items.size() * 30.f + 30.f });
				toolbar.menu.background.setPosition(sf::Vector2f(0.f, window.getSize().y - toolbar.menu.background.getSize().y));

				for (int i = 0; i < toolbar.menu.items.size(); i++) {
					toolbar.menu.items[i].setPosition(toolbar.menu.background.getPosition() + sf::Vector2f(0.f, i * 30.f));
				}
			}
			saveCircuit(ASSETS_DIR + std::string("/saves"), textInputBox->content, nodes, wires);
		};

	}
}

void Editor::draw(sf::RenderWindow& window) {
	window.setView(view);
	if (chipViewer.isViewingChip) {
		chipViewer.draw(window);
		return;
	}
	
	window.clear(sf::Color(66, 66, 69));

	for (auto& wire : wires) {
		window.draw(wire);
	}

	// preview wire
	if (currentNode && currentPin) {
		Wire wire;
		wire.addPoint(currentPin->getPosition());
		wire.addPoint(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
		window.draw(wire);
	}

	for (auto& node : nodes) {
		node->draw(window);
	}

	// UI
	window.setView(window.getDefaultView());
	window.draw(toolbar);

	if (textInputBox->shouldDraw) {
		darkenOverlay.setSize(sf::Vector2f(window.getSize()));
		darkenOverlay.setFillColor(sf::Color(0, 0, 0, 150));
		window.draw(darkenOverlay);
		window.draw(*textInputBox);
	}
	if (contextMenu.shouldDraw) {
		window.draw(contextMenu);
	}
	window.setView(view);
	window.display();
}

void Editor::makeChip(sf::RenderWindow& window) {
	if (nodes.empty() || wires.empty()) return;
	textInputBox = std::make_unique<TextInputBox>();
	textInputBox->setSize({ 700.f, 200.f });
	textInputBox->shouldDraw = true;
	textInputBox->setPosition(sf::Vector2f(window.getSize()) / 2.f - textInputBox->getSize() / 2.f);
	textInputBox->onConfirm = [this]() {
		auto chip = std::make_unique<Chip>(nodes, wires, textInputBox->content);
		nodes.push_back(std::move(chip));
		nodeDragOffset = { 32.f, 32.f };
		textInputBox->confirmed = false;
		textInputBox->content.clear();
		};
}