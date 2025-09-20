#include "Utils/UnionFind.h"

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

typedef pair<int, int> branch;

class KruskalAlgorithm {
public:
	KruskalAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
		, m_unionFind(adjacencyList.size())
 	{
		findMST();
		printMST();
	}

private:
	void sortBranches(vector<pair<int, branch>>& branches) {

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
		sort(branches.begin(), branches.end());
	}

	void findMST() {
		int numberOfIncludedEdges = 0;

		vector<pair<int, pair<int, int>>> branches;
		sortBranches(branches);

		// iteration through the branches
		for (auto it = branches.begin(); it != branches.end(); it++) {

			if (numberOfIncludedEdges == m_nodesNumber - 1) break;

			int weight = it->first;
			int x = it->second.first;
			int y = it->second.second;

			int fx = m_unionFind.find(x);
			int fy = m_unionFind.find(y);

			// only if they are in different forests
			if (fx != fy) {
				m_unionFind.unionOperation(fx, fy);

				m_MST.push_back({ weight, { x, y } });
				numberOfIncludedEdges++;
			}
		}
	}

	void printMST() {

		for (auto it = m_MST.begin(); it != m_MST.end(); it++) {
			cout << "(" << it->second.first << ", " << it->second.second << ") weight: " << it->first << endl;
		}
	}

private:
	int m_nodesNumber{0};

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList;

	UnionFind m_unionFind;
};