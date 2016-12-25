#include <queue>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "Map.h"
#include "PQposition.h"


inline int heuristic(const Position& p1, const Position& p2)
{
	return std::abs(p1.x-p2.x) + std::abs(p1.y - p2.y);
}

std::vector<MapIndex> a_star_search(const Map& map, const Position& start, const Position& goal)
{
	MapIndex istart = map.index(start);
	MapIndex igoal = map.index(goal);
	PQindex frontier;
	frontier.put(istart, 0);

	std::unordered_map<MapIndex, MapIndex> came_from;
	came_from[istart] = istart;

	std::unordered_map<MapIndex, int> cost_so_far;
	cost_so_far[istart] = 0;

	while (!frontier.empty())
	{
		MapIndex current = frontier.get();
		
		if (current == igoal)
			break;

		int new_cost = cost_so_far[current] + 1;
		for (MapIndex next : map.neighbors(current))
		{
			if (!cost_so_far.count(next) || new_cost < cost_so_far[next])
			{
				cost_so_far[next] = new_cost;
				int priority = new_cost + heuristic(map.position(next), goal);
				frontier.put(next, priority);
				came_from[next] = current;
			}
		}
	}
	if (came_from.find(igoal) == came_from.end())
		return std::vector<MapIndex>{-1};
	// реконструируем путь
	std::vector<MapIndex> path;
	MapIndex current = igoal;
	path.push_back(current);
	while (current != istart)
	{
		current = came_from[current];
		path.push_back(current);
	}
	std::reverse(path.begin(), path.end());
	return path;
}