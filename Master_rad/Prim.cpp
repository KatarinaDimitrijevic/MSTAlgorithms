#include <vector>
#include <queue>
#include <iostream>

using namespace std;

typedef pair<int, int> distanceToTheNode;

class PrimsAlgorithm {
public:
	PrimsAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
		, m_shortestBranch(adjacencyList.size(), numeric_limits<int>::max())
		, m_parent(adjacencyList.size(), -1)
		, m_visited(adjacencyList.size(), false)
	{
		initialize();
		findMST();
		printMST();
	}

	void initialize(){

		int minBranch = numeric_limits<int>::max();
		m_minBeg = -1;
		int minEnd = -1;

		// find the shortest distance
		for (int i = 0; i < m_adjacencyList.size(); i++) {
			for (int j = 0; j < m_adjacencyList[i].size(); j++) {
				if (m_adjacencyList[i][j].second < minBranch) {
					minBranch = m_adjacencyList[i][j].second;
					m_minBeg = i;
					minEnd = m_adjacencyList[i][j].first;
				}
			}
		}
		// put it in the priority queue, others will be set to inf
		m_distances.push(std::make_pair(0, m_minBeg));
		m_shortestBranch[m_minBeg] = 0;
		m_parent[m_minBeg] = m_minBeg;

		for (int node = 0; node < m_nodesNumber; node++) {
			if (node != m_minBeg) {
				m_distances.push(std::make_pair(std::numeric_limits<int>::max(), node));
			}
		}
	}

	void findMST() {
		for (int i = 0; i < m_nodesNumber; i++) {
			distanceToTheNode closestDistance = m_distances.top();
			m_distances.pop();

			int node = closestDistance.second;

			if (m_visited[node]) {
				i--;
				continue;
			}
			m_visited[node] = true;

			m_shortestBranch[node] = closestDistance.first;

			for (int j = 0; j < m_adjacencyList[node].size(); j++) {
				if (!m_visited[m_adjacencyList[node][j].first]) {
					int adjacentNode = m_adjacencyList[node][j].first;
					int adjacentNodeBranch = m_adjacencyList[node][j].second;

					if (adjacentNodeBranch < m_shortestBranch[adjacentNode]) {
						m_shortestBranch[adjacentNode] = adjacentNodeBranch;
						m_parent[adjacentNode] = node;
						m_distances.push(std::make_pair(adjacentNodeBranch, adjacentNode));
					}
				}
			}
		}
	}

	void printMST(){

		for (int i = 0; i < m_nodesNumber; i++) {
			if (m_minBeg != i) {
				std::cout << "(" << m_parent[i] << "," << i << ") distance: " << m_shortestBranch[i] << std::endl;
			}
		}
	}

private:
	int m_nodesNumber{0};
	int m_minBeg{-1};

	vector<int> m_shortestBranch;
	vector<int> m_parent;
	vector<bool> m_visited;

	vector<vector<pair<int, int>>>& m_adjacencyList;

	priority_queue<distanceToTheNode, vector<distanceToTheNode>, greater<distanceToTheNode>> m_distances;

};