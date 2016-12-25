#pragma once
#include <queue>
#include <functional>
#include <vector>
#include "Map.h"

struct PQElement {
	int priority;
	int index;
};
typedef std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>>  PriorityQueue;
inline bool operator>(const PQElement& l, const PQElement& r)
{
	return l.priority > r.priority;
}

class PQindex {
public:
	bool empty() const { return pq.empty(); }
	void put(int p, int priority) { pq.push({priority, p}); }
	int get() { int p = pq.top().index; pq.pop(); return p; }
private:
	PriorityQueue pq;
};