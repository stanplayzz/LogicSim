#include "wire.hpp"


WireSegment::WireSegment(float thickness) : thickness(thickness) {
	capA.setOrigin({ thickness / 2.f, thickness / 2.f });
	capB.setOrigin({ thickness / 2.f, thickness / 2.f });
}

void WireSegment::setFillColor(sf::Color color) {
	wire.setFillColor(color);
	capA.setFillColor(color);
	capB.setFillColor(color);
}


void WireSegment::setPoints(sf::Vector2f A, sf::Vector2f B) {
	sf::Vector2f diff = B - A;
	float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
	float angle = std::atan2(diff.y, diff.x) * 180.f / 3.14159f;

	wire.setSize({ length, thickness });
	wire.setOrigin({ 0, thickness / 2 });
	wire.setPosition(A);
	wire.setRotation(sf::degrees(angle));

	capA.setRadius(thickness / 2);
	capA.setPosition(A);
	capB.setRadius(thickness / 2);
	capB.setPosition(B);

}


void WireSegment::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(capA, states);
	target.draw(capB, states);
	target.draw(wire, states);
}

void Wire::addPin(Pin* pin) {
	WirePoint wp;
	wp.pin = pin;
	points.push_back(wp);
}

void Wire::addPoint(sf::Vector2f point) {
	WirePoint wp;
	wp.pos = point;
	points.push_back(wp);
}


void Wire::setLastPoint(sf::Vector2f point) {
	if (points.empty()) return;
	points.back().pin = nullptr;
	points.back().pos = point;
}

void Wire::setLastPoint(Pin* pin) {
	if (points.empty()) return;
	points.back().pin = pin;
}

void Wire::propogate() {
	bool newState = false;

	for (auto& wp: points) {
		if (wp.pin && wp.pin->pinType == PinType::Output && wp.pin->isHigh) {
			newState = true;
		}
	}
	for (auto& wp : points) {
		if (wp.pin && wp.pin->pinType == PinType::Input) {
			if (wp.pin->isHigh != newState) {
				wp.pin->isHigh = newState;
			}
		}
	}
}

bool Wire::contains(sf::Vector2f point) {
	std::vector<sf::Vector2f> resolved;

	for (auto& wp : points) {
		if (wp.pin) {
			resolved.push_back(wp.pin->getPosition());
		}
		else {
			resolved.push_back(wp.pos);
		}
	}
	for (size_t i = 0; i < points.size() - 1; i++) {
		WireSegment segment(thickness);
		segment.setPoints(resolved[i], resolved[i + 1]);
		if (segment.capA.getGlobalBounds().contains(point) || segment.capB.getGlobalBounds().contains(point) || segment.wire.getGlobalBounds().contains(point)) {
			return true;
		}
	}
	return false;
}

void Wire::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (points.size() < 2) return;

	std::vector<sf::Vector2f> resolved;

	int pinCount = 0;
	bool isHigh = false;

	for (auto& wp : points) {
		if (wp.pin) {
			pinCount++;
			if (wp.pin->pinType == PinType::Output) {
				if (wp.pin->isHigh) {
					isHigh = true;
				}
			}
			resolved.push_back(wp.pin->getPosition());
		}
		else {
			resolved.push_back(wp.pos);
		}
	}

	sf::Color color;
	color = sf::Color::Black;
	if (pinCount > 1) {
		color = sf::Color(70, 35, 35);
		if (isHigh) {
			color = sf::Color(242, 76, 79);
		}
	}

	for (size_t i = 0; i < points.size() - 1; i++) {
		WireSegment segment(thickness);
		segment.setFillColor(color);
		segment.setPoints(resolved[i], resolved[i + 1]);
		target.draw(segment, states);
	}
}