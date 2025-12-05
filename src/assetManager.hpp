#pragma once
#include <SFML/Graphics/Font.hpp>

class AssetManager {
public:
	static AssetManager& getInstance();
	void loadFont(const std::string& name, const std::string& path);
	sf::Font& getFont(const std::string& name);
private:
	std::unordered_map<std::string, sf::Font> fonts;
};