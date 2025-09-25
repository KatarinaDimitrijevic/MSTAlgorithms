#include "Prim.cpp"
#include "Kruskal.cpp"
#include "Boruvka.cpp"
#include "ReverseDelete.cpp"
#include "Karger/Karger.cpp"
#include "FredmanTarjan/FredmanTarjan.cpp"
#include "Chazelle/Chazelle.cpp"

#include "GenerateGraph.hpp"

int main() {

	/*std::vector<std::vector<std::pair<int, int>>> adjacencyList{ {{1,7}, {2,6}},
		{{0,7}, {2,2}, {3,3}}, {{0,6}, {1,2}, {3,4}, {4,5}},
		{{1,3}, {2,4}, {4,1}}, {{2,5}, {3,1}} };*/

	//std::vector<std::vector<std::pair<int, int>>> adjacencyList{
 //       /* 0 */{ {1,1}, {15,23} },
 //       /* 1 */{ {0,1}, {2,9}, {14,31} },
 //       /* 2 */{ {3,2}, {1,9}, {5,25} },
 //       /* 3 */{ {2,2}, {4,17} },
 //       /* 4 */{ {5,3}, {3,17} },
 //       /* 5 */{ {4,3}, {6,11}, {2,25} },
 //       /* 6 */{ {7,4}, {5,11}, {9,27} },
 //       /* 7 */{ {6,4}, {8,19} },
 //       /* 8 */{ {9,5}, {7,19} },
 //       /* 9 */{ {8,5}, {10,13}, {6,27} },
 //       /* 10 */{ {11,6}, {9,13}, {13,29} },
 //       /* 11 */{ {10,6}, {12,21} },
 //       /* 12 */{ {13,7}, {11,21} },
 //       /* 13 */{ {12,7}, {14,15}, {10,29} },
 //       /* 14 */{ {15,8}, {13,15}, {1,31} },
 //       /* 15 */{ {14,8}, {0,23} }
 //   };

	//PrimsAlgorithm prim(adjacencyList);
	//KruskalAlgorithm kruskal(adjacencyList);
	//BoruvkaAlgorithm boruvka(adjacencyList);
	//ReverseDeleteAlgorithm deleteReverse(adjacencyList);
	//KargerAlgorithm karger(adjacencyList);
    //FredmanTarjan fredmanTarjan(adjacencyList);
    //Chazelle chazelle(adjacencyList);

    using namespace mstgen;

    
    using namespace mstgen;

    std::size_t n = 5;
    double d = std::min(1.0, 3.0 / double(n));
    uint32_t seed = 123;

    BGLGraph g = make_connected_fixed_density_graph(n, d, seed);

    auto [vi, ve] = boost::vertices(g);
    auto [ei, ee] = boost::edges(g);
    std::size_t V = static_cast<std::size_t>(std::distance(vi, ve));
    std::size_t E = static_cast<std::size_t>(std::distance(ei, ee));

    std::cout << "V=" << V
        << "  E=" << E
        << "  density=" << density(g)
        << "  connected=" << (is_connected(g) ? 1 : 0)
        << "\n";

   
    auto adjacencyList = to_adjacency_list(g);

    for (int i = 0; i < adjacencyList.size(); i++) {
        for (int j = 0; j < adjacencyList[i].size(); j++) {
            cout << "(" << i << ", " << adjacencyList[i][j].first << ") weight: " << adjacencyList[i][j].second << endl;
        }
    }

    PrimsAlgorithm prim(adjacencyList);
    KruskalAlgorithm kruskal(adjacencyList);
    BoruvkaAlgorithm boruvka(adjacencyList);
    ReverseDeleteAlgorithm deleteReverse(adjacencyList);
    KargerAlgorithm karger(adjacencyList);
    FredmanTarjan fredmanTarjan(adjacencyList);
    Chazelle chazelle(adjacencyList);

	return 0;
}