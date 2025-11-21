#pragma once
#include "nodes/chip.hpp"
#include "nodes/wire.hpp"



void saveCircuit(const std::string& path, const std::vector<std::unique_ptr<Node>>& nodes, const std::vector<Wire>& wires);


void loadCircuit(const std::string& path, std::vector<std::unique_ptr<Node>>& nodes, std::vector<Wire>& wires);