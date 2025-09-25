#include <vector>
#include <queue>
#include <iostream>

#include <algorithm>

using namespace std;

typedef pair<int, int> branch;

class ReverseDeleteAlgorithm {
public:
	ReverseDeleteAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
	{

		findMST();
		
		printMST();
		cout << "ReverseDelete: " << m_mstWeight << endl;
	}

private:
	void dfs(int node, vector<bool>& visited) {

		visited[node] = true;

		for (auto n : m_adjacencyList[node]) {
			if (!visited[n.first]) {
				dfs(n.first, visited);
			}
		}
	}

	bool isGraphConnected() {

		vector<bool> visited(m_nodesNumber, false);
		dfs(0, visited);

		for (int i = 0; i < m_adjacencyList.size(); i++) {
			if (!visited[i]) {
				return false;
			}
		}
		return true;
	}

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
		sort(branches.begin(), branches.end(),
			[](const pair<int, branch>& a, const pair<int, branch>& b) { return a.first > b.first; });
	}

	void findMST() {
		
		vector<pair<int, branch>> branches;
		sortBranches(branches);

		for (auto currectBranch : branches)
		{
			int u = currectBranch.second.first;
			int v = currectBranch.second.second;
			int weight = currectBranch.first;

			m_adjacencyList[u].erase(remove_if(m_adjacencyList[u].begin(), m_adjacencyList[u].end(),
				[v](const branch& b) {
					return b.first == v ;
				}), m_adjacencyList[u].end());
			m_adjacencyList[v].erase(remove_if(m_adjacencyList[v].begin(), m_adjacencyList[v].end(),
				[u](const branch& b) {
					return b.first == u;
				}), m_adjacencyList[v].end());

			if (isGraphConnected() == false)
			{
				m_adjacencyList[u].push_back({v, weight});
				m_adjacencyList[v].push_back({u, weight});

				m_MST.push_back({ weight, {u, v} });
			}
		}

	}

	void printMST() {

		for (auto it = m_MST.begin(); it != m_MST.end(); it++) {
			// cout << "(" << it->second.first << ", " << it->second.second << ") weight: " << it->first << endl;
			m_mstWeight += it->first;
		}
	}

	size_t getMSTWeight() { return m_mstWeight; }

private:
	int m_nodesNumber{0};
	size_t m_mstWeight{ 0 };

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList;
};