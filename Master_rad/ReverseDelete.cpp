#include <vector>
#include <queue>
#include <iostream>

#include <algorithm>

#include <chrono>

using namespace std;

typedef pair<int, int> branch;

using Clock = std::chrono::steady_clock;
using ms = std::chrono::duration<double, std::milli>;

class ReverseDeleteAlgorithm {
public:
	ReverseDeleteAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
	{

		auto start = Clock::now();
		findMST();
		auto end = Clock::now();

		m_duration = std::chrono::duration_cast<ms>(end - start).count();
		
		printMST();
		cout << "ReverseDelete: " << m_mstWeight << endl;
	}

	size_t getMSTWeight() { return m_mstWeight; }

	double getDuration() { return m_duration; }

private:
	// Depth First Search algorithm
	void dfs(int node, vector<bool>& visited) {

		visited[node] = true;

		for (auto n : m_adjacencyList[node]) {
			if (!visited[n.first]) {
				dfs(n.first, visited);
			}
		}
	}

	void dfsIterative(int node, std::vector<bool>& visited) {

		std::vector<int> stack;

		stack.push_back(node);
		visited[node] = true;

		while (!stack.empty()) {
			int u = stack.back();
			stack.pop_back();

			for (const auto& neighbor : m_adjacencyList[u]) {
				int v = neighbor.first;
				if (!visited[v]) {
					visited[v] = 1;
					stack.push_back(v);
				}
			}
		}
	}

	bool isGraphConnected() {
		// when all nodes are connected, there is only one component and graph is connected
		vector<bool> visited(m_nodesNumber, false);
		// dfs(0, visited);
		dfsIterative(0, visited);

		for (auto node : visited) {
			if (!node) {
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
		// sort edges in ascending order
		sort(branches.begin(), branches.end(),
			[](const pair<int, branch>& a, const pair<int, branch>& b) { return a.first > b.first; });
	}

	void findMST() {
		
		vector<pair<int, branch>> branches;
		sortBranches(branches);

		// takes one by one edge from sorted vector
		for (auto currentBranch : branches){

			int u = currentBranch.second.first;
			int v = currentBranch.second.second;
			int weight = currentBranch.first;

			// try to remove it
			m_adjacencyList[u].erase(remove_if(m_adjacencyList[u].begin(), m_adjacencyList[u].end(),
				[v](const branch& b) {
					return b.first == v ;
				}), m_adjacencyList[u].end());
			m_adjacencyList[v].erase(remove_if(m_adjacencyList[v].begin(), m_adjacencyList[v].end(),
				[u](const branch& b) {
					return b.first == u;
				}), m_adjacencyList[v].end());

			// if graph remains connected it's valid, if not push the branch back to adjacency list
			if (isGraphConnected() == false){
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

private:
	int m_nodesNumber{0};

	size_t m_mstWeight{ 0 };
	double m_duration{ 0 };

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList;
};