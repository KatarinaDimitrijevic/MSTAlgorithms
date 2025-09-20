#include "Tree.h"

#include "MSTUtils.h"
#include "SoftHeap/Utils.h"

#include <exception>
#include <deque>

static vector<size_t> initTargetSizesPerHeight(size_t t, size_t maxHeight)
{
    vector<size_t> targetSizePerHeight{};

    // leaves
    targetSizePerHeight.push_back(1);

    for (uint32_t i = 1; i <= maxHeight; ++i) {
        targetSizePerHeight.push_back(::MST::calculateTargetSize(t, i));
    }

    return targetSizePerHeight;
}

Tree::Tree(Graph& graph, size_t t, size_t maxHeight, size_t initialVertex)
    : m_graph{ graph }
    , m_r{ CalculateRByEps(1 / static_cast<double>(MST::c)) }
    , m_sizesPerHeight{ initTargetSizesPerHeight(t, maxHeight) }
{
    pushNode(initialVertex);
}

void Tree::push(const EdgePtrWrapper& extensionEdge)
{
    auto& last = m_activePath.back();
    pushNode(extensionEdge.getOutsideVertex());

    last->addChild(extensionEdge->index, m_activePath.back());
}

SoftHeapDecorator::ExtractedItems Tree::pop()
{
    auto& lastSubgraph = m_activePath.back();

    if (m_activePath.size() == 1){
        m_activePath.emplace_front(std::make_shared<SubGraph>(lastSubgraph,
            m_sizesPerHeight[indexToHeight(lastSubgraph->getLevelInTree() - 1)],
            m_r, m_badEdges));
    }

    (*std::next(m_activePath.rbegin()))->meldHeapsFrom(lastSubgraph);
    auto data = lastSubgraph->extractItems();

    m_activePath.pop_back();

    std::for_each_n(m_activePath.begin(),
        m_activePath.size(),
        [](SubGraphPtr& subgraph) { subgraph->popMinLink(); });

    return data;
}

SoftHeapDecorator::ExtractedItems Tree::fusion(list<SubGraphPtr>::iterator itr, const EdgePtrWrapper& fusionEdge)
{
    auto popCount = distance(itr, m_activePath.end()) - 1;

    SoftHeapDecorator::ExtractedItems items{};

    for (size_t i = 0; i < popCount; ++i){
        auto data = pop();

        items.corrupted.splice(items.corrupted.end(), data.corrupted);
        items.items.splice(items.items.end(), data.items);
    }

    auto lastChild = m_activePath.back()->popLastChild();

    auto i = m_graph.getRoot(fusionEdge->i);
    auto j = m_graph.getRoot(fusionEdge->j);

    auto children = m_activePath.back()->getChildren();
    auto child_itr = std::ranges::find_if(children, [&](const SubGraphPtr& sub)
        {
            auto vertices = sub->getVertices();
            return IsRangeContains(vertices, i) || IsRangeContains(vertices, j);
        });

    assert(child_itr != childs.end());

    (*child_itr)->addChild(fusionEdge->index, lastChild);
    return items;
}

ISubGraph& Tree::top()
{
    assert(!m_activePath.empty());
    return *m_activePath.back();
}

size_t Tree::size() const { 
    
    return m_activePath.empty() ? 0 : m_activePath.back()->getLevelInTree() + 1;
}

list<Graph> Tree::createSubGraphs(const std::set<size_t>& badEdges)
{
    list<SubGraphPtr> listOfSubgraphs{ m_activePath.front() };
    list<Graph> result{};

    while (!listOfSubgraphs.empty()){

        auto& front = listOfSubgraphs.front();
        auto& graph = result.emplace_back();

        for (const auto& edgeIndex : front->getChildrenEdges()){

            if (IsRangeContains(badEdges, edgeIndex)) continue;

            auto& edge = m_graph.getEdge(edgeIndex);
            auto i = m_graph.getRoot(edge.i);
            auto j = m_graph.getRoot(edge.j);
            graph.addEdge(i, j, edge.weight, edge.index);
        }

        for (auto& child : front->getChildren()){

            listOfSubgraphs.emplace_back(child);

            auto vertices = child->getVertices(true);
            auto frontVertex = vertices.front();

            for (auto other : vertices | ranges::views::drop(1)) {
                graph.unionNodes(frontVertex, other);
            }
        }

        if (graph.getNodesNumber() <= 1) {
            result.pop_back();
        }

        listOfSubgraphs.pop_front();
    }
    return result;
}

list<size_t> Tree::getVerticesInside()
{
    return m_activePath.front()->getVertices(true);
}

void Tree::pushNode(size_t vertex)
{
    auto index = size();
    
    if (index == 0)
        index = getMaxHeight();

    m_activePath.emplace_back(std::make_shared<SubGraph>(vertex,
        index,
        m_sizesPerHeight[indexToHeight(index)],
        m_r, m_badEdges));

    addNewBorderEdgesAfterPush();
    deleteOldBorderEdgesAndUpdateMinLinksAfterPush();
}

void Tree::addNewBorderEdgesAfterPush()
{
    auto& newNode = m_activePath.back();
    auto  nodeVertices = newNode->getVertices();
    auto allVertices = m_activePath.front()->getVertices();

    assert(nodeVertices.size() == 1);

    unordered_map<size_t, const Graph::Edge*> cheapestEdgePerVertex{};
    m_graph.findValidEdges([&](const Graph::Edge& edge, size_t i, size_t j)
        {
            if (IsRangeContains(nodeVertices, i))
            {
                if (!IsRangeContains(allVertices, j))
                {
                    auto& v = cheapestEdgePerVertex[j];
                    if (!v || v->weight > edge.weight)
                        v = &edge;
                }
            }
            else if (IsRangeContains(nodeVertices, j))
            {
                if (!IsRangeContains(allVertices, i))
                {
                    auto& v = cheapestEdgePerVertex[i];
                    if (!v || v->weight > edge.weight)
                        v = &edge;
                }
            }
        });

    for (const auto& [vertex, edge] : cheapestEdgePerVertex){
        newNode->pushToHeap(EdgePtrWrapper{ edge, vertex });
    }
}

void Tree::deleteOldBorderEdgesAndUpdateMinLinksAfterPush()
{
    auto& newNode = m_activePath.back();
    if (m_activePath.size() <= 1)
        return;

    auto condition = [&, vertex = newNode->getVertices().back()](const EdgePtrWrapper& edge)
        {
            auto   i = m_graph.getRoot(edge->i);
            auto   j = m_graph.getRoot(edge->j);
            return i == vertex || j == vertex;
        };

    std::for_each_n(m_activePath.begin(),
        m_activePath.size() - 1,
        [&](SubGraphPtr& node)
        {
            const auto oldBorderEdges = node->deleteAndReturnIf(condition);
            if (oldBorderEdges.empty()) return;

            auto min = std::min_element(oldBorderEdges.cbegin(),
                oldBorderEdges.cend());
            node->addToMinLinks(*min);
        });
}
