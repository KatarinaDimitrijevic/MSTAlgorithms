#pragma once
#include <cstdint>
#include <map>

#include <optional>
#include <stdexcept>

class Graph;

namespace MST
{
    // Ackermann function implemented recursively using memoization technique
    static uint32_t Ackermann(uint32_t i, uint32_t j)
    {
        static std::map<uint32_t, std::map<uint32_t, std::optional<uint32_t>>> s_result{};
        auto& result = s_result[i][j];
        if (result.has_value())
            return result.value();

        if (i == 0)
            result = 2 * j;
        else if (j == 0)
            result = 0;
        else if (j == 1)
            result = 2;
        else
            result = Ackermann(i - 1, Ackermann(i, j - 1));

        return result.value();
    }


    static uint32_t S(uint32_t i, uint32_t j)
    {
        return Ackermann(i - 1, j);
        static std::map<uint32_t, std::map<uint32_t, std::optional<uint32_t>>> s_result{};

        auto& result = s_result[i][j];
        if (i == 0 || j == 0)
            throw std::out_of_range{ "i and j must be > 0" };

        if (i == 1)
            result = Ackermann(i - 1, j);
        else if (j == 1)
            result = Ackermann(i - 1, j);
        else
            result = S(i, j - 1) * S(i - 1, S(i, j - 1));

        return result.value();
    }

    // limit for boruvka steps in each Chazelle phase
	constexpr unsigned c = 4;

	uint32_t findMaxHeight(Graph& graph, uint32_t c);
	uint32_t findParamT(const Graph& graph, uint32_t d);
	uint32_t calculateTargetSize(uint32_t t, uint32_t nodeHeight);
}