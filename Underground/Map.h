#pragma once
#include <iostream>
#include <vector>
#include <array>

struct SizeMap {
	int w = 0;
	int h = 0;
	SizeMap(int wt, int ht) :w(wt), h(ht) {}
	SizeMap() :w(0), h(0) {}
};
enum class Cell {
	wall, space, start, path, end
};
struct Position { //Координаты на карте
	int x;
	int y;
	Position(int tx, int ty) :x(tx), y(ty) {}
};
inline bool operator==(const Position& lhs, const Position& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
Position operator+(const Position& a, const Position& b);
using MapIndex = int;
class Map {
public:
	Map(std::istream&);
	Map(std::vector<Cell>&, int w);

	bool check_position(const Position& p) const; //проверяет верна ли позиция
	bool check_index(MapIndex i) const {return i >= 0 && static_cast<size_t>(i) < arr.size(); }
//	Cell get(const Position& p) const;
	Cell get(MapIndex i) const { return arr[i]; }
//	inline bool passable(const Position& p) const {return get(p) != Cell::wall; }
	inline bool passable(MapIndex i) const { return arr[i] != Cell::wall; }
	std::vector<MapIndex> neighbors(MapIndex p) const;

	Position start() const { return position(start_); }
	Position goal() const { return position(goal_); }

	MapIndex index(const Position& p) const;
	Position position(MapIndex i) const;

	int width() const {return size.w;}

	void set_path(const std::vector<MapIndex>& path);
private:
	std::vector<Cell> arr; // Одномерный массив к которому мы обращаемся как к двумерному
	SizeMap size;
	MapIndex start_;
	MapIndex goal_;
	std::array<MapIndex, 4> index_DIRS;

	void init();
	friend std::ostream& operator<<(std::ostream& os, const Map& m);
};