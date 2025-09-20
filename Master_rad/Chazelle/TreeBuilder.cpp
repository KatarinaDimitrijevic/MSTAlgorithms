#include "TreeBuilder.h"

#include "Graph.h"

TreeBuilder::TreeBuilder(Graph& graph, size_t t, size_t maxHeight, size_t initialVertex)
    : m_graph{ graph }
    , m_tree{ m_graph, t, maxHeight, initialVertex }
{
    while (true){
        if (m_tree.top().satisfiedTargetSize()){
            if (!retraction()) return;
        }
        else{
            if (!extension()) return;
        }
    }
}

bool TreeBuilder::retraction()
{
    if (m_tree.top().getLevelInTree() == 0) {
        return false;
    }

    postRetractionActions(m_tree.pop());

    return true;
}

bool TreeBuilder::extension()
{
    const auto heap = findHeapWithExtensionEdge();
    
    if (!heap) {
        return false;
    }

    auto edge = heap->deleteMin();

    postRetractionActions(fusion(edge));

    m_tree.push(edge);

    return true;
}

void TreeBuilder::createClustersAndPushCheapest(list<EdgePtrWrapper>&& items)
{
    // cluster based on outer vertex
    map<size_t, set<EdgePtrWrapper>> clusters{};

    std::for_each(std::make_move_iterator(items.begin()),
        std::make_move_iterator(items.end()),
        [&](EdgePtrWrapper&& edge)
        {
            auto i = m_graph.getRoot(edge->i);
            auto j = m_graph.getRoot(edge->j);

            size_t outsideVertex = edge.getOutsideVertex();
            assert(i == outsideVertex || j == outsideVertex);

            clusters[outsideVertex].emplace(edge);
        });

    for (auto& cluster : clusters | ranges::views::values){

        for (auto& edge : cluster | ranges::views::drop(1)){
            m_graph.disableEdge(edge->index);
        }

        m_tree.top().pushToHeap(*cluster.begin());
    }
}

SoftHeapDecorator* TreeBuilder::findHeapWithExtensionEdge()
{
    auto stackView = m_tree.view();

    auto transformedStackView = ranges::views::transform(stackView, &ISubGraph::findHeapWithMin);
    const auto min_element = ranges::min_element(transformedStackView,
        [](SoftHeapDecorator* left,
            SoftHeapDecorator* right)
        {
            if (!left) return false;

            if (!right) return true;

            return *left->findMin() < *right->findMin();
        });

    return *min_element; 
}

SoftHeapDecorator::ExtractedItems TreeBuilder::fusion(EdgePtrWrapper& extensionEdge)
{
    const auto view = m_tree.view();

    for (auto itr = view.begin(); itr != view.end(); ++itr){

        const auto& minLinks = (*itr)->getMinLinks();
        auto edgeItr = ranges::find_if(minLinks,
            [&](const EdgePtrWrapper& edge)
            {
                return edge <= extensionEdge;
            });

        if (edgeItr != minLinks.cend()){
            return m_tree.fusion(itr.base(), *edgeItr);
        }
    }
    return {};
}

void TreeBuilder::postRetractionActions(SoftHeapDecorator::ExtractedItems items)
{
    for (auto& corruptedEdge : items.corrupted){

        m_graph.disableEdge(corruptedEdge->index);
    }

    createClustersAndPushCheapest(std::move(items.items));
}
