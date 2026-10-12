#pragma once
#include <iostream>
#include <set>
#include <queue>
#include <string>
#include <vector>

// предварительное обьявление класса узла
class Node;
// определение типа итератора для удобного обхода множества указателей на узлы
typedef std::set<Node*>::const_iterator
node_iterator;

// граф
class Graph {
	std::set<Node*> nodes; // множество всех узлов (граф владеет узлами и удаляет их в деструкторе)
	std::string header;    // первая строка файла (если была), нужна чтобы записать файлы в том же формате
	char separator;        // разделитель столбцов в файле (запятая, точка с запятой, табуляция или пробел)
	bool loaded;           // удалось ли открыть файл при создании графа

	// копировать граф запрещено (иначе узлы удалились бы дважды)
	Graph(const Graph&);
	Graph& operator=(const Graph&);
public:
	// пустой граф
	Graph() : separator(','), loaded(true) {}
	// создание графа из файла со списком ребер (пары смежных вершин)
	Graph(const char* file_name);
	// деструктор: удаляет все узлы, оставшиеся в графе
	~Graph();

	// удалось ли прочитать файл
	bool isLoaded() const { return loaded; }
	// добавление нового узла
	void addNode(Node* node);
	// удаление узла (из графа и из списков соседей; сам объект узла вызывающий удаляет сам)
	void removeNode(Node* node);
	// создание ребра
	void addEdge(Node* begin, Node* end);
	// количество узлов
	int nodeCount() const { return (int)nodes.size(); }
	// количество ребер
	int edgeCount() const;
	// поиск узла по имени (nullptr, если такого нет)
	Node* findNode(const std::string& name) const;
	// все узлы, отсортированные по имени
	std::vector<Node*> sortedNodes() const;
	// выделение непересекающихся графов (поиском в ширину) и запись каждого в отдельный файл
	// prefix_1.txt, prefix_2.txt, ... ; возвращает количество найденных графов
	int saveComponents(const std::string& prefix) const;

	// итератор на первый узел
	node_iterator begin() const {
		return nodes.begin();
	}

	// получение итератора, указывающего на конец множества узлов
	node_iterator end() const { return nodes.end(); }
};

// узел
class Node {
	std::string name; // имя узла
	std::set<Node*> neighbours; // множество смежных узлов (соседи)
	// добавление связи с другим узлом
	void addNeighbour(Node* neighbour);
	// удаление узла из списока соседей
	void removeNeighbour(Node* neighbour);
public:
	// конструктор: создание узла заданным именем
	Node(const std::string& aname) : name(aname) {}
	// получение имени текущего узла
	const std::string& getName() const { return name; }
	// итератор, указывающий на первого соседа в списке смежности
	node_iterator nb_begin() const {
		return neighbours.begin();
	}
	// итератор, указывающий на элемент за последним соседом
	node_iterator nb_end() const { return neighbours.end(); }
	// разраешаем классу Graph доступ к приватным методам Node
	friend class Graph;
};

// класс для реализации алгоритма поиска в ширину (Breadth-First Search)
class BFS {
	const Graph& graph; // ссылка на граф, в котором будет производиться поиск
public:
	// конструктор: связывает объект BFS с конкретным графом
	BFS(const Graph& agraph) : graph(agraph) {}
	// метод проверки связности двух узлов
	bool connected(Node* begin, Node* end);
	// множество всех узлов, достижимых из заданного (его непересекающийся граф)
	std::set<Node*> component(Node* start);
};

// класс для реализации алгоритма поиска в глубину (Depth-First Search)
class DFS {
	const Graph& graph; // ссылка на исследуемый граф
	std::set<Node*> visited; // вспомогательное множество для хранения уже посещенных узлов
	// внутренний рекурсивный метод поиска с учетом текущей глубины рекурсии
	bool connected(Node* begin, Node* end, int depth);
public:
	// конструктор: связывает объект DFS с конкретным графом
	DFS(const Graph& agraph) : graph(agraph) {}
	// метод для запуска проверки наличия пути между узлами
	bool connected(Node* begin, Node* end);
};
