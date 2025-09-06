#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

class UnionFind {
public:
	UnionFind(int nodesNumber)
		: m_nodesNumber(nodesNumber)
		, m_componentNumber(nodesNumber)
		, m_parent(nodesNumber, -1)
		, m_rang(nodesNumber, 0)
	{
		// initially, each vertex is separate component and its own parent 
		for (int i = 0; i < m_nodesNumber; i++) {
			m_parent[i] = i;
			m_rang[i] = 0;
		}
	}

	int find(int x) {
		int root = x;
		while (root != m_parent[root]) {
			root = m_parent[root];
		}
		while (x != root) {
			int tmp = m_parent[x];
			m_parent[x] = root;
			x = tmp;
		}

		return root;
	}

	// union operation, it merges two components
	void unionOperation(int x, int y) {
		int fx = find(x);
		int fy = find(y);

		if (fx == fy) return;

		// checks rang of both components
		if (m_rang[fx] < m_rang[fy]) {
			m_parent[fx] = fy;
		}
		else if (m_rang[fy] < m_rang[fx]) {
			m_parent[fy] = fx;
		}
		else {
			m_parent[fx] = fy;
			m_rang[fy]++;
		}

		// important to keep track of number of components (until only one remains)
		m_componentNumber--;
	}

	// checks if x and y belong to the same component 
	bool connected(int x, int y) {
		return find(x) == find(y);
	}

	int getComponentNumber() { return m_componentNumber; }

private:
	int m_nodesNumber{0};
	int m_componentNumber{0};

	vector<int> m_parent;
	vector<int> m_rang;
};