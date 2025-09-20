#include "Graph.h"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <unordered_set>

namespace
{
int maxVertexId(const vector<tuple<int, int, int>>& edges) {
    int maxNode = -1;

    for (auto& [u, v, weight] : edges) {
        maxNode = max(maxNode, max(u, v));
    }

    return maxNode;
}
}

Graph::Graph()
    : m_unionFind(0)
{}

Graph::Graph(const vector<vector<int>>& adjacencyMatrix)
    : m_unionFind(adjacencyMatrix.size())
{
    for (int i = 0; i < adjacencyMatrix.size(); ++i){
        // (i, j) is the same edge as (j, i)
        for (int j = 0; j < i; ++j) {
            addEdge(i, j, max(adjacencyMatrix[i][j], adjacencyMatrix[j][i]));
        }
    }
}

Graph::Graph(const vector<tuple<int, int, int>>& edges)
    : m_unionFind(maxVertexId(edges))
{
    for (auto& [i, j, weight] : edges) {
        addEdge(i, j, weight);
    }
}

void Graph::unionNodes(int a, int b) {

    m_unionFind.ensureSize(std::max(a, b) + 1);
    m_unionFind.unionOperation(a, b);
}

void Graph::addEdge(int i, int j, int weight, optional<int> index){

    if (weight == 0) return;

    m_unionFind.ensureSize(std::max(i, j) + 1);

    auto currentIndex = index.value_or(m_edges.size());
    // in case m_edges is not empty, add a new one to the end of m_edges
    m_edges.emplace(currentIndex, Edge{ min(i, j), max(i, j), weight, currentIndex });
}

void Graph::findValidEdges(function<void(const Edge&, int, int)> action)
{
    for (auto edge = m_edges.begin(); edge != m_edges.end();){

        if (edge->second.disabled){
            edge = m_edges.erase(edge);
            continue;
        }

        const auto fi = m_unionFind.find(edge->second.i);
        const auto fj = m_unionFind.find(edge->second.j);

        // valid edges are the ones that are not disabled and their endpoints belong to different components
        if (fi == fj){

            edge = m_edges.erase(edge);
            continue;
        }

        action(edge->second, fi, fj);

        edge++;
    }
}

list<int> Graph::boruvkaPhase(int count, bool* noChanges){

    if (noChanges) {
        *noChanges = false;
    }

    list<int> result;

    // apply Boruvka step "count" times
    for (int phase = 0; phase < count; ++phase) {

        // cheapest edge to each node (connected component)
        unordered_map<int, pair<int, int>> cheapestEdgeToEachNode;

        // find valid (cheapest edge) between components
        findValidEdges([&](const Edge& edge, int u, int v) {
            // edge must be valid
            if (u == v) return; 

            auto consider = [&](int node) {
                auto it = cheapestEdgeToEachNode.find(node);
                if (it == cheapestEdgeToEachNode.end() || edge.weight < it->second.second) {
                    cheapestEdgeToEachNode[node] = { edge.index, edge.weight };
                }
                };
            consider(u);
            consider(v);
            }
        );

        // add edge
        bool mergeComponents = false;
        unordered_set<int> usedEdges;

        for (const auto& cheapestEdge : cheapestEdgeToEachNode) {
            const int edgeIndex = cheapestEdge.second.first;

            // already processed
            if (!usedEdges.insert(edgeIndex).second) continue;

            auto edgeIt = m_edges.find(edgeIndex);
            // already removed 
            if (edgeIt == m_edges.end()) continue;

            Edge edge = edgeIt->second;
            m_edges.erase(edgeIt);

            const int fi = m_unionFind.find(edge.i);
            const int fj = m_unionFind.find(edge.j);

            // nodes belong to the same component
            if (fi == fj) continue;

            // merge components
            m_unionFind.unionOperation(fi, fj);

            result.push_back(edgeIndex);
            mergeComponents = true;
        }

        // if no merging in this phase it ends Boruvka's step
        if (!mergeComponents) {
            if (noChanges) {
                *noChanges = true;
            }
            break;
        }
    }

    return result;
}

void Graph::disableEdge(int index){

    m_edges[index].disabled = true;
}