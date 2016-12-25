#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "Map.h"

constexpr char char_tab = '\t';
constexpr char char_space = 'F';
constexpr char char_start = 'S';
constexpr char char_end = 'G';

Position operator+(const Position& a, const Position& b)
{
	return Position(a.x+b.x, a.y+b.y);
}
const std::array<Position, 4> DIRS = { Position{ 1,0 } ,Position{ 0,-1 },Position{ -1, 0 },Position{ 0,1 } };

MapIndex Map::index(const Position& p) const
{
	if (!check_position(p))
		return -1;
	return (p.x + p.y*size.w);
}

Position Map::position(MapIndex i) const
{
	return Position(i % size.w, i / size.w);
}

bool Map::check_position(const Position& p)  const
{
	if (p.x < 0 || p.y < 0 || p.x >= size.w || p.y >= size.h)
		return false;
	return true;
}

std::vector<MapIndex> Map::neighbors(MapIndex p) const
{
	std::vector<MapIndex> result;
	for (MapIndex dir : index_DIRS)
	{
		int x = p % size.w;
		MapIndex next{};
		if ((x == 0 && dir == -1) || (x == size.w - 1 && dir == 1))
			next = -1;
		else
			next = dir + p;
		if (check_index(next) && passable(next))
			result.push_back(next);
	}
	return result;
}
void init_index_dirs(std::array<MapIndex, 4>& index_DIRS, int w)
{
	for (size_t i = 0; i < 4; i++)
		index_DIRS[i] = DIRS[i].x + DIRS[i].y * w;
}

std::string read_field(std::istream& is)
{
	//	Читаем символы в буффер, пока не получим \n,\t или eof
	std::string buffer;
	char c = 0;
	while (is.get(c))
	{
		if (c == '\t' || c == '\n')
		{
			is.putback(c);
			return buffer;
		}
		buffer += c;
	}
	if (is.eof())
		return buffer;
	else //	Фэйл или бад бит
		throw std::runtime_error("read_field Îøèáêà!\n");
}
std::vector<Cell> read_line(std::istream& is)
{
	std::vector<Cell> line;
	char ch = 0;
	if (is.get(ch))
		is.putback(ch);
	else
		return std::vector<Cell>();
	while (true)
	{
		std::string field = read_field(is);

		if (field.size() == 0)
			line.push_back(Cell::wall);
		else if (field.size() > 1)
			throw std::runtime_error("read_line íåâåðíûé ñèìâîë!\n");
		else
		{
			char f = field[0];
			switch(f)
			{
			case char_start:
				line.push_back(Cell::start);
				break;
			case char_space:
				line.push_back(Cell::space);
				break;
			case char_end:
				line.push_back(Cell::end);
				break;
			default:
				throw std::runtime_error("read_line íåâåðíûé ñèìâîë!\n");
			}
		}

		char c = 0;
		if (is.get(c) && c == '\t')
			continue;
		else //	'\n' или конец файла
			return line;			
	}
}
void read_map(std::istream& is, std::vector<Cell>& data, int& w, int& h)
{
	data.clear();
	int fields_in_line = 0;
	bool first_line = true;
	int lines = 0;

	if (!is.good())
		throw std::runtime_error("Ïîòîê ââîäà â íåõîðîøåì ñîñòîÿíèè!\n");
	
	for (char ch = 0; is.get(ch); ++lines)
	{
		is.putback(ch);

		std::vector<Cell> line = read_line(is);
		if (first_line)
		{
			fields_in_line = static_cast<int>(line.size());
			first_line = false;
		}
		else if (fields_in_line != line.size())
			throw std::runtime_error("Ðàçíîå êîëè÷åñòâî ïîëåé â ñòðîêàõ â îïèñàíèè êàðòû!\n");
		data.insert(data.end(), line.begin(), line.end());
	}
	w = fields_in_line;
	h = lines;
}
MapIndex get_once_field_index(const std::vector<Cell>& map, Cell type)
{ 
	//	Возвращает -1 если поля данного типа не найдено, или если их больше одного
	auto it = std::find(map.begin(), map.end(), type);
	if (it == map.end())
		return -1;
	else
	{
		if (std::find(it+1, map.end(), type) != map.end())
			return -1;
		return static_cast<MapIndex>(it - map.begin());
	}
}
void Map::init()
{
	start_ = get_once_field_index(arr, Cell::start);
	if (start_ == -1)
		throw std::runtime_error("Íà êàðòå íå óêàçàí ñòàðò, èëè èõ óêàçàíî áîëüøå îäíîãî!\n");

	goal_ = get_once_field_index(arr, Cell::end);
	if (goal_ == -1)
		throw std::runtime_error("Íà êàðòå íå óêàçàí ôèíèø, èëè èõ óêàçàíî áîëüøå îäíîãî!\n");

	init_index_dirs(index_DIRS, size.w);
}
Map::Map(std::istream& is)
:arr{}
{
	int w, h;
	read_map(is, arr, w, h);
	size.w = w;
	size.h = h;
	Map::init();
}
Map::Map(std::vector<Cell>& cells, int w)
{
	arr = cells;
	int length = static_cast<int>(cells.size());
	if (length % w != 0)
		throw std::runtime_error("Ðàçìåð êàðòû óêàçàí íåâåðíî!\n");
	size.w = w;
	size.h = length / w;
	Map::init();
}
std::ostream& operator<<(std::ostream& os, const Map& m)
{
	int i = 0;
	for (auto c : m.arr)
	{
		if (i != 0 && (i%m.size.w) == 0)
			os << '\n';
		i++;

		switch (c)
		{
		case Cell::wall:
			os << '0';
			break;
		case Cell::space:
			os << ' ';
			break;
		case Cell::start:
			os << 's';
			break;
		case Cell::path:
			os << '+';
			break;
		case Cell::end:
			os << 'g';
			break;
		}
	}
	return os;
}

void Map::set_path(const std::vector<MapIndex>& path)
{
	for (MapIndex i : path)
	{
		if (arr[i] == Cell::space)
			arr[i] = Cell::path;
	}
}
