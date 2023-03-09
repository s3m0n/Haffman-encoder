#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

class Node {
public:
	int weight;
	unsigned char let;
	bool isUsed;
	Node* pLeft;
	Node* pRight;

	friend std::ostream& operator << (std::ostream& os, const Node& p);

	Node() {
		weight = 0;
		let = 0;
		isUsed = false;
		pLeft = nullptr;
		pRight = nullptr;
	}

	Node(const Node& n) {
		this->weight = n.weight;
		this->let = n.let;
		this->isUsed = n.isUsed;
		this->pLeft = n.pLeft;
		this->pRight = n.pRight;
	}


	Node(int w, char l) {
		this->weight = w;
		this->let = l;
		this->isUsed = false;
		this->pLeft = nullptr;
		this->pRight = nullptr;
	}

	Node(Node& n, Node& m) {
		this->weight = n.weight + m.weight;
		let = NULL;
		if (n.weight < m.weight) {
			pLeft = &n;
			pRight = &m;
			isUsed = false;
		}
		else {
			
			pLeft = &m;
			pRight = &n;
			isUsed = false;
		}
	}

	
	

	bool operator< (const Node n) const {
		return this->weight < n.weight;
	}

	bool operator<= (const Node n) const {
		return this->weight <= n.weight;
	}
};

std::ostream& operator << (std::ostream& os, const Node& p)
{
	return os << p.let << " " << p.weight << " | " << p.isUsed;
}

int getFirstUnused(Node* vec, int& n) {
	for (int i = 0; i < n; i++) {
		if (vec[i].isUsed == false) return i;
	}
}



std::pair<int, int> getMinElements(Node* vec, int& n) {
	int min = getFirstUnused(vec, n);
	if (min >= n - 1) {
		std::pair<int, int> ret(-1, -1);
		return ret;
	}
	for (int i = min + 1; i < n; i++) {
		if (vec[i] <= vec[min] && !vec[i].isUsed) {
			min = i;
		}
	}
	vec[min].isUsed = true;

	int min2 = getFirstUnused(vec, n);
	for (int i = min2 + 1; i < n; i++) {
		if (vec[i] <= vec[min2] && !vec[i].isUsed && i != min) {
			min2 = i;
		}
	}
	vec[min2].isUsed = true;

	std::pair<int, int> ret(min, min2);
	return ret;
}


Node getTree(Node* vec, int& n) {
	while (true) {
		std::pair<int, int> pair = getMinElements(vec, n);
		if (pair.first == -1 || pair.second == -1) break;
		n++;
		vec[n - 1] = Node(vec[pair.first], vec[pair.second]);
	}
	return vec[n - 1];
}

void obhod(Node& Tree) {
	if (Tree.pLeft == nullptr && Tree.pRight == nullptr) {
		std::cout << Tree << std::endl;
	}
	else {
		obhod(*Tree.pLeft);
		obhod(*Tree.pRight);
	}
}

void getCodes(Node& Tree, std::map<unsigned char, std::string>& table, std::string code) {
	if (Tree.pLeft == nullptr && Tree.pRight == nullptr) {
		table[Tree.let] = code;
	}
	else {
		getCodes(*Tree.pLeft, table, code + "0");
		getCodes(*Tree.pRight, table, code + "1");
	}
}

void getDeCodes(Node& Tree, std::map<std::string, unsigned char>& table, std::string code) {
	if (Tree.pLeft == nullptr && Tree.pRight == nullptr) {
		table[code] = Tree.let;
	}
	else {
		getDeCodes(*Tree.pLeft, table, code + "0");
		getDeCodes(*Tree.pRight, table, code + "1");
	}
}

void set0(unsigned char& v, int k) {
	v = v & (~(1 << k));
}

void set1(unsigned char& v, int k) {
	v = v | (1 << k);
}

bool getk(unsigned char& v, int k) {
	return v & (1 << k);
}

void encode(std::string inputName, std::string outputName) {

	std::ifstream file(inputName, std::ios::binary);

	std::map<unsigned char, int> freqTable;

	while (!file.eof()) {
		unsigned char cell;
		file.read((char*)&cell, sizeof(cell));
		freqTable[cell]++;
	}
	file.close();

	int n = freqTable.size();

	Node* myVec = new Node[n + n - 1];

	n = 0;

	for (const auto& elem : freqTable) {
		myVec[n] = Node(elem.second, elem.first);
		n++;
	}


	Node Tree = getTree(myVec, n);

	std::map<unsigned char, std::string> table;
	getCodes(Tree, table, "");

	int k = 0;

	std::ifstream ifs(inputName, std::ios::binary);
	std::ofstream ofs(outputName, std::ios::binary);

	if (!ifs.is_open()) {
		std::cout << "Error open file" << std::endl;
		return;
	}

	unsigned char size = freqTable.size();
	ofs.write((char*)&size, sizeof(char));

	for (const auto& elem : freqTable) {
		unsigned char let = elem.first;
		unsigned int freq = elem.second;
		ofs.write((char*)&let, sizeof(char));
		ofs.write((char*)&freq, sizeof(freq));
	}

	unsigned char wcell = 0;

	while (!ifs.eof()) {
		unsigned char rcell;
		ifs.read((char*)&rcell, sizeof(rcell));

		std::string code = table[rcell];

		for (int i = 0; i < code.size(); i++) {
			if (code[i] == '0') {
				set0(wcell, k);
				k++;
				if (k == 8) {
					ofs.write((char*)&wcell, sizeof(char));
					wcell = 0;
					k = 0;
				}
			}
			else {
				set1(wcell, k);
				k++;
				if (k == 8) {
					ofs.write((char*)&wcell, sizeof(char));
					wcell = 0;
					k = 0;
				}
			}
		}
	}
	ifs.close();
	ofs.close();
	delete[] myVec;
}

void decode(std::string fileToDecode, std::string outputName) {
	std::ifstream ifs(fileToDecode, std::ios::binary);

	std::map<unsigned char, unsigned int> freqTable;

	unsigned char c;
	ifs.read((char*)&c, sizeof(c));

	Node* Vec = new Node[c + c - 1];
	int k = 0;
	for (unsigned char i = 0; i < c; i++) {
		unsigned char let;
		unsigned int freq;
		ifs.read((char*)&let, sizeof(c));
		ifs.read((char*)&freq, sizeof(freq));
		Vec[k] = Node(freq, let);
		k++;
		freqTable[let] = freq;
	}
	Node decTree = getTree(Vec, k);

	std::map<std::string, unsigned char> deCodes;
	getDeCodes(decTree, deCodes, "");

	std::string code = "";

	std::ofstream ofs(outputName, std::ios::binary);

	while (!ifs.eof()) {

		unsigned char cell;
		ifs.read((char*)&cell, sizeof(cell));

		for (int i = 0; i < 8; i++) {

			if (getk(cell, i)) code = code + "1";
			else code = code + "0";

			std::map<std::string, unsigned char>::iterator it;
			it = deCodes.find(code);

			if (it != deCodes.end()) {
				ofs.write((char*)&deCodes[code], sizeof(char));
				code = "";
			}
		}
	}
	delete[] Vec;
	ifs.close();
	ofs.close();
}