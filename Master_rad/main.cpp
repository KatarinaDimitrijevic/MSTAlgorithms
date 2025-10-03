#include "Prim.cpp"
#include "Kruskal.cpp"
#include "Boruvka.cpp"
#include "ReverseDelete.cpp"
#include "Karger/Karger.cpp"
#include "FredmanTarjan/FredmanTarjan.cpp"
#include "Chazelle/Chazelle.cpp"

#include "GenerateGraph.hpp"

#include <numeric>
#include <iostream>
#include <fstream>


vector<uint32_t> createSeed(int iterationNumber) {

    std::mt19937 rng(123456);
    std::uniform_int_distribution<uint32_t> U;

    vector<uint32_t> seed;
    
    for (size_t i = 0; i < iterationNumber; ++i) {
            seed.push_back(U(rng));
    }

    return seed;
}


mstgen::AdjList generateGraph(size_t n, uint32_t seed) {

    //double d = std::min(1.0, 3.0 / double(n));

    double k = 4.0;
    double avg_deg = k * std::log((double)n);
    double d = 0.1; /*(n > 1) ? std::min(1.0, avg_deg / (n - 1)) : 0.0;*/

    mstgen::BGLGraph g = mstgen::make_connected_fixed_density_graph(n, d, seed);

    auto [vi, ve] = boost::vertices(g);
    auto [ei, ee] = boost::edges(g);
    std::size_t V = static_cast<std::size_t>(std::distance(vi, ve));
    std::size_t E = static_cast<std::size_t>(std::distance(ei, ee));

    // debug info
    std::cout << "V=" << V << "  E=" << E << "  density=" << mstgen::density(g) << "  connected=" << (mstgen::is_connected(g) ? 1 : 0) << endl;

    /*for (int i = 0; i < adjacencyList.size(); i++) {
        for (int j = 0; j < adjacencyList[i].size(); j++) {
            cout << "(" << i << ", " << adjacencyList[i][j].first << ") weight: " << adjacencyList[i][j].second << endl;
        }
    }*/

    return mstgen::to_adjacency_list(g);
}

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
    

    const int from = 100, to = 10000, step = 100;
    const int count = (to - from) / step + 1;

    std::vector<int> inputSize(count);
    for (int i = 0; i < count; ++i) {
        inputSize[i] = i * step + from;
    }

    // number of iterations for each input size
    int iterationNumber = 100;

    vector<uint32_t> seed =  createSeed(iterationNumber);

    std::ofstream csv("benchDense.csv");
    csv << "algo;n;ms\n"; // header

    for (auto n : inputSize) {
        //cout << "Current n: " << n << endl;

        double averageDuration = 0;
        if (n > 5000) break;

        for (int iter = 0; iter < iterationNumber; ++iter) {
            auto adjacencyList = generateGraph(n, seed[iter]);
            PrimsAlgorithm prim(adjacencyList);
            //std::cout << "** " << prim.getDuration() << " ** " << endl;
            averageDuration += prim.getDuration();
        }
     
        averageDuration = averageDuration / (double)iterationNumber;

        csv << "Prim" << ';' << n << ';' << averageDuration << '\n';
    }

    for (auto n : inputSize) {
        //cout << "Current n: " << n << endl;

        double averageDuration = 0;
        if (n > 5000) break;

        for (int iter = 0; iter < iterationNumber; ++iter) {
            auto adjacencyList = generateGraph(n, seed[iter]);
            KruskalAlgorithm kruskal(adjacencyList);
            //std::cout << "** " << kruskal.getDuration() << " ** " << endl;
            averageDuration += kruskal.getDuration();
        }

        averageDuration = averageDuration / (double)iterationNumber;

        csv << "Kruskal" << ';' << n << ';' << averageDuration << '\n';
    }

    for (auto n : inputSize) {
        //cout << "Current n: " << n << endl;

        double averageDuration = 0;
        if (n > 5000) break;

        for (int iter = 0; iter < iterationNumber; ++iter) {
            auto adjacencyList = generateGraph(n, seed[iter]);
            BoruvkaAlgorithm boruvka(adjacencyList);
            //std::cout << "** " << boruvka.getDuration() << " ** " << endl;
            averageDuration += boruvka.getDuration();
        }

        averageDuration = averageDuration / (double)iterationNumber;

        csv << "Boruvka" << ';' << n << ';' << averageDuration << '\n';
    }

    for (auto n : inputSize) {
        //cout << "Current n: " << n << endl;

        double averageDuration = 0;
        if (n > 500) break;

        for (int iter = 0; iter < iterationNumber; ++iter) {
            auto adjacencyList = generateGraph(n, seed[iter]);
            ReverseDeleteAlgorithm reverseDelete(adjacencyList);
            //std::cout << "** " << boruvka.getDuration() << " ** " << endl;
            averageDuration += reverseDelete.getDuration();
        }

        averageDuration = averageDuration / (double)iterationNumber;

        csv << "ReverseDelete" << ';' << n << ';' << averageDuration << '\n';
    }

    for (auto n : inputSize) {
        //cout << "Current n: " << n << endl;

        double averageDuration = 0;
        if (n > 5000) break;

        for (int iter = 0; iter < iterationNumber; ++iter) {
            auto adjacencyList = generateGraph(n, seed[iter]);
            KargerAlgorithm karger(adjacencyList);
            //std::cout << "** " << karger.getDuration() << " ** " << endl;
            averageDuration += karger.getDuration();
        }

        averageDuration = averageDuration / (double)iterationNumber;

        csv << "Karger" << ';' << n << ';' << averageDuration << '\n';
    }

    for (auto n : inputSize) {
        //cout << "Current n: " << n << endl;

        double averageDuration = 0;
        if (n > 5000) break;

        for (int iter = 0; iter < iterationNumber; ++iter) {
            auto adjacencyList = generateGraph(n, seed[iter]);
            FredmanTarjan fredmanTarjan(adjacencyList);
            //std::cout << "** " << fredmanTarjan.getDuration() << " ** " << endl;
            averageDuration += fredmanTarjan.getDuration();
        }

        averageDuration = averageDuration / (double)iterationNumber;

        csv << "FredmanTarjan" << ';' << n << ';' << averageDuration << '\n';
    }

    for (auto n : inputSize) {
        //cout << "Current n: " << n << endl;

        double averageDuration = 0;
        if (n > 5000) break;

        for (int iter = 0; iter < iterationNumber; ++iter) {
            auto adjacencyList = generateGraph(n, seed[iter]);
            Chazelle chazelle(adjacencyList);
            //std::cout << "** " << chazelle.getDuration() << " ** " << endl;
            averageDuration += chazelle.getDuration();
        }

        averageDuration = averageDuration / (double)iterationNumber;

        csv << "Chazelle" << ';' << n << ';' << averageDuration << '\n';
    }

    /*PrimsAlgorithm prim(adjacencyList);
    KruskalAlgorithm kruskal(adjacencyList);
    BoruvkaAlgorithm boruvka(adjacencyList);
    ReverseDeleteAlgorithm deleteReverse(adjacencyList);
    KargerAlgorithm karger(adjacencyList);
    FredmanTarjan fredmanTarjan(adjacencyList);
    Chazelle chazelle(adjacencyList);*/

	return 0;
}