#include <iostream>

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <algorithm>

#include <functional>
#include <utility>

using namespace std;

typedef pair<int, int> branch;

template <typename T> inline void hash_combine(std::size_t& seed, const T& value)
{
	seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T> inline void hash_value(std::size_t& seed, const T& value)
{
	hash_combine(seed, value);
}
template <typename T, typename... Types>
inline void hash_value(std::size_t& seed, const T& value, const Types&... args)
{
	hash_combine(seed, value);
	hash_value(seed, args...);
}

template <typename... Types> inline std::size_t hash_value(const Types&... args)
{
	std::size_t seed = 0;
	hash_value(seed, args...);
	return seed;
}

struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& p) const
	{
		return hash_value(p.first, p.second);
	}
};

class KargerAlgorithm {
public:
	KargerAlgorithm(vector<vector<pair<int, int>>>& adjacencyList)
		: m_adjacencyList(adjacencyList)
	{
		// ovde treba da transformisem ulaz
	}

	struct Graph {		
		Graph(int n = 0, vector<tuple<int, int, int, int>> transformedEdges = vector<tuple<int, int, int, int>>())
			: nodesNumber(n), edges(transformedEdges) {}

		int nodesNumber;
		vector< tuple<int, int, int, int> > edges;
	};	


	pair<unordered_set<int>, Graph> boruvkaStep(Graph graph) {
		
		int nodesNumber = graph.nodesNumber;

		// this vector will store index of the cheapest incident edges for each vertex
		vector<int> cheapestIncidentEdge(nodesNumber, -1);
		int edgeNumber = graph.edges.size();

		// find the cheapest incident edge for each vertex
		// go through all edges using their indices
		for (int i = 0; i < edgeNumber; ++i) {
			int from, to, weight, index;
			tie(from, to, weight, index) = graph.edges[i];

			if (cheapestIncidentEdge[from] == -1 || weight < get<2>(graph.edges[cheapestIncidentEdge[from]])){
				cheapestIncidentEdge[from] = i;
			}
			if (cheapestIncidentEdge[to] == -1 || weight < get<2>(graph.edges[cheapestIncidentEdge[to]])){
				cheapestIncidentEdge[to] = i;
			}
		}

		// mark indices of selected edges
		vector<bool> selected(edgeNumber, false);
		unordered_set<int> selectedEdges;

		// graph represented by adjacency list that contains vertices and its cheapest incident edges
		vector<vector<int>> cheapestAdjacencyEdgeGraph(nodesNumber);

		// go through all nodes
		for (int i = 0; i < nodesNumber; ++i){
			if (cheapestIncidentEdge[i] == -1) continue;

			// select the index of the edge that is chosen as the cheapest for current node
			selected[cheapestIncidentEdge[i]] = true;

			int from, to, weight, index;
			tie(from, to, weight, index) = graph.edges[cheapestIncidentEdge[i]];

			cheapestAdjacencyEdgeGraph[from].push_back(to);
			cheapestAdjacencyEdgeGraph[to].push_back(from);

			selectedEdges.insert(get<3>(graph.edges[cheapestIncidentEdge[i]]));
		}

		// id for the next connected component (by selected edges), found using depth-first-search algorithm
		int nextId = 0;
		// ids of the all found components
		vector<int> newNodeIDs(nodesNumber, -1);

		function<void(int, int)> dfs = [&](int root, int parent) {
			if (parent == -1) {
				newNodeIDs[root] = nextId++;
			}
			else{
				newNodeIDs[root] = newNodeIDs[parent];
			}

			for (const auto& node : cheapestAdjacencyEdgeGraph[root]){
				if (newNodeIDs[node] == -1) {
					dfs(node, root); 
				}
			}
		};

		for (int i = 0; i < nodesNumber; ++i){
			// for each node, if component hasn't been already found - find it using DFS
			if (newNodeIDs[i] == -1) {
				dfs(i, -1);
			}
		}

		// degree of each connected component
		vector<int> newNodeDegree(nextId, 0);
		// new id - ids will be possibly changed due to some isolated nodes
		vector<int> newNodeNewIDs(nextId, 0);

		for (int i = 0; i < edgeNumber; ++i){
			int from, to, weight, index;
			tie(from, to, weight, index) = graph.edges[i];

			// if TO and FROM are not in the same component
			if (newNodeIDs[from] != newNodeIDs[to]){
				newNodeDegree[newNodeIDs[from]]++;
				newNodeDegree[newNodeIDs[to]]++;
			}
		}

		// process only components that are not isolated

		int nextNonIsolatedId = 0;
		// process all connected components
		for (int i = 0; i < nextId; ++i){
			// ignore in case it's isolated
			if (newNodeDegree[i] > 0){
				newNodeNewIDs[i] = nextNonIsolatedId++;
			}
		}

		// only edges between different components 
		vector<tuple<int, int, int, int>> remainingGraphEdges;
		// for the purpose of removing multiple edges between components
		unordered_map<pair<int, int>, int, pair_hash> minimumEdges;


		for (int i = 0; i < edgeNumber; ++i){
			int from, to, weight, index;
			tie(from, to, weight, index) = graph.edges[i];

			// do not process if current edge is already selected
			if (selectedEdges.find(index) != selectedEdges.end()) continue;

			// process remaining edges whose endpoints are in different components
			if (newNodeIDs[from] != newNodeIDs[to]){
				int u = newNodeNewIDs[newNodeIDs[from]];
				int v = newNodeNewIDs[newNodeIDs[to]];

				// (u, v) is the same edge as (v, n), do not process it twice
				if (u > v) swap(u, v);

				// add index of the current edge if does not exist edge between u and v components
				if (minimumEdges.count(make_pair(u, v)) == 0){
					minimumEdges[make_pair(u, v)] = i;
				}
				// take the index of minimum one if a connection already exists
				else{
					int currentWeight = get<2>(graph.edges[minimumEdges[make_pair(u, v)]]);
					if (weight < currentWeight) { 
						minimumEdges[make_pair(u, v)] = i; 
					}
				}
			}
		}

		// edgeInfo -> (endpoints in contracted graph, original edge index)
		for (const auto& edgeInfo : minimumEdges) {
			int from, to, weight, index;
			tie(from, to, weight, index) = graph.edges[edgeInfo.second];

			remainingGraphEdges.emplace_back(edgeInfo.first.first, edgeInfo.first.second, weight, index);
		}

		// remaining graph
		Graph remainingGraph = Graph(nextNonIsolatedId, remainingGraphEdges);

		return make_pair(selectedEdges, remainingGraph);
	}

	Graph removeIsolatedNodes(const Graph& graph) {
		int nodesNumber = graph.nodesNumber;

		vector<int> componentDegree(nodesNumber, 0);
		vector<vector<int>> adjacencyListOfNonIsolatedComponents(nodesNumber);

		// create adjacencyList
		for (const auto& edge : graph.edges){
			int from, to, weight, index;
			tie(from, to, weight, index) = edge;

			componentDegree[from]++;
			componentDegree[to]++;
			adjacencyListOfNonIsolatedComponents[from].push_back(to);
			adjacencyListOfNonIsolatedComponents[to].push_back(from);
		}

		// check all degrees and if any equals zero - remove it, otherwise stop here
		int minDegree = *(min_element(componentDegree.begin(), componentDegree.end()));
		if (minDegree > 0) return graph;

		int nextID = 0;
		vector<int> newNodeIDs(nodesNumber, -1);

		for (int i = 0; i < nodesNumber; ++i){
			// calculate new ID if node is not isolated
			if (componentDegree[i] > 0) { 
				newNodeIDs[i] = nextID++; 
			}
		}

		// create new Graph without isolated nodes - possibly modified indices
		vector<tuple<int, int, int, int>> newEdges;
		for (const auto& edge : graph.edges)
		{
			int from, to, weight, index;
			tie(from, to, weight, index) = edge;

			newEdges.emplace_back(newNodeIDs[from], newNodeIDs[to], weight, index);
		}

		return Graph(nextID, newEdges);
	}


	Graph randomSampling(Graph& graph, unsigned int seed = 0)
	{
		srand(seed);
		vector<tuple<int, int, int, int>> randomSelectedEdges;

		for (const auto& edge : graph.edges)
		{
			int b = (rand() & 1);
			// current edge is selected with probability of 1/2 if condition is satisfied
			if (b) {
				randomSelectedEdges.push_back(edge);
			}
		}

		Graph graphOfRandomSelectedEdges = Graph(graph.nodesNumber, randomSelectedEdges);

		return removeIsolatedNodes(graphOfRandomSelectedEdges);
	}

	// find minimum spanning tree via Karger's algorithm
	unordered_set<int> findMST(Graph& graph, unsigned int seed) {
		unordered_set<int> MSTEdges;
		
		// return in case only one component remains

		if (graph.edges.empty()) { 
			return MSTEdges; 
		}

		// Boruvka steps (apply twice):

		auto firstBoruvkaStep = boruvkaStep(Graph(graph.nodesNumber, graph.edges));
		for (const auto& edge : firstBoruvkaStep.first){
			MSTEdges.insert(edge);
		}
		Graph& reducedGraphAfterFirstBoruvka = firstBoruvkaStep.second;

		// finish here if reduced graph is empty after the first phase
		if (reducedGraphAfterFirstBoruvka.edges.empty() || reducedGraphAfterFirstBoruvka.nodesNumber == 0)
		{
			return MSTEdges;
		}

		auto secondBoruvkaStep = boruvkaStep(Graph(reducedGraphAfterFirstBoruvka.nodesNumber, reducedGraphAfterFirstBoruvka.edges));
		for (const auto& edge : secondBoruvkaStep.first) {
			MSTEdges.insert(edge);
		}
		Graph& reducedGraphAfterSecondBoruvka = secondBoruvkaStep.second;

		if (reducedGraphAfterSecondBoruvka.nodesNumber == 0 || reducedGraphAfterSecondBoruvka.edges.empty()){
			return MSTEdges;
		}

		// this one will be used in next steps - randomSampling steps
		Graph originalGraph = reducedGraphAfterSecondBoruvka;

		// return graph created of random selected edges from the original one with probability of 0.5
		Graph randomSamplingGraph = randomSampling(originalGraph);

		// edge sets created in these phases are selected as F1 and F2 in order to be consistent with documentation 
		// apply this algorithm recursively to the randomSamplingGraph 
		unordered_set<int> kargerF1 = findMST(randomSamplingGraph, seed);

		// edges that belong to final MST, but original indices
		vector<tuple<int, int, int, int>> MSTFromF1;
		for (const auto& edge : originalGraph.edges){
			if (kargerF1.find(get<3>(edge)) != kargerF1.end()) {
				MSTFromF1.push_back(edge);
			}
		}

		// the edges from originalGraph that are MSTF1-heavy.
		unordered_set<int> heavyEdges = {}; // TODO

		// take just non-heavy edges from originalGraph
		vector<tuple<int, int, int, int>> relevantEdges;
		for (const auto& edge : originalGraph.edges){
			if (heavyEdges.find(get<3>(edge)) == heavyEdges.end()){
				relevantEdges.emplace_back(edge);
			}
		}

		Graph relevanEdgesGraph = Graph(originalGraph.nodesNumber, relevantEdges);
		relevanEdgesGraph = removeIsolatedNodes(relevanEdgesGraph);

		// apply this algorithm recursively to the relevanEdgesGraph 
		unordered_set<int> result = findMST(relevanEdgesGraph, seed);
		for (const auto& edge : result) {
			MSTEdges.insert(edge);
		}

		return MSTEdges;
	}


private:
	vector<vector<pair<int, int>>>& m_adjacencyList;
};