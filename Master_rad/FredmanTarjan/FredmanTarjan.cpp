#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

typedef pair<int, int> branch;

class FredmanTarjan {
public:
	FredmanTarjan(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
 	{
		//findMST();
	}

private:
	

private:
	vector<vector<pair<int, int>>>& m_adjacencyList; 
};