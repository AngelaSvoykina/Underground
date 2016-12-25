#pragma once
#include "Map.h"

std::vector<MapIndex> a_star_search(const Map& map, const Position& start, const Position& goal);
