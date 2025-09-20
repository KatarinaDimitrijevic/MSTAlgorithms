#include "UnionFind.h"

UnionFind::UnionFind(int nodesNumber)
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

void UnionFind::ensureSize(int n)
{
	if (n <= m_nodesNumber) return;

	int oldNodesNumber = m_nodesNumber;
	m_parent.resize(n);
	m_rang.resize(n);

	for (int i = oldNodesNumber; i < n; ++i) {
		// new nodes
		m_parent[i] = i;   
		m_rang[i] = 0;
	}

	m_componentNumber += (n - oldNodesNumber);
	m_nodesNumber = n;
}

int UnionFind::find(int x) {
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
void UnionFind::unionOperation(int x, int y) {
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
bool UnionFind::connected(int x, int y) {
	return find(x) == find(y);
}

set<int> UnionFind::getComponents() {
	set<int> components; 

	for (int i = 0; i < m_parent.size(); ++i) {
		if (i == m_parent[i]) {
			components.insert(i);
		}
	}
	return components;
}

