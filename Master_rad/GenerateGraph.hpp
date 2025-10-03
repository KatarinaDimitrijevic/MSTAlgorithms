#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

#include <unordered_set>
#include <random>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cmath>
#include <iterator> 
#include <algorithm>

// code used for generating connected undirected graphs
 
namespace mstgen {

    using BGLGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::undirectedS,
        boost::no_property,
        boost::property<boost::edge_weight_t, int>
    >;

    inline std::size_t complete_edges(std::size_t n) {
        return (n > 1) ? (n * (n - 1)) / 2 : 0;
    }

    inline double density(const BGLGraph& g) {
        auto [vi, ve] = boost::vertices(g);
        const double n = static_cast<double>(std::distance(vi, ve));
        auto [ei, ee] = boost::edges(g);
        const double m = static_cast<double>(std::distance(ei, ee));
        return (n <= 1.0) ? 0.0 : (2.0 * m) / (n * (n - 1.0));
    }

    inline bool is_connected(const BGLGraph& g) {
        auto [vi, ve] = boost::vertices(g);
        const std::size_t n = static_cast<std::size_t>(std::distance(vi, ve));
        std::vector<int> comp(n);
        int cc = boost::connected_components(g, comp.data());
        return cc == 1;
    }

    inline BGLGraph make_connected_fixed_density_graph(std::size_t n, double d, std::uint32_t seed = 123) {
        if (n == 0) return BGLGraph{};
        if (!(0.0 <= d && d <= 1.0)) throw std::runtime_error("density d not in [0,1]");

        const std::size_t maxM = complete_edges(n);
        std::size_t m_target = static_cast<std::size_t>(std::llround(d * static_cast<long double>(maxM)));
        if (m_target < n - 1) m_target = n - 1;
        if (m_target > maxM)  m_target = maxM;

        BGLGraph g(n);
        auto wmap = boost::get(boost::edge_weight, g);

        // RNG
        std::mt19937 gen_struct(seed);
        std::mt19937 gen_w(seed + 7);
        std::uniform_int_distribution<int> wdist(1, 1000);

        using VDesc = boost::graph_traits<BGLGraph>::vertex_descriptor;

        for (std::size_t i = 1; i < n; ++i) {
            std::uniform_int_distribution<std::size_t> pick(0, i - 1);
            std::size_t j = pick(gen_struct);
            auto [e, ok] = boost::add_edge(static_cast<VDesc>(i), static_cast<VDesc>(j), g);
            (void)ok;
            wmap[e] = wdist(gen_w);
        }

        auto key_of = [](std::size_t u, std::size_t v) -> std::uint64_t {
            if (u > v) std::swap(u, v);
            return (static_cast<std::uint64_t>(u) << 32) ^ static_cast<std::uint64_t>(v);
            };

        std::unordered_set<std::uint64_t> used;
        used.reserve(m_target * 2 + 16);
        for (auto [ei, ee] = boost::edges(g); ei != ee; ++ei) {
            auto u = static_cast<std::size_t>(boost::source(*ei, g));
            auto v = static_cast<std::size_t>(boost::target(*ei, g));
            used.insert(key_of(u, v));
        }

        std::uniform_int_distribution<std::size_t> pickV(0, n - 1);
        while (used.size() < m_target) {
            std::size_t u = pickV(gen_struct), v = pickV(gen_struct);
            if (u == v) continue;
            auto key = key_of(u, v);
            if (used.find(key) != used.end()) continue;

            auto [e, ok] = boost::add_edge(static_cast<VDesc>(u), static_cast<VDesc>(v), g);
            if (ok) {
                used.insert(key);
                wmap[e] = wdist(gen_w);
            }
        }
        return g;
    }

    inline BGLGraph make_connected_by_avg_degree(std::size_t n, double avg_deg, std::uint32_t seed = 123) {
        double d = (n > 1) ? std::clamp(avg_deg / static_cast<double>(n - 1), 0.0, 1.0) : 0.0;
        return make_connected_fixed_density_graph(n, d, seed);
    }

    struct AdjPair { int v; int w; };
    using AdjList = std::vector<std::vector<pair<int, int>>>;

    inline AdjList to_adjacency_list(const BGLGraph& g) {
        auto [vi, ve] = boost::vertices(g);
        const std::size_t n = static_cast<std::size_t>(std::distance(vi, ve));
        AdjList adj(n);

        auto weight = boost::get(boost::edge_weight, g);
        for (auto [ei, ee] = boost::edges(g); ei != ee; ++ei) {
            int u = static_cast<int>(boost::source(*ei, g));
            int v = static_cast<int>(boost::target(*ei, g));
            int w = weight[*ei];
            adj[u].push_back({ v, w });
            adj[v].push_back({ u, w });
        }
        return adj;
    }

    struct EdgeRec { int u, v, w; };

    inline std::vector<EdgeRec> to_edge_list(const BGLGraph& g) {
        std::vector<EdgeRec> E;
        auto [ei, ee] = boost::edges(g);
        E.reserve(static_cast<std::size_t>(std::distance(ei, ee)));
        auto weight = boost::get(boost::edge_weight, g);
        for (; ei != ee; ++ei) {
            E.push_back({
                static_cast<int>(boost::source(*ei, g)),
                static_cast<int>(boost::target(*ei, g)),
                weight[*ei]
                });
        }
        return E;
    }

} // namespace mstgen
