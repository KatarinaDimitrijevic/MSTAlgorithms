#pragma once

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <set>

using namespace std;

class UnionFind
{
public:
    UnionFind(int nodesNumber);

	// handle the case when graph is created using default constructor 
	void ensureSize(int n);

	// return the parent of node x
	int find(int x);
	// number of components (initially vertices)
	int getComponentNumber() const { return m_componentNumber; }

	// merge components
	void unionOperation(int x, int y);

	//check if two nodes belong to the same component
	bool connected(int x, int y);

	set<int> getComponents();

private:
    int m_nodesNumber{ 0 };
    int m_componentNumber{ 0 };

    vector<int> m_parent;
    vector<int> m_rang;
};