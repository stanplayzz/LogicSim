#pragma once
#include "nodes/chip.hpp"
#include "nodes/wire.hpp"



void saveCircuit(const std::string& folder, const std::string& name, const std::vector<std::unique_ptr<Node>>& nodes, const std::vector<Wire>& wires);


std::string readString(std::ifstream& in);
void loadCircuit(const std::string& path, std::vector<std::unique_ptr<Node>>& nodes, std::vector<Wire>& wires);