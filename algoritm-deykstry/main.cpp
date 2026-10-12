#include <iostream>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
#include "Graph.h"

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    // Граф из задания: 9 вершин, рёбра направленные (from, to, вес)
    Graph g(9);
    g.addEdge(1, 2, 10);
    g.addEdge(1, 3, 6);
    g.addEdge(1, 4, 8);
    g.addEdge(2, 4, 5);
    g.addEdge(2, 5, 13);
    g.addEdge(2, 7, 11);
    g.addEdge(3, 5, 3);
    g.addEdge(4, 3, 2);
    g.addEdge(4, 5, 5);
    g.addEdge(4, 6, 7);
    g.addEdge(4, 7, 12);
    g.addEdge(5, 6, 9);
    g.addEdge(5, 9, 12);
    g.addEdge(6, 8, 8);
    g.addEdge(6, 9, 10);
    g.addEdge(7, 6, 4);
    g.addEdge(7, 8, 6);
    g.addEdge(7, 9, 16);
    g.addEdge(8, 9, 15);

    const int from = 1, to = 9;

    std::vector<int> path;
    long long length = 0;
    if (!g.shortestPath(from, to, path, length)) {
        std::cout << "Пути из вершины " << from << " в вершину " << to << " не существует\n";
        return 1;
    }

    std::cout << "Оптимальный путь из " << from << "-й вершины в " << to << "-ю: ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i) std::cout << " -> ";
        std::cout << path[i];
    }
    std::cout << "\nДлина пути: " << length << "\n";
    return 0;
}
