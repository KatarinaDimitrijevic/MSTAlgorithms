#pragma once
#include "./Graph.h"
#include "./SoftHeap/SoftHeap.h"

#include <array>
#include <optional>
#include <ostream>
#include <set>

using namespace std;

class EdgePtrWrapper
{
public:
    // class that stores pointers to the original edges
    EdgePtrWrapper(const Graph::Edge* edge, size_t outsideVertex)
        : m_edge{ edge }
        , m_outsideVertex{ outsideVertex } {
    }

    const Graph::Edge& getEdge() const { return *m_edge; }
    const Graph::Edge* operator->() const { return m_edge; }

    size_t getOutsideVertex() const { return m_outsideVertex; }

    bool operator<(const EdgePtrWrapper& rhs) const { return m_workingCost < rhs.m_workingCost; }
    bool operator<=(const EdgePtrWrapper& rhs) const { return m_workingCost <= rhs.m_workingCost; }
    bool operator==(const EdgePtrWrapper& rhs) const { return m_edge == rhs.m_edge; }

    // originally it is weight of the edge, but it can be increased (corrupted) by soft heap
    void setWorkingCost(size_t cost) { m_workingCost = cost; }
    size_t getWorkingCost() const { return m_workingCost; }

    void setIsCorrupted(bool corrupted) { m_isEdgeCorrupted = corrupted; }
    bool getIsCorrupted() const { return m_isEdgeCorrupted; };

private:
    const Graph::Edge* const m_edge;

    size_t m_workingCost = m_edge->weight;
    // node that is outside the component
    const size_t m_outsideVertex;

    // if its working cost is modified by m_heap (soft heap)
    bool m_isEdgeCorrupted = false;
};

// shared pointer to the object that contains edge info (if it's corrupted, its working cost and outsideVertex)
struct EdgePtrWrapperShared
{
    EdgePtrWrapperShared(const shared_ptr<EdgePtrWrapper>& edgeInfo)
        : sharedPointer{ edgeInfo } {
    }

    // compares their working costs
    bool operator<(const EdgePtrWrapperShared& rhs) const { return *sharedPointer < *rhs.sharedPointer; }
    bool operator==(const EdgePtrWrapperShared& rhs) const { return *sharedPointer == *rhs.sharedPointer; }

    std::shared_ptr<EdgePtrWrapper> sharedPointer;
};

class SoftHeapDecorator
{
public:
    // this decorator is used by soft heap in order to work with edges
    explicit SoftHeapDecorator(size_t r, set<size_t>& badEdges);

    struct ExtractedItems
    {
        list<EdgePtrWrapper> corrupted{};
        list<EdgePtrWrapper> items{};
    };

    // insert edge 
    void insert(EdgePtrWrapper newKey);
    // merge two heaps
    void meld(SoftHeapDecorator& other);
    ExtractedItems extractItems();

    EdgePtrWrapper  deleteMin();
    EdgePtrWrapper* findMin();

    list<EdgePtrWrapper> deleteAndReturnIf(const function<bool(const EdgePtrWrapper& edge)>& func);

private:
    SoftHeap<EdgePtrWrapperShared> m_heap;
    list<shared_ptr<EdgePtrWrapper>> m_items{};
};
