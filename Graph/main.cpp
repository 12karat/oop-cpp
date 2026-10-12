#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "Graph.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// имя файла без расширения (TestGraph.txt -> TestGraph), нужно для названий выходных файлов
string withoutExtension(const string& name) {
    size_t dot = name.find_last_of('.');
    size_t slash = name.find_last_of("/\\");
    if (dot == string::npos || (slash != string::npos && dot < slash))
        return name;
    return name.substr(0, dot);
}

// (I) демонстрация методов Node и Graph на небольшом графе
void demoSmallGraph() {
    cout << "I. Проверка методов Graph и Node, поиск BFS и DFS" << endl;

    Graph g;
    // узлы создаются динамически, граф сам удалит их в деструкторе
    Node* n1 = new Node("A");
    Node* n2 = new Node("B");
    Node* n3 = new Node("C");
    Node* n4 = new Node("D");

    g.addNode(n1);
    g.addNode(n2);
    g.addNode(n3);
    g.addNode(n4);

    // создаем связи: A-B, B-C, узел D остается отдельно
    g.addEdge(n1, n2);
    g.addEdge(n2, n3);

    BFS bfs(g);
    DFS dfs(g);

    cout << "Узлов: " << g.nodeCount() << ", ребер: " << g.edgeCount() << endl;
    cout << "BFS (A to C): " << (bfs.connected(n1, n3) ? "Yes" : "No") << endl;
    cout << "DFS (A to C): " << (dfs.connected(n1, n3) ? "Yes" : "No") << endl;
    cout << "BFS (A to D): " << (bfs.connected(n1, n4) ? "Yes" : "No") << endl;
    cout << "DFS (A to D): " << (dfs.connected(n1, n4) ? "Yes" : "No") << endl;

    // удаляем узел B: связь между A и C пропадает
    g.removeNode(n2);
    delete n2; // удаленный из графа узел граф уже не удалит, поэтому удаляем сами
    cout << "После удаления B -> узлов: " << g.nodeCount() << ", ребер: " << g.edgeCount() << endl;
    cout << "BFS (A to C): " << (bfs.connected(n1, n3) ? "Yes" : "No") << endl;
    cout << endl;
}

// (III) проверка корректности BFS на графе из файла:
// результат BFS сравниваем с результатом DFS и с множеством, найденным BFS::component
void checkBFS(const Graph& g) {
    cout << "III. Проверка корректности BFS" << endl;

    BFS bfs(g);
    DFS dfs(g);
    vector<Node*> nodes = g.sortedNodes();

    int checks = 0, errors = 0;
    // проверяем первые 20 узлов относительно всех остальных
    for (size_t i = 0; i < nodes.size() && i < 20; i++) {
        set<Node*> comp = bfs.component(nodes[i]);
        for (size_t j = 0; j < nodes.size(); j++) {
            bool inComp = comp.find(nodes[j]) != comp.end();
            bool byBfs = bfs.connected(nodes[i], nodes[j]);
            bool byDfs = dfs.connected(nodes[i], nodes[j]);
            checks++;
            if (byBfs != inComp || byBfs != byDfs) errors++;
        }
    }
    cout << "Проверено пар вершин: " << checks << ", расхождений: " << errors << endl;
    if (errors == 0) cout << "BFS работает корректно" << endl;
    else cout << "Найдены ошибки в BFS!" << endl;
    cout << endl;
}

int main() {
#ifdef _WIN32
    // чтобы русский текст нормально отображался в консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    demoSmallGraph();

    // работа с файлами: сначала TestGraph.txt (задачи II, III, IV), потом 1000.csv (задача V)
    cout << "Введите имя файла с графом (например TestGraph.txt или 1000.csv)." << endl;
    cout << "Пустая строка - выход." << endl;

    string fileName;
    while (true) {
        cout << endl << "Имя файла: ";
        getline(cin, fileName);
        if (fileName.empty()) break;

        // (II) формирование графа по данным файла
        Graph g(fileName.c_str());
        if (!g.isLoaded()) {
            cout << "Не удалось открыть файл " << fileName << endl;
            continue;
        }
        cout << "II. Граф создан: узлов " << g.nodeCount() << ", ребер " << g.edgeCount() << endl;
        cout << endl;

        // (III) проверка BFS
        checkBFS(g);

        // (IV, V) выделение непересекающихся графов и запись в отдельные файлы
        string prefix = withoutExtension(fileName) + "_graph";
        int count = g.saveComponents(prefix);
        cout << "IV/V. Найдено непересекающихся графов: " << count << endl;
        cout << "Файлы записаны: " << prefix << "_1.txt ... " << prefix << "_" << count << ".txt" << endl;

        // размеры найденных графов
        BFS bfs(g);
        set<Node*> seen;
        vector<Node*> nodes = g.sortedNodes();
        int number = 0;
        for (size_t i = 0; i < nodes.size(); i++) {
            if (seen.find(nodes[i]) != seen.end()) continue;
            set<Node*> comp = bfs.component(nodes[i]);
            seen.insert(comp.begin(), comp.end());
            number++;
            if (number <= 10)
                cout << "  граф " << number << ": " << comp.size() << " вершин" << endl;
        }
        if (count > 10) cout << "  ..." << endl;
    }

    return 0;
}
