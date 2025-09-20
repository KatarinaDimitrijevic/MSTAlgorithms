#include "SoftHeapDecorator.h"
#include "./SoftHeap/Utils.h"

#include <algorithm>

// r - soft heap error rate
SoftHeapDecorator::SoftHeapDecorator(size_t r, set<size_t>& badEdges)
    : m_heap{ r,
                // callback that is called when weight of some edge is corrupted (increased)
                // this lambda has a reference to the badEdges
                [&](EdgePtrWrapperShared& edgeInfo, const EdgePtrWrapperShared& changedEdgeInfo)
                {
                    if (edgeInfo.sharedPointer->getWorkingCost() != changedEdgeInfo.sharedPointer->getWorkingCost())
                    {
                        // edge is corrupted if weight (cost) is different for the same key
                        edgeInfo.sharedPointer->setIsCorrupted(true);
                        // put original index to badEdges
                        badEdges.emplace(edgeInfo.sharedPointer->getEdge().index);
                    }
                    edgeInfo.sharedPointer->setWorkingCost(changedEdgeInfo.sharedPointer->getWorkingCost());
                }
    }
{
}

void SoftHeapDecorator::insert(EdgePtrWrapper newKey)
{
    auto ptr = std::make_shared<EdgePtrWrapper>(std::move(newKey));

    // insert edge into the heap
    m_heap.Insert(EdgePtrWrapperShared{ ptr });
    // insert edge into the local edge list
    m_items.emplace_back(std::move(ptr));
}

EdgePtrWrapper SoftHeapDecorator::deleteMin()
{
    // remove current min from the heap
    const auto value = m_heap.DeleteMin();
    auto ptr = value.sharedPointer;

    // if it's already removed from local edges, find the new one
    if (!IsRangeContains(m_items, value.sharedPointer)) {
        return deleteMin();
    }

    m_items.remove(ptr);
    return *ptr;
}

EdgePtrWrapper* SoftHeapDecorator::findMin()
{
    if (m_items.empty()) return {};

    // find current min from the heap
    const auto value_ptr = m_heap.FindMin();
    if (!value_ptr) return {};

    // if it's in local list it returns it, otherwise delete it from the heap
    if (IsRangeContains(m_items, value_ptr->sharedPointer)){
        return value_ptr->sharedPointer.get();
    }

    m_heap.DeleteMin();
    return findMin();
}

list<EdgePtrWrapper> SoftHeapDecorator::deleteAndReturnIf(const function<bool(const EdgePtrWrapper& edge)>& func)
{
    list<EdgePtrWrapper> result{};

    // go through all the items and remove them from the local list if satisfy func and add them to result
    for (auto itr = m_items.begin(); itr != m_items.end();){
        if (func(**itr)){
            result.emplace_back(**itr);
            itr = m_items.erase(itr);
        }
        else
            ++itr;
    }
    return result;
}

// merge two heaps
void SoftHeapDecorator::meld(SoftHeapDecorator& other)
{
    m_items.splice(m_items.end(), other.m_items);
    m_heap.Meld(other.m_heap);
}

SoftHeapDecorator::ExtractedItems SoftHeapDecorator::extractItems()
{
    ExtractedItems edges{};

    for (auto& edge : m_items){
        if (edge->getIsCorrupted()) {
            edges.corrupted.push_back(*edge);
        }
        else {
            edges.items.push_back(*edge);
        }
    }

    m_items.clear();

    // returns the object that contains list of corrupted and list of other edges
    return edges;
}
