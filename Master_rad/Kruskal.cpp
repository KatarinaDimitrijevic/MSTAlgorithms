#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

typedef pair<int, int> branch;

namespace {
	void unionFindInitialization(vector<int>& parent, vector<int>& rang, int n) {

		for (int i = 0; i < n; i++) {
			parent[i] = i;
			rang[i] = 0;
		}
	}

	int find(int x, vector<int>& parent) {
		int root = x;
		while (root != parent[root]) {
			root = parent[root];
		}
		while (x != root) {
			int tmp = parent[x];
			parent[x] = root;
			x = tmp;
		}

		return root;
	}

	void unionOperation(int x, int y, vector<int>& parent, vector<int>& rang) {
		int fx = find(x, parent);
		int fy = find(y, parent);

		if (rang[fx] < rang[fy]) {
			parent[fx] = fy;
		}
		else if (rang[fy] < rang[fx]) {
			parent[fy] = fx;
		}
		else {
			parent[fx] = fy;
			rang[fy]++;
		}
	}
}

class KruskalAlgorithm {
public:
	KruskalAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
		, m_parent(adjacencyList.size(), -1)
		, m_rang(adjacencyList.size(), 0)
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

		unionFindInitialization(m_parent, m_rang, m_nodesNumber);

		// iteration through the branches
		for (auto it = branches.begin(); it != branches.end(); it++) {

			if (numberOfIncludedEdges == m_nodesNumber - 1) break;

			int weight = it->first;
			int x = it->second.first;
			int y = it->second.second;

			int fx = find(x, m_parent);
			int fy = find(y, m_parent);

			// only if they are in different forests
			if (fx != fy) {
				unionOperation(fx, fy, m_parent, m_rang);

				m_MST.push_back({ weight, { x, y } });
				numberOfIncludedEdges++;
			}
		}
	}

	void printMST() {

		for (auto it = m_MST.begin(); it != m_MST.end(); it++) {
			cout << "(" << it->second.first << ", " << it->second.second << ") cene: " << it->first << endl;
		}
	}

private:
	int m_nodesNumber{0};

	vector<int> m_parent;
	vector<int> m_rang;

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList; 
};