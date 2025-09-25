#include <limits>
#include <utility>

#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/d_ary_heap.hpp>

#define INF numeric_limits<int>::max()

using namespace std;

// min heap - efficient access to the node with the cheapest incident edge
struct comparator
{
	bool operator() (const pair<int, int>& a, const pair<int, int>& b) const
	{
		if (a.second != b.second) {
			return a.second > b.second;
		}

		return a.first > b.first;
	}
};

// fibonacci heap is used to store pairs of node IDs and its cheapest edge
typedef boost::heap::fibonacci_heap< pair<int, int>, boost::heap::compare< comparator>, boost::heap::mutable_<true>> fibonacciHeap;
// reference to the each element 
typedef boost::heap::fibonacci_heap< pair<int, int>, boost::heap::compare< comparator>, boost::heap::mutable_<true>>::handle_type fibonacciHeapHandle;