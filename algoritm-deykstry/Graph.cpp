#include "Graph.h"
#include <algorithm>
#include <functional>
#include <queue>
#include <utility>

const long long Graph::INF = 1000000000000000000LL;

Graph::Graph(int vertexCount)
    : m_n(vertexCount), m_adj(static_cast<size_t>(vertexCount) + 1) {
}

void Graph::addEdge(int from, int to, int weight)
{
    if (from < 1 || from > m_n || to < 1 || to > m_n || weight < 0) return;
    Edge e = { to, weight };
    m_adj[from].push_back(e);
}

void Graph::dijkstra(int source, std::vector<long long>& dist, std::vector<int>& parent) const
{
    dist.assign(static_cast<size_t>(m_n) + 1, INF);
    parent.assign(static_cast<size_t>(m_n) + 1, 0);
    if (source < 1 || source > m_n) return;

    typedef std::pair<long long, int> Item;   // (расстояние, вершина)
    std::priority_queue<Item, std::vector<Item>, std::greater<Item> > queue;

    dist[source] = 0;
    queue.push(Item(0, source));

    while (!queue.empty()) {
        Item top = queue.top();
        queue.pop();
        long long d = top.first;
        int v = top.second;
        if (d > dist[v]) continue;            // устаревшая запись в очереди

        for (size_t k = 0; k < m_adj[v].size(); ++k) {
            const Edge& e = m_adj[v][k];
            long long nd = d + e.weight;
            if (nd < dist[e.to]) {            // релаксация ребра
                dist[e.to] = nd;
                parent[e.to] = v;
                queue.push(Item(nd, e.to));
            }
        }
    }
}

bool Graph::shortestPath(int from, int to, std::vector<int>& path, long long& length) const
{
    path.clear();
    length = 0;
    if (from < 1 || from > m_n || to < 1 || to > m_n) return false;

    std::vector<long long> dist;
    std::vector<int> parent;
    dijkstra(from, dist, parent);
    if (dist[to] >= INF) return false;

    for (int v = to; v != 0; v = parent[v]) path.push_back(v);
    std::reverse(path.begin(), path.end());
    length = dist[to];
    return true;
}
