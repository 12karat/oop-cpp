#pragma once
#include <cstddef>
#include <vector>

// Ориентированный взвешенный граф (вершины нумеруются с 1)
class Graph
{
public:
    explicit Graph(int vertexCount);

    // Добавить ребро from -> to с весом weight (weight >= 0)
    void addEdge(int from, int to, int weight);

    int vertexCount() const { return m_n; }

    // Алгоритм Дейкстры: кратчайшие расстояния от вершины source до всех остальных.
    // parent[v] - предыдущая вершина на кратчайшем пути (0 - нет).
    // Недостижимые вершины получают расстояние INF.
    void dijkstra(int source, std::vector<long long>& dist, std::vector<int>& parent) const;

    // Кратчайший путь from -> to. Возвращает false, если пути нет.
    // path - вершины пути по порядку, length - его длина.
    bool shortestPath(int from, int to, std::vector<int>& path, long long& length) const;

    static const long long INF;

private:
    struct Edge {
        int to;
        int weight;
    };

    int m_n;
    std::vector<std::vector<Edge> > m_adj;   // списки смежности
};
