#include "Utils/UnionFind.h"

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

#include <chrono>

using namespace std;

typedef pair<int, int> branch;

using Clock = std::chrono::steady_clock;
using ms = std::chrono::duration<double, std::milli>;

class KruskalAlgorithm {
public:
	KruskalAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
		, m_unionFind(adjacencyList.size())
 	{
		auto start = Clock::now();
		findMST();
		auto end = Clock::now();

		m_duration = std::chrono::duration_cast<ms>(end - start).count();

		printMST();
		cout << "Kruskal: " << m_mstWeight << endl;
	}

	double getDuration() { return m_duration; }

	size_t getMSTWeight() { return m_mstWeight; }

private:
	void sortBranches(vector<pair<int, branch>>& branches) {
		// (i, j) and (j, i) represent the same edge in undirected graph
		for (int i = 0; i < m_nodesNumber; i++) {
			for (int j = 0; j < m_adjacencyList[i].size(); j++) {
				int neighbor = m_adjacencyList[i][j].first;
				int weight = m_adjacencyList[i][j].second;
				if (i < neighbor)
				{
					branches.push_back({ weight,{i, neighbor} });
				}
			}
		}
		// sort edges in descending order
		sort(branches.begin(), branches.end());
	}

	void findMST() {
		int numberOfIncludedEdges = 0;

		vector<pair<int, pair<int, int>>> branches;
		sortBranches(branches);

		// iteration through sorted vector of branches
		for (auto it = branches.begin(); it != branches.end(); it++) {

			// stop when |V|-1 edges have been added (the smallest number needed to connect all the nodes and create a MST)
			if (numberOfIncludedEdges == m_nodesNumber - 1) break;

			int weight = it->first;
			int x = it->second.first;
			int y = it->second.second;

			int fx = m_unionFind.find(x);
			int fy = m_unionFind.find(y);

			// add edge and connect components represented by fx and fy only if they are in different forests
			if (fx != fy) {
				m_unionFind.unionOperation(fx, fy);

				m_MST.push_back({ weight, { x, y } });
				numberOfIncludedEdges++;
			}
		}
	}

	void printMST() {

		for (auto it = m_MST.begin(); it != m_MST.end(); it++) {
			// cout << "(" << it->second.first << ", " << it->second.second << ") weight: " << it->first << endl;
			m_mstWeight += it->first;
		}
	}

private:
	int m_nodesNumber{0};

	size_t m_mstWeight{ 0 };
	double m_duration{ 0 };

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList;

	// used for merging different connected componnets
	UnionFind m_unionFind;
};