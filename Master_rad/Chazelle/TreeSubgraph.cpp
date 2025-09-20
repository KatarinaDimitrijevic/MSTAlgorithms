#include "TreeSubgraph.h"
#include "SoftHeap/Utils.h"


SubGraph::SubGraph(size_t vertex, size_t levelInTree, size_t targetSize, size_t r, set<size_t>& badEdges)
    : m_vertex{ vertex }
    , m_levelInTree{ levelInTree }
    , m_targetSize{ targetSize }
    , m_r{ r }
    , m_badEdges{ badEdges }
{
    initHeaps();
}

SubGraph::SubGraph(const SubGraphPtr& child, size_t targetSize, size_t r, set<size_t>& badEdges)
    : m_levelInTree{ child->getLevelInTree() - 1 }
    , m_targetSize{ targetSize }
    , m_r{ r }
    // initially only one child
    , m_children{ {std::nullopt, child} }
    , m_badEdges{ badEdges }
{
    initHeaps();
}

void SubGraph::initHeaps()
{
    m_heaps.reserve(m_levelInTree + 1);

    // suppose that each subtree contains m_levelInTree+1 component: one for each on the active path and one for the outside 
    for (size_t i = 0; i < m_levelInTree + 1; ++i) {
        m_heaps.emplace_back(m_r, m_badEdges);
    }

    m_heaps.shrink_to_fit();
}

size_t SubGraph::getLevelInTree() const
{
    return m_levelInTree;
}

bool SubGraph::satisfiedTargetSize() const
{
    return m_vertex.has_value() ? true : m_children.size() >= m_targetSize;
}

list<size_t> SubGraph::getVertices(bool cache)
{
    assert(m_vertex.has_value() && m_children.empty() || !m_vertex.has_value() && !m_children.empty());

    if (m_vertex)
        return { *m_vertex };

    // if there are some cached results, use them to avoid duplicated recursion
    if (cache && !m_cachedVertices.empty())
        return m_cachedVertices;

    list<size_t> result{};

    for (const auto& child : m_children) {
        // apply recursion to the children
        result.splice(result.end(), child.second->getVertices(cache));
    }

    if (cache) {
        m_cachedVertices = result;
    }

    // return all vertices from the component - not nodes, but original vertices
    return result;
}

void SubGraph::pushToHeap(EdgePtrWrapper edge)
{
    m_heaps[m_levelInTree].insert(edge);
}

void SubGraph::meldHeapsFrom(SubGraphPtr& other)
{
    for (size_t i = 0; i < m_levelInTree; ++i) {
        m_heaps[i].meld(other->m_heaps[i]);
    }
}

void SubGraph::addToMinLinks(const EdgePtrWrapper& edge)
{
    m_minLinksToNextNodesInActivePath.emplace_back(edge);
}

void SubGraph::popMinLink()
{
    if (!m_minLinksToNextNodesInActivePath.empty()) {
        m_minLinksToNextNodesInActivePath.pop_back();
    }
}

SoftHeapDecorator::ExtractedItems SubGraph::extractItems()
{
    SoftHeapDecorator::ExtractedItems data{};

    // process the heaps starting from the last one, but at most two of them - the one related to the outside
    // and to the previous component from the active path
    for_each_n(m_heaps.rbegin(),
        min(size_t{ 2 }, m_levelInTree),
        [&](SoftHeapDecorator& heap)
        {
            auto heap_data = heap.extractItems();
            data.corrupted.splice(data.corrupted.end(), heap_data.corrupted);
            data.items.splice(data.items.end(), heap_data.items);
        });

    // clear heaps and prepare fot the next phase
    m_heaps.clear();
    initHeaps();

    return data;
}

void SubGraph::addChild(size_t edgeIndex, const SubGraphPtr& child)
{
    m_children.emplace_back(edgeIndex, child);
}

SubGraphPtr SubGraph::popLastChild()
{
    // remove the last child and return its shared pointer
    assert(!m_children.empty());

    auto subgraph = move(m_children.back().second);
    m_children.pop_back();

    return subgraph;
}

SoftHeapDecorator* SubGraph::findHeapWithMin()
{
    auto* minHeap = &m_heaps.front();
    auto  minValue = minHeap->findMin();
    for (auto& heap : m_heaps | ranges::views::drop(1)){

        const auto newValue = heap.findMin();
        if (!minValue || (newValue && (*newValue < *minValue))) {

            minHeap = &heap;
            minValue = newValue;
        }
    }
    return minValue ? minHeap : nullptr;
}


list<EdgePtrWrapper> SubGraph::deleteAndReturnIf(const function<bool(const EdgePtrWrapper& edge)>& func)
{
    list<EdgePtrWrapper> result{};
    
    for (auto& heap : m_heaps) {
        result.splice(result.end(), heap.deleteAndReturnIf(func));
    }
    return result;
}
