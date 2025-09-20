#pragma once

#include "SoftHeapDecorator.h"
#include "TreeSubgraph.h"
#include "Graph.h"

#include <vector>

// This class represents the active path

class Tree
{
public:
    Tree(Graph& graph, size_t t, size_t maxHeight, size_t initialNode);

    // extends the active path adding the next component via extensionEdge
    void push(const EdgePtrWrapper& extensionEdge);

    // retract the active path (melds the last and the previous component from the active path)
    SoftHeapDecorator::ExtractedItems pop();
    SoftHeapDecorator::ExtractedItems fusion(list<SubGraphPtr>::iterator itr, const EdgePtrWrapper& fusionEdge);

    // take the current (the last one) component on the active path
    ISubGraph& top();
    size_t size() const;

    auto view()
    {
        return ranges::views::transform(m_activePath,
            [](SubGraphPtr& subgraph)-> ISubGraphPtr
            {
                return subgraph;
            });
    }

    auto view() const
    {
        return ranges::views::transform(m_activePath,
            [](const SubGraphPtr& subgraph)-> const ISubGraphPtr
            {
                return subgraph;
            });
    }

    // Divide the graph by badEdges and apply recursion to the remaining parts of the original graph
    list<Graph> createSubGraphs(const set<size_t>& badEdges);

    // get all vertices from the current tree
    list<size_t> getVerticesInside();
    const set<size_t>& getBadEdges() const { return m_badEdges; }

private:
    // add new subGraph (which starts with vertex) to the m_activePath 
    void pushNode(size_t vertex);

    // put border edges of the new component to the proper heaps
    void addNewBorderEdgesAfterPush();
    // update border/intern edges after adding a new component to the active path (m_activePath)
    void deleteOldBorderEdgesAndUpdateMinLinksAfterPush();

    size_t getMaxHeight() const { return m_sizesPerHeight.size() - 1; }
    // index: position of the component on the active path
    size_t indexToHeight(size_t index) const { return getMaxHeight() - index; }

private:
    Graph& m_graph;

    const size_t m_r;

    // all the components in the active path
    list<SubGraphPtr> m_activePath{};
    set<size_t> m_badEdges{};
    const vector<size_t> m_sizesPerHeight;
};
