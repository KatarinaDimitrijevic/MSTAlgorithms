#include "Utils/UnionFind.h"

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

#include <chrono>

using namespace std;

using Clock = std::chrono::steady_clock;
using ms = std::chrono::duration<double, std::milli>;

typedef pair<int, int> branch;

class BoruvkaAlgorithm {
public:
	BoruvkaAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
		, m_nodesNumber(adjacencyList.size())
		, m_unionFind(adjacencyList.size())
	{ 
		auto start = Clock::now();
		findMST();
		auto end = Clock::now();

		m_duration = std::chrono::duration_cast<ms>(end - start).count();
		
		printMST();
		cout << "Boruvka: " << m_mstWeight << endl;
	}

	size_t getMSTWeight() { return m_mstWeight; }

	double getDuration() { return m_duration; }

private:
	void findMST() {
		
		int numOfComponents = m_nodesNumber;

		// the cheapest branch to each component
		vector<vector<int>> cheapestBranch(m_nodesNumber, vector<int>(3, -1));

		// until there is only one component - MST
		while (numOfComponents > 1) {

			for(int i = 0; i < m_adjacencyList.size(); i++){
				for (int j = 0; j < m_adjacencyList[i].size(); j++) {
					int x = i;
					int y = m_adjacencyList[i][j].first;

					int fx = m_unionFind.find(x);
					int fy = m_unionFind.find(y);

					int weight = m_adjacencyList[i][j].second;

					// if current edge (x, y) does not belong to one connected component, use it as the next cheapest edge that expands the component
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

			// connect components
			for (int i = 0; i < m_nodesNumber; i++) {
				if(cheapestBranch[i][2] != -1) {
					int x = cheapestBranch[i][0];
					int y = cheapestBranch[i][1];
					int weight = cheapestBranch[i][2];

					int fx = m_unionFind.find(x);
					int fy = m_unionFind.find(y);

					if (fx != fy) {
						m_unionFind.unionOperation(fx, fy);
						m_MST.push_back({ weight, {x, y} });

						numOfComponents--;
					}
				}
			}

			// prepare for the next iteration
			for (int i = 0; i < m_nodesNumber; i++) {
				cheapestBranch[i][2] = -1;
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
	int m_nodesNumber{ 0 };

	size_t m_mstWeight{ 0 };
	double m_duration{ 0 };

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList;

	UnionFind m_unionFind;
};