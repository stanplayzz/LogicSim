#include "saveLoad.hpp"
#include <fstream>
#include <print>
#include <unordered_map>

//--- SAVING ---//
void writeString(std::ofstream& out, const std::string& s) {
	uint32_t len = s.size();
	out.write((char*)&len, sizeof(len));
	out.write(s.data(), len);
}

void savePin(std::ofstream& out, const Pin& p) {
	out.write((char*)&p.id, sizeof(p.id));
	out.write((char*)&p.isHigh, sizeof(p.isHigh));

	auto type = (int)p.pinType;
	out.write((char*)&type, sizeof(type));
}

void saveWire(std::ofstream& out, const Wire& w) {
	int pointCount = w.points.size();
	out.write((char*)&pointCount, sizeof(pointCount));

	for (auto& p : w.points) {
		out.write((char*)&p.pos, sizeof(p.pos));

		int pinId = p.pin ? p.pin->id : -1;
		
		out.write((char*)&pinId, sizeof(pinId));
	}
}

void saveNode(std::ofstream& out, const Node& n) {
	out.write((char*)&n.id, sizeof(n.id));

	auto type = (int)n.nodeType;
	out.write((char*)&type, sizeof(type));

	writeString(out, n.name);

	auto pos = n.getPosition();
	out.write((char*)&pos, sizeof(pos));

	//// input pins
	//int inCount = n.inPins.size();
	//out.write((char*)&inCount, sizeof(inCount));
	//for (auto& p : n.inPins) 
	//	savePin(out, p);

	//// output pins
	//int outCount = n.outPins.size();
	//out.write((char*)&outCount, sizeof(outCount));
	//for (auto& p : n.outPins)
	//	savePin(out, p);

	// chip nodes
	if (n.nodeType == NodeType::Chip) {
		auto& chip = static_cast<const Chip&>(n);

		// internal nodes
		int internalCount = chip.internalNodes.size();
		out.write((char*)&internalCount, sizeof(internalCount));
		for (auto& node : chip.internalNodes) 
			saveNode(out, *node);

		// internal wires
		int wireCount = chip.internalWires.size();
		out.write((char*)&wireCount, sizeof(wireCount));
		for (auto& w : chip.internalWires)
			saveWire(out, w);

		/*int inCount = chip.inPins.size();
		out.write((char*)&inCount, sizeof(inCount));
		for (auto& p : chip.inPins)
			out.write((char*)&p.id, sizeof(p.id));

		int outCount = chip.outPins.size();
		out.write((char*)&outCount, sizeof(outCount));
		for (auto& p : chip.outPins)
			out.write((char*)&p.id, sizeof(p.id));*/

		// input maps
		//int mapCount = chip.inputMaps.size();
		//out.write((char*)&mapCount, sizeof(mapCount));
		//for (auto& m : chip.inputMaps) {
		//	out.write((char*)&m.chipPin->id, sizeof(int));
		//	out.write((char*)&m.nodePin->id, sizeof(int));
		//}

		//// output maps
		//mapCount = chip.outputMaps.size();
		//out.write((char*)&mapCount, sizeof(mapCount));
		//for (auto& m : chip.outputMaps) {
		//	out.write((char*)&m.chipPin->id, sizeof(int));
		//	out.write((char*)&m.nodePin->id, sizeof(int));
		//}
	}
}

void saveCircuit(const std::string& path, const std::vector<std::unique_ptr<Node>>& nodes, const std::vector<Wire>& wires) {
	std::ofstream out(path, std::ios::binary);
	if (!out.is_open()) {
		std::println("Failed to open save file at path: {}", path);
		return;
	}

	// assign IDs
	int nextId = 1;
	for (auto& n : nodes) {
		n->id = nextId++;
		for (auto& p : n->inPins) p.id = nextId++;
		for (auto& p : n->outPins) p.id = nextId++;
	}

	// write nodes
	int nodeCount = nodes.size();
	out.write((char*)&nodeCount, sizeof(nodeCount));
	for (auto& n : nodes)
		saveNode(out, *n);

	// write wires
	int wireCount = wires.size();
	out.write((char*)&wireCount, sizeof(wireCount));
	for (auto& w : wires)
		saveWire(out, w);

	std::println("Successfully saved the circuit to path: {}", path);
}


//--- LOADING ---//

std::string readString(std::ifstream& in) {
	uint32_t len;
	in.read((char*)&len, sizeof(len));
	std::string s(len, '\0');
	in.read(s.data(), len);
	return s;
}

// map to keep track of pins for rewiring later
std::unordered_map<int, Pin*> pinMap;

Pin readPin(std::ifstream& in) {
	int id;
	bool isHigh;
	int typeInt;

	in.read((char*)&id, sizeof(id));
	in.read((char*)&isHigh, sizeof(isHigh));
	in.read((char*)&typeInt, sizeof(typeInt));

	PinType type = (PinType)typeInt;
	Pin p(type);
	p.id = id;
	p.isHigh = isHigh;

	return p;
}

Wire readWire(std::ifstream& in) {
	Wire w;
	int pointCount;
	in.read((char*)&pointCount, sizeof(pointCount));

	w.points.reserve(pointCount);
	for (int i = 0; i < pointCount; i++) {
		WirePoint wp;
		in.read((char*)&wp.pos, sizeof(wp.pos));

		int pinId;
		in.read((char*)&pinId, sizeof(pinId));
		wp.pin = (pinId != -1) ? pinMap[pinId] : nullptr;
		w.points.push_back(wp);
	}

	return w;
}

std::unique_ptr<Node> readNode(std::ifstream& in) {
	int id;
	in.read((char*)&id, sizeof(id));

	int typeInt;
	in.read((char*)&typeInt, sizeof(typeInt));
	NodeType type = (NodeType)typeInt;

	auto name = readString(in);

	sf::Vector2f pos;
	in.read((char*)&pos, sizeof(pos));

	std::unique_ptr<Node> node;

	switch (type) {
	case NodeType::Input: node = std::make_unique<InputNode>(); break;
	case NodeType::Output: node = std::make_unique<OutputNode>(); break;
	case NodeType::Gate: node = std::make_unique<NAND_Gate>(); break;
	case NodeType::Chip: node = std::make_unique<Chip>(); break;
	default: node = std::make_unique<InputNode>(); break;
	}

	node->id = id;
	node->name = name;
	node->setPosition(pos);

	// input pins
	//int inCount;
	//in.read((char*)&inCount, sizeof(inCount));
	//node->inPins.reserve(inCount);
	//for (int i = 0; i < inCount; i++) {
	//	Pin p = readPin(in);
	//	node->inPins.push_back(p);
	//	pinMap[p.id] = &node->inPins.back();
	//}

	//// output pins
	//int outCount;
	//in.read((char*)&outCount, sizeof(outCount));
	//node->outPins.reserve(outCount);
	//for (int i = 0; i < outCount; i++) {
	//	Pin p = readPin(in);
	//	node->outPins.push_back(p);
	//	pinMap[p.id] = &node->outPins.back();
	//}

	if (type == NodeType::Chip) {
		auto& chip = static_cast<Chip&>(*node);

		// internal nodes
		int internalCount;
		in.read((char*)&internalCount, sizeof(internalCount));
		for (int i = 0; i < internalCount; i++) {
			chip.internalNodes.push_back(readNode(in)); 
		}

		// internal wires
		int wireCount;
		in.read((char*)&wireCount, sizeof(wireCount));
		for (int i = 0; i < wireCount; i++) {
			chip.internalWires.push_back(readWire(in));
		}

		/*std::vector<int> savedInIds;
		std::vector<int> savedOutIds;

		int inCount;
		in.read((char*)&inCount, sizeof(inCount));
		savedInIds.resize(inCount);
		for (int& id : savedInIds) in.read((char*)&id, sizeof(id));

		int outCount;
		in.read((char*)&outCount, sizeof(outCount));
		savedOutIds.resize(outCount);
		for (int& id : savedOutIds) in.read((char*)&id, sizeof(id));*/

		chip.initChip();

		//// 3. assign IDs onto real pins
		//for (int i = 0; i < inCount; i++) {
		//	chip.inPins[i].id = savedInIds[i];
		//	pinMap[savedInIds[i]] = &chip.inPins[i];
		//}
		//for (int i = 0; i < outCount; i++) {
		//	chip.outPins[i].id = savedOutIds[i];
		//	pinMap[savedOutIds[i]] = &chip.outPins[i];
		//}

		//// input maps
		//int mapCount;
		//in.read((char*)&mapCount, sizeof(mapCount));
		//chip.inputMaps.clear();
		//chip.inputMaps.resize(mapCount);
		//for (int i = 0; i < mapCount; i++) {
		//	int chipId, nodeId;
		//	in.read((char*)&chipId, sizeof(chipId));
		//	in.read((char*)&nodeId, sizeof(nodeId));
		//	if (pinMap[chipId]) {
		//		std::println("chip");

		//	}
		//	if (pinMap[nodeId]) {
		//		std::println("node");

		//	}
		//	chip.inputMaps[i].chipPin = pinMap[chipId];
		//	chip.inputMaps[i].nodePin = pinMap[nodeId];
		//}

		//// output maps
		//in.read((char*)&mapCount, sizeof(mapCount));
		//chip.outputMaps.clear();
		//chip.outputMaps.resize(mapCount);
		//for (int i = 0; i < mapCount; i++) {
		//	int chipId, nodeId;
		//	in.read((char*)&chipId, sizeof(chipId));
		//	in.read((char*)&nodeId, sizeof(nodeId));
		//	chip.outputMaps[i].chipPin = pinMap[chipId];
		//	chip.outputMaps[i].nodePin = pinMap[nodeId];
		//}

		chip.hasBeenInit = true;
	}
	return node;
}

void loadCircuit(const std::string& path, std::vector<std::unique_ptr<Node>>& nodes, std::vector<Wire>& wires) {
	std::ifstream in(path, std::ios::binary);
	if (!in.is_open()) {
		std::println("Failed to open save file at path: {}", path);
		return;
	}

	pinMap.clear();
	nodes.clear();
	wires.clear();

	// read nodes
	int nodeCount;
	in.read((char*)&nodeCount, sizeof(nodeCount));
	for (int i = 0; i < nodeCount; i++) 
		nodes.push_back(readNode(in));

	// read wires
	int wireCount;
	in.read((char*)&wireCount, sizeof(wireCount));
	for (int i = 0; i < wireCount; i++)
		wires.push_back(readWire(in));

	// (re)move nodes to their position to update all items like pins and wires
	for (auto& n : nodes) {
		n->setPosition(n->getPosition());
	}

	std::println("Successfully loaded circuit from {}", path);
}