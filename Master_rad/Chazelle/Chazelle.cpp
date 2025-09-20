#include "Graph.h"
#include "MSTUtils.h"
#include "TreeBuilder.h"

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

namespace
{
    int calculateParameterT(Graph& graph, int d)
    {
        if (d == 1) {
            return 1;
        }

        double nodesNumber = static_cast<double>(graph.getNodesNumber());

        int result = 1;
        while (nodesNumber > pow(MST::S(result, d), 3)) {
            ++result;
        }

        return result;
    }
}

class Chazelle {
public:
	Chazelle(vector<vector<pair<int, int>>>& adjacencyList)
		: m_nodesNumber(adjacencyList.size())
	{
        vector<tuple<int, int, int>> edges = {};

        for (int i = 0; i < m_nodesNumber; ++i) {
            for (int j = 0; j < adjacencyList[i].size(); ++j) {
                if (i < adjacencyList[i][j].first) {
                    int u = i;
                    int v = adjacencyList[i][j].first;
                    int weight = adjacencyList[i][j].second;
                    edges.push_back({ u, v, weight });
                }
            }
        }

        // create graph
        Graph graph = Graph(edges);

        // MST construction
        int height = MST::findMaxHeight(graph, MST::c);

        list<int> result = findMST(graph, height);

        // print result:
        for (int edgeIndex : result) {
            cout << "( " << get<0>(edges[edgeIndex]) << ", " << get<1>(edges[edgeIndex]) << ") weight: " << get<2>(edges[edgeIndex]) << endl;
        }

	}

    list<int> findMST(Graph& graph, int treeHeight, int t = 0)
    {
        if (!t) {
            t = calculateParameterT(graph, treeHeight <= 2 ? 3 : treeHeight);
        }

        int count = t <= 1 ? numeric_limits<int>::max() : MST::c;

        bool noChanges = false;
        list<int> boruvkaResult = graph.boruvkaPhase(count, &noChanges);
        // if Boruvka phase hasn't produced any changes the algorithm stops here and solution has been found
        if (noChanges) {
            return boruvkaResult;
        }

        set<int> nodes = graph.getNodes();
        list<int> badEdges = {};
        list<Graph> graphs{};

        // while there is any unprocessed component
        while (!nodes.empty())
        {
            auto  treeBuilder = TreeBuilder(graph, t, treeHeight, *nodes.begin());
            auto& tree = treeBuilder.getTree();

            for (const auto& node : tree.getVerticesInside()) {
                nodes.erase(node);
            }

            auto currentBadEdges = tree.getBadEdges();

            graphs.splice(graphs.end(), tree.createSubGraphs(currentBadEdges));
            std::ranges::move(currentBadEdges, std::inserter(badEdges, badEdges.end()));
        }

        list<int> badEdgesCopy = badEdges;
        for (auto& subgraph : graphs) {
            badEdgesCopy.splice(badEdgesCopy.end(), findMST(subgraph, treeHeight, t > 1 ? t - 1 : t));
        }

        Graph newGraph{};
        for (const auto& edgeIndex : badEdgesCopy)
        {
            auto& originalEdge = graph.getEdge(edgeIndex);
            auto fi = graph.getRoot(originalEdge.i);
            auto fj = graph.getRoot(originalEdge.j);
            newGraph.addEdge(fi, fj, originalEdge.weight, edgeIndex);
        }

        boruvkaResult.splice(boruvkaResult.end(), findMST(newGraph, treeHeight, t));
       
        return boruvkaResult;
    }

private:
	int m_nodesNumber{ 0 };
};