// This class represents one node in the tree, that is actually a subgraph of the original graph. 
// It is one node on the active path

#pragma once
#include "SoftHeapDecorator.h"

#include <ranges>
#include <set>


class SubGraph;

// used as interface that is implemented by SubGraph class
struct ISubGraph
{
    virtual ~ISubGraph() {}

    // use to store a border edge into the heap
    virtual void pushToHeap(EdgePtrWrapper edge) = 0;

    virtual size_t getLevelInTree() const = 0;
    // there is specified limit of nodes number for each level, tree construction stops when the specified limit is met
    virtual bool satisfiedTargetSize() const = 0;

    virtual list<size_t> getVertices(bool cache = false) = 0;
    // minimum edges connected to the node (component) that will be next added to the active path
    virtual vector<EdgePtrWrapper> getMinLinks() const = 0;

    //min heap contains component for path extension
    virtual SoftHeapDecorator* findHeapWithMin() = 0;
};

using ISubGraphPtr = shared_ptr<ISubGraph>;
using SubGraphPtr = shared_ptr<SubGraph>;

// SubGraph contains its subcomponents, border edges and min-link
// SoftHeapDecorator is used to store candidates (border edges) in heap 
class SubGraph : public ISubGraph
{
public:
    // creation of a new component, starts with only one node
    SubGraph(size_t vertex, size_t levelInTree, size_t targetSize, size_t r, set<size_t>& badEdges);
    // creation of a new parent subgraph of existing children node (one level up in the tree)
    SubGraph(const SubGraphPtr& child, size_t targetSize, size_t r, set<size_t>& badEdges);

    // copies mustn't be created, there is a shared ptr instead
    SubGraph(SubGraph&& other) = delete;
    SubGraph(const SubGraph& other) = delete;
    SubGraph& operator=(const SubGraph& other) = delete;
    SubGraph& operator=(SubGraph&& other) = delete;

    void pushToHeap(EdgePtrWrapper edge) override;

    size_t getLevelInTree() const override;
    bool satisfiedTargetSize() const override;
    
    list<size_t> getVertices(bool cache = false) override;
    vector<EdgePtrWrapper> getMinLinks() const { return m_minLinksToNextNodesInActivePath; }

    SoftHeapDecorator* findHeapWithMin() override;

    void meldHeapsFrom(SubGraphPtr& other);
    void addToMinLinks(const EdgePtrWrapper& edge);
    void popMinLink();

    SoftHeapDecorator::ExtractedItems extractItems();

    // connect subgraph to the child by passed the edge
    void addChild(size_t edge, const SubGraphPtr& child);
    // used during fusion
    SubGraphPtr popLastChild();
    auto getChildren() const { return m_children | ranges::views::values; }

    auto getChildrenEdges() const
    {
        return m_children
            | ranges::views::keys
            | ranges::views::filter([](const optional<size_t>& v) { return v.has_value(); })
            | ranges::views::transform([](const optional<size_t>& v) {return v.value(); });
    }

    // delete edges from the heap when specific edge becomes intern edge, not border due to heaps melding
    list<EdgePtrWrapper> deleteAndReturnIf(const function<bool(const EdgePtrWrapper& edge)>& func);

private:
    // initialize vector of heaps for this subtree
    void initHeaps();
private:
    // has value only if the vertex is leaf
    const optional<size_t> m_vertex;
    // index on the active path
    const size_t m_levelInTree;
    const size_t m_targetSize;
    const size_t m_r;

    // stores index of the edge to all children (optional values since they can be sometimes unknown)
    vector<pair<optional<size_t>, SubGraphPtr>> m_children{};

    vector<SoftHeapDecorator> m_heaps;
    vector<EdgePtrWrapper> m_minLinksToNextNodesInActivePath{};

    list<size_t> m_cachedVertices{};
    set<size_t>& m_badEdges;
};
