#pragma once

#include "../Utils/UnionFind.h"

#include <functional>
#include <list>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <unordered_map>
#include <vector>

using namespace std;

class Graph
{
public:
    struct Edge {
        int i;
        int j;
        int weight;
        int index;
        bool disabled = false;
    };

    Graph();
    Graph(const vector<vector<int>>& adjacencyMatrix);
    Graph(const vector<tuple<int, int, int>>& edges);

    // add new edge to the graph
    void addEdge(int i, int j, int weight, optional<int> index = nullopt);
    // mark edge with id "index" to be excluded from further consideration
    void disableEdge(int index);

    int getNodesNumber() const { return m_unionFind.getComponentNumber(); }
    
    set<int> getNodes() { return m_unionFind.getComponents();  }

    list<int> boruvkaPhase(int count = 1, bool* noChanges = nullptr);
    Edge& getEdge(int index) { return m_edges[index]; }  

    const auto& getEdges() const { return m_edges; }

    // apply specific action to the valid edges
    void findValidEdges(function<void(const Edge&, int, int)> action);

    // find representative (parent) of node 
    int getRoot(int node) { return m_unionFind.find(node); }

    void unionNodes(int a, int b);

    const UnionFind& getUnionFind() const { return m_unionFind; }
    UnionFind& getUnionFind() { return m_unionFind; }

private:
    unordered_map<int, Edge> m_edges{};
   
    UnionFind m_unionFind;
};