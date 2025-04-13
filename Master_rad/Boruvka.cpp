#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

typedef pair<int, int> branch;

class BoruvkaAlgorithm {
public:
	BoruvkaAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
		, m_parent(adjacencyList.size(), -1)
		, m_rang(adjacencyList.size(), 0)
	{ 

		findMST();
		printMST();
	}

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

private:
	void findMST() {
		
		int numOfComponents = m_nodesNumber;
		unionFindInitialization(m_parent, m_rang, m_nodesNumber);

		vector<vector<int>> cheapestBranch(m_nodesNumber, vector<int>(3, -1));

		// until there is only one component - MST
		while (numOfComponents > 1) {

			for(int i = 0; i < m_adjacencyList.size(); i++){
				for (int j = 0; j < m_adjacencyList[i].size(); j++) {
					int x = i;
					int y = m_adjacencyList[i][j].first;
					int fx = find(x, m_parent);
					int fy = find(y, m_parent);
					int weight = m_adjacencyList[i][j].second;

					if (fx != fy) {
						if (cheapestBranch[fx][2] == -1 || cheapestBranch[fx][2] > weight) {
							cheapestBranch[fx] = {x, y, weight};
						}
						if (cheapestBranch[fy][2] == -1 || cheapestBranch[fy][2] > weight) {
							cheapestBranch[fy] = {x, y, weight};
						}
					}
				}
			}

			for (int i = 0; i < m_nodesNumber; i++) {
				if(cheapestBranch[i][2] != -1) {
					int x = cheapestBranch[i][0];
					int y = cheapestBranch[i][1];
					int weight = cheapestBranch[i][2];

					int fx = find(x, m_parent);
					int fy = find(y, m_parent);

					if (fx != fy) {
						unionOperation(fx, fy, m_parent, m_rang);
						m_MST.push_back({ weight, {x, y} });

						numOfComponents--;
					}
				}
			}

			for (int i = 0; i < m_nodesNumber; i++) {
				cheapestBranch[i][2] = -1;
			}
		}
	}

	void printMST() {

		for (auto it = m_MST.begin(); it != m_MST.end(); it++) {
			cout << "(" << it->second.first << ", " << it->second.second << ") cene: " << it->first << endl;
		}
	}

private:
	int m_nodesNumber{ 0 };

	vector<int> m_parent;
	vector<int> m_rang;

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList;
};