#include "Utils/UnionFind.h"

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
		, m_unionFind(adjacencyList.size())
	{ 

		findMST();
		printMST();
	}

private:
	void findMST() {
		
		int numOfComponents = m_nodesNumber;

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

					int fx = m_unionFind.find(x);
					int fy = m_unionFind.find(y);

					if (fx != fy) {
						m_unionFind.unionOperation(fx, fy);
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
			cout << "(" << it->second.first << ", " << it->second.second << ") weight: " << it->first << endl;
		}
	}

private:
	int m_nodesNumber{ 0 };

	vector<pair<int, branch>> m_MST;
	vector<vector<pair<int, int>>>& m_adjacencyList;

	UnionFind m_unionFind;
};