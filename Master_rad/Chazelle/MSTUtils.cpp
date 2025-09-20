#include "MSTUtils.h"

#include "Graph.h"

#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace MST
{
    uint32_t findMaxHeight(Graph& graph, uint32_t c)
    {
        const double verticesCount = static_cast<double>(graph.getNodesNumber());
        const double edgesCount = static_cast<double>(graph.getNodesNumber());
        return c * static_cast<uint32_t>(std::ceil(std::pow(edgesCount / verticesCount, 1.0 / 3.0)));
    }

    uint32_t findParamT(const Graph& graph, uint32_t d)
    {
        if (d == 1)
            return 1;

        const double verticesCount = static_cast<double>(graph.getNodesNumber());

        uint32_t result = 1;
        
        while (verticesCount > std::pow(S(result, d), 3)) {
            ++result;
        }

        return result;
    }

    uint32_t calculateTargetSize(uint32_t t, uint32_t nodeHeight)
    {
        if (nodeHeight == 1) {
            return std::pow(S(t, 1u), 3u);
        }

        return std::pow(S(t - 1, S(t, nodeHeight - 1)), 3u);
    }
} // namespace MST