#include "Prim.cpp"
#include "Kruskal.cpp"
#include "Boruvka.cpp"
#include "ReverseDelete.cpp"
#include "Karger.cpp"

int main() {

	std::vector<std::vector<std::pair<int, int>>> adjacencyList{ {{1,7}, {2,6}},
		{{0,7}, {2,2}, {3,3}}, {{0,6}, {1,2}, {3,4}, {4,5}},
		{{1,3}, {2,4}, {4,1}}, {{2,5}, {3,1}} };

	//PrimsAlgorithm prim(adjacencyList);
	//KruskalAlgorithm kruskal(adjacencyList);
	//BoruvkaAlgorithm boruvka(adjacencyList);
	//DeleteReverseAlgorithm deleteReverse(adjacencyList);
	KargerAlgorithm karger(adjacencyList);

	return 0;
}