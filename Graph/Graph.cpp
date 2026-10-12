#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include "Graph.h"

// ---------- вспомогательные функции для работы с файлами ----------

// разбиение строки на слова (разделители: запятая, точка с запятой, табуляция, пробел)
static std::vector<std::string> splitLine(const std::string& line) {
	std::vector<std::string> words;
	std::string cur;
	for (size_t i = 0; i < line.size(); i++) {
		char c = line[i];
		if (c == ',' || c == ';' || c == '\t' || c == ' ') {
			if (!cur.empty()) words.push_back(cur);
			cur.clear();
		}
		else {
			cur += c;
		}
	}
	if (!cur.empty()) words.push_back(cur);
	return words;
}

// состоит ли строка только из цифр
static bool isNumber(const std::string& s) {
	if (s.empty()) return false;
	for (size_t i = 0; i < s.size(); i++)
		if (s[i] < '0' || s[i] > '9') return false;
	return true;
}

// сравнение имен: числа сравниваем как числа (2 < 10), остальное как строки
static bool lessName(const std::string& a, const std::string& b) {
	if (isNumber(a) && isNumber(b)) {
		if (a.size() != b.size()) return a.size() < b.size();
	}
	return a < b;
}

static bool lessNode(Node* a, Node* b) {
	return lessName(a->getName(), b->getName());
}

typedef std::pair<std::string, std::string> Edge;

static bool lessEdge(const Edge& a, const Edge& b) {
	if (a.first != b.first) return lessName(a.first, b.first);
	return lessName(a.second, b.second);
}

// ---------- класс Graph ----------

// создание графа из файла: каждая строка - пара смежных вершин
Graph::Graph(const char* file_name) : separator(','), loaded(false) {
	std::ifstream file(file_name);
	if (!file.is_open()) return;
	loaded = true;

	std::map<std::string, Node*> byName; // чтобы для одного имени создавался один узел
	std::string line;
	bool firstLine = true;

	while (std::getline(file, line)) {
		// убираем символ BOM в начале файла и перевод строки Windows
		if (firstLine && line.size() >= 3 && (unsigned char)line[0] == 0xEF
			&& (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
			line.erase(0, 3);
		while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
			line.erase(line.size() - 1);

		std::vector<std::string> w = splitLine(line);
		if (w.size() < 2) continue; // пустая или некорректная строка

		if (firstLine) {
			firstLine = false;
			// определяем, какой разделитель использует файл
			const char seps[] = { ',', ';', '\t', ' ' };
			for (int i = 0; i < 4; i++)
				if (line.find(seps[i]) != std::string::npos) {
					separator = seps[i];
					break;
				}
			// если первая строка не числа (например, Source,Target) - это заголовок
			if (!isNumber(w[0]) || !isNumber(w[1])) {
				header = line;
				continue;
			}
		}

		// создаем узлы (если их еще нет) и ребро между ними
		Node* n[2];
		for (int i = 0; i < 2; i++) {
			std::map<std::string, Node*>::iterator it = byName.find(w[i]);
			if (it == byName.end()) {
				n[i] = new Node(w[i]);
				byName[w[i]] = n[i];
				addNode(n[i]);
			}
			else {
				n[i] = it->second;
			}
		}
		addEdge(n[0], n[1]);
	}
}

// деструктор
Graph::~Graph() {
	for (std::set<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++)
		delete* it;
}

// добавление нового узла
void Graph::addNode(Node* node) {
	nodes.insert(node);
}

// добавление связи с другим узлом
void Node::addNeighbour(Node* neighbour) {
	neighbours.insert(neighbour);
}

// удаление узла из списока соседей
void Node::removeNeighbour(Node* neighbour) {
	neighbours.erase(neighbour);
}

// удаление узла
void Graph::removeNode(Node* node) {
	nodes.erase(node);
	// Remove also from all neighbours list
	for (std::set<Node*>::iterator it = nodes.begin();
		it != nodes.end(); it++) {
		(*it)->removeNeighbour(node);
	}
}

// создание ребра
void Graph::addEdge(Node* begin, Node* end) {
	if (nodes.find(begin) == nodes.end()) // существует ли начальный узел в графе
		return;
	if (nodes.find(end) == nodes.end()) // существует ли конечный узел в графе
		return;
	begin->addNeighbour(end); // добавляем конечный узел в список соседей начального
	end->addNeighbour(begin); // добавляем начальный узел в список соседей конечного
}

// количество ребер (каждое ребро считаем один раз)
int Graph::edgeCount() const {
	int count = 0;
	for (node_iterator a = nodes.begin(); a != nodes.end(); a++)
		for (node_iterator b = (*a)->nb_begin(); b != (*a)->nb_end(); b++)
			if (*a <= *b) count++;
	return count;
}

// поиск узла по имени
Node* Graph::findNode(const std::string& name) const {
	for (node_iterator it = nodes.begin(); it != nodes.end(); it++)
		if ((*it)->getName() == name) return *it;
	return nullptr;
}

// все узлы, отсортированные по имени
std::vector<Node*> Graph::sortedNodes() const {
	std::vector<Node*> v(nodes.begin(), nodes.end());
	std::sort(v.begin(), v.end(), lessNode);
	return v;
}

// выделение непересекающихся графов с помощью BFS и запись каждого в свой файл
int Graph::saveComponents(const std::string& prefix) const {
	std::vector<Node*> sorted = sortedNodes();
	BFS bfs(*this);
	std::set<Node*> seen; // узлы, которые уже попали в какой-то граф
	int count = 0;

	for (size_t i = 0; i < sorted.size(); i++) {
		if (seen.find(sorted[i]) != seen.end()) continue;

		// BFS от первого непосещенного узла находит весь его непересекающийся граф
		std::set<Node*> comp = bfs.component(sorted[i]);
		seen.insert(comp.begin(), comp.end());
		count++;

		// собираем ребра этого графа (каждое один раз, меньшее имя первым)
		std::vector<Edge> edges;
		for (std::set<Node*>::iterator a = comp.begin(); a != comp.end(); a++)
			for (node_iterator b = (*a)->nb_begin(); b != (*a)->nb_end(); b++)
				if (*a <= *b) {
					std::string x = (*a)->getName(), y = (*b)->getName();
					if (lessName(y, x)) std::swap(x, y);
					edges.push_back(Edge(x, y));
				}
		std::sort(edges.begin(), edges.end(), lessEdge);

		// пишем в файл в той же структуре, что и входной файл
		std::string fileName = prefix + "_" + std::to_string(count) + ".txt";
		std::ofstream out(fileName.c_str());
		if (!header.empty()) out << header << "\n";
		for (size_t e = 0; e < edges.size(); e++)
			out << edges[e].first << separator << edges[e].second << "\n";
	}
	return count;
}

// ---------- алгоритмы поиска ----------

// проверка связности через алгоритм поиска в ширину(BFS)
bool BFS::connected(Node* begin, Node* end) {
	// создание очереди для хранения узлов, подлежащих проверке
	std::queue<Node*> nodes; nodes.push(begin);
	std::set<Node*> visited; // создание множества для учета уже посещенных узлов
	visited.insert(begin);
	// пока очередь не пуста, продолжаем поиск
	while (!nodes.empty()) {
		// берем первый элемент из очереди и удаляем из очереди
		Node* next = nodes.front(); nodes.pop();
		if (end == next) return true;
		// перебираем всех соседей текущего узла
		for (node_iterator it = next->nb_begin();
			it != next->nb_end(); it++)
			// если сосед еще не был посещен - помечаем и добавляем в очередь
			if (visited.insert(*it).second)
				nodes.push(*it);
	}
	return false;
}

// все узлы, достижимые из start (то есть непересекающийся граф, содержащий start)
std::set<Node*> BFS::component(Node* start) {
	std::queue<Node*> nodes; nodes.push(start);
	std::set<Node*> visited;
	visited.insert(start);
	while (!nodes.empty()) {
		Node* next = nodes.front(); nodes.pop();
		for (node_iterator it = next->nb_begin();
			it != next->nb_end(); it++)
			if (visited.insert(*it).second)
				nodes.push(*it);
	}
	return visited;
}

// запуск поиска в глубину (DFS)
bool DFS::connected(Node* begin, Node* end) {
	visited.clear(); return connected(begin, end, 0);
}

// рекурсивный поиск в глубину
bool DFS::connected(Node* begin, Node* end, int depth) {
	if (begin == end) return true;
	visited.insert(begin);
	// перебираем всех соседей текущего узла
	for (node_iterator it = begin->nb_begin();
		it != begin->nb_end(); it++) {
		// если сосед еще не был посещен
		if (visited.find(*it) == visited.end()) {
			// рекурсивно переходим к соседу и проверяем путь от него
			if (connected(*it, end, depth + 1)) return true;
		}
	}
	return false;
}
