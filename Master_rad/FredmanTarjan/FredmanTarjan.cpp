#include "../Utils/UnionFind.h"
#include "../Utils/FibonacciHeap.cpp"

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <unordered_set>

#include <chrono>

using namespace std;

using Clock = std::chrono::steady_clock;
using ms = std::chrono::duration<double, std::milli>;

class FredmanTarjan {
public:
	FredmanTarjan(vector<vector<pair<int, int>>>& adjacencyList)
		: m_nodesNumber(adjacencyList.size())
 	{
		// initialize m_adjacencyList
		for (int i = 0; i < adjacencyList.size(); ++i) {
			m_nodes.insert(i);
			unordered_map<int, int> currentNeighbors = {};

			for (int j = 0; j < adjacencyList[i].size(); ++j) {
				currentNeighbors.insert(adjacencyList[i][j]);
				// no need to consider both (i, adjacencyList[i][j].first) and (adjacencyList[i][j].first, i)
				if (i < adjacencyList[i][j].first) m_edgesNumber++;
			}
			m_adjacencyList[i] = currentNeighbors;
		}

		auto start = Clock::now();
		UnionFind unionFind(m_nodesNumber);
		findMST(unionFind);
		auto end = Clock::now();

		m_duration = std::chrono::duration_cast<ms>(end - start).count();

		printResult();

		cout << "Fredman-Tarjan: " << m_mstWeight << endl;
	}

public:
	// unionFind parameter is a struct that has info about connected components
	void findMST(UnionFind& unionFind) {

		if (!m_heapSize) m_heapSize = max(1, m_edgesNumber / max(1, m_nodesNumber));
		else m_heapSize = max(1, (int)pow(2, (double)m_heapSize));

		// all nodes that have been already added to the heap
		unordered_set<int> visited;

		// go through all the nodes
		for (int i = 0; i < m_nodesNumber; ++i) {

			// process the node in case it's not been visited yet
			if (visited.find(unionFind.find(i)) == visited.end()) {

				fibonacciHeap heap;
				fibonacciHeapHandle position;

				// reference to each element from the heap
				unordered_map<int, fibonacciHeapHandle>nodePosition;

				// take all neighbors of the current node
				const auto& neighbors = m_adjacencyList[i];

				// go through all the neighbors of i
				for (auto neighbor = neighbors.begin(); neighbor != neighbors.end(); neighbor++) {
					auto currentPair = make_pair(neighbor->first, neighbor->second);
					position = heap.emplace(currentPair);

					nodePosition[neighbor->first] = position;
				}

				while (!heap.empty()) {
					// take the top one, it's the cheapest one
					auto currentNode = heap.top();

					bool exceededHeapSize = heap.size() > m_heapSize;
					bool currentNodeAlreadyVisited = visited.find(currentNode.first) != visited.end();
					bool sameComponent = unionFind.connected(currentNode.first, i);

					if (exceededHeapSize || currentNodeAlreadyVisited || sameComponent) break;

					// marked as visited and can be removed from the heap
					visited.insert(i);
					visited.insert(currentNode.first);

					heap.pop();

					// the is processed successfully
					m_result.push_back({ i, currentNode.first, currentNode.second });

					// current node and it's parent can be merged - put the into the same component
					unionFind.unionOperation(i, currentNode.first);
				
					// go through current's node neighbors
					const auto& currentNodeNeighbors = m_adjacencyList[currentNode.first];
					fibonacciHeapHandle currentPosition;

					for (auto currentNodeNeighbor = currentNodeNeighbors.begin(); currentNodeNeighbor != currentNodeNeighbors.end(); currentNodeNeighbor++) {
						
						// shouldn't go back to the parent again
						if (currentNodeNeighbor->first == i) continue;

						// add nodes that have not been already added
						if (nodePosition.find(currentNodeNeighbor->first) == nodePosition.end()) {
							position = heap.emplace(make_pair(currentNodeNeighbor->first, currentNodeNeighbor->second));
							nodePosition[currentNodeNeighbor->first] = position;
						}
						// update ones that have been already added
						else {
							// these shouldn't be processed before (maybe it's removed, it has to be checked)
							if (visited.find(currentNodeNeighbor->first) == visited.end()) {
								currentPosition = nodePosition[currentNodeNeighbor->first];

								if ((*currentPosition).second > (*currentNodeNeighbor).second) {
									heap.update(currentPosition, make_pair(currentNodeNeighbor->first, currentNodeNeighbor->second));
								}
							}
						}
					}

				}

			}

		}

		// only one component has left - MST is created, otherwise contraction and recursive call must be done
		if (unionFind.getComponentNumber() == 1) return;

		unordered_map<int, unordered_map<int, int>> newAdjacencyList;
		unordered_set<int> newNodes;

		// update set of nodes using unionFind
		// go through current set of nodes
		for (auto& node : m_nodes) {
			int component = unionFind.find(node);
			// insert only if the current node is not already added
			if (newNodes.find(component) == newNodes.end()) {
				newNodes.insert(component);
			}
		}
		m_nodes = newNodes;


		unordered_map<int, int> neighbors;
		// initialization of newAdjacency list
		for (auto it = newNodes.begin(); it != newNodes.end(); it++)
			newAdjacencyList[*it] = neighbors;

		// go through initial m_adjacencyList and initialize newAdjacencyList
		for (auto currentNode = m_adjacencyList.begin(); currentNode != m_adjacencyList.end(); currentNode++) {
			// adjacency list for current currentNeighbors 
			for (auto neighbor = currentNode->second.begin(); neighbor != currentNode->second.end(); neighbor++) {

				// only if the current node and its current neighbor do not belong to the same component
				if (!unionFind.connected(currentNode->first, neighbor->first)) {
					// add if it's not added yet to the new list
					if ((newAdjacencyList[unionFind.find(currentNode->first)]).find(unionFind.find(neighbor->first)) == (newAdjacencyList[unionFind.find(currentNode->first)]).end()) {
						newAdjacencyList[unionFind.find(currentNode->first)][unionFind.find(neighbor->first)] = neighbor->second;
					}
					// update value if it's added, but with greater weight
					else if (newAdjacencyList[unionFind.find(currentNode->first)][unionFind.find(neighbor->first)] > neighbor->second) {
						newAdjacencyList[unionFind.find(currentNode->first)][unionFind.find(neighbor->first)] = neighbor->second;
					}
				}
			}
		}


		m_adjacencyList = newAdjacencyList;
		// solve the same, but contracted problem using recursion
		findMST(unionFind);
	}
	
	void printResult() {

		for (auto& edge : m_result) {
			//cout << "(" << get<0>(edge) << ", " << get<1>(edge) << ") weight: " << get<2>(edge) << ")" << endl;
			m_mstWeight += get<2>(edge);
		}
	}

	size_t getMSTWeight() { return m_mstWeight; }

	double getDuration() { return m_duration; }

private:
	// size of the heap that is controlled carefully and different in different recursive calls
	int m_heapSize{ 0 };
	int m_nodesNumber{ 0 };
	int m_edgesNumber{ 0 };

	size_t m_mstWeight{ 0 };
	double m_duration{ 0 };

	unordered_set<int> m_nodes;
	// for the purpose of this algorithm is better to have this form of adjacency list
	unordered_map<int, unordered_map<int, int>> m_adjacencyList;
	// MST
	vector<tuple<int, int, int>> m_result;
};