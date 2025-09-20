#pragma once
#include "Tree.h"

#include "Graph.h"


// create the tree (active path) using two operations
class TreeBuilder
{
public:
    TreeBuilder(Graph& graph, size_t t, size_t maxHeight, size_t initialVertex);

    Tree& getTree() { return m_tree; }
private:
    // two main operations for the purpose of creation the tree
    bool retraction();
    bool extension();

    void createClustersAndPushCheapest(list<EdgePtrWrapper>&& items);
    SoftHeapDecorator* findHeapWithExtensionEdge();

    SoftHeapDecorator::ExtractedItems fusion(EdgePtrWrapper& edge);
    // disable all corrupted edges and group them using common outer node
    void postRetractionActions(SoftHeapDecorator::ExtractedItems items);
private:
    Graph& m_graph;
    Tree m_tree;
};
