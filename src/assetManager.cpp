#include "assetManager.hpp"

#include <exception>

AssetManager& AssetManager::getInstance() {
	static AssetManager instance;
	return instance;
}

void AssetManager::loadFont(const std::string& name, const std::string& path) {
	sf::Font font;
	if (!font.openFromFile(ASSETS_DIR + path)) {
		throw std::runtime_error("Failed to load font at path: " + std::string(ASSETS_DIR) + path);
	}
	fonts[name] = std::move(font);
}

sf::Font& AssetManager::getFont(const std::string& name) {
	auto it = fonts.find(name);
	if (it == fonts.end()) {
		throw std::runtime_error("[CRITICAL ERROR] AssetManager: Font: " + name + " not found. Make sure the font is loaded.");
	}
	return it->second;
}