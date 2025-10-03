#include <vector>
#include <queue>
#include <iostream>

#include <chrono>

using namespace std;

typedef pair<int, int> distanceToTheNode;

using Clock = std::chrono::steady_clock;
using ms = std::chrono::duration<double, std::milli>;


class PrimsAlgorithm {
public:
	PrimsAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
		, m_shortestBranch(adjacencyList.size(), numeric_limits<int>::max())
		, m_parent(adjacencyList.size(), -1)
		, m_visited(adjacencyList.size(), false)
	{
		auto start = Clock::now();
		initialize();
		findMST();
		auto end = Clock::now();

		m_duration = std::chrono::duration_cast<ms>(end - start).count();

		printMST();
		cout << "Prim: " << m_mstWeight << endl;
	}

	void initialize(){

		int minBranch = numeric_limits<int>::max();
		m_minBeg = -1;
		int minEnd = -1;

		// find the shortest distance in the initial graph
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
			// current shortest distance is on the top of the priority queue
			distanceToTheNode closestDistance = m_distances.top();
			m_distances.pop();

			int node = closestDistance.second;

			// do not process the node if it's already visited
			if (m_visited[node]) {
				i--;
				continue;
			}
			m_visited[node] = true;

			m_shortestBranch[node] = closestDistance.first;

			// go through all the current node's neighbors that are not already been processed and shorten the path to them if it's possible
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
				// std::cout << "(" << m_parent[i] << "," << i << ") distance: " << m_shortestBranch[i] << std::endl;
				m_mstWeight += m_shortestBranch[i];
			}
		}
	}

	size_t getMSTWeight() { return m_mstWeight; }

	double getDuration() { return m_duration; }

private:
	int m_nodesNumber{0};
	int m_minBeg{-1};

	size_t m_mstWeight{ 0 };
	double m_duration{ 0 };

	vector<int> m_shortestBranch;
	// parent vector has been used to reconstruct MST tree
	vector<int> m_parent;
	vector<bool> m_visited;

	vector<vector<pair<int, int>>>& m_adjacencyList;

	priority_queue<distanceToTheNode, vector<distanceToTheNode>, greater<distanceToTheNode>> m_distances;
};