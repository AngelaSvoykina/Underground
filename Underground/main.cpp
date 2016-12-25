#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include "Map.h"
#include "A_search.h"

//	Из пути - вектора индексов, формируем выходные команды перемещения U, D, R, L
std::vector<char> make_actions(const Map& map, const std::vector<MapIndex>& path)
{
	int w = map.width();
	std::vector<char> actions;
	for (size_t i = 1; i < path.size(); i++)
	{
		int delta = path[i] - path[i-1];
		if (delta == 1)
			actions.push_back('R');
		else if (delta == -1)
			actions.push_back('L');
		else if (delta == w)
			actions.push_back('D');
		else if (delta == -w)
			actions.push_back('U');
		else
			throw std::runtime_error("Bad path!");
	}
	return actions;
}

//	Ввод в выходной файл os выводных данных
void process_input(std::ifstream& ifs, std::ostream& os)
try
{
	Map undeground(ifs);
	std::vector<MapIndex> path = a_star_search(undeground, undeground.start(), undeground.goal());

	if (path[0] == -1) //	если нет пути
	{
		os << "Пути не существует!\n\n";
		return;
	}

	undeground.set_path(path);
	os << "Карта:\n" << undeground << std::endl;

	os << "Действия:\n";
	std::vector<char> actions = make_actions(undeground, path);
	for (char ch : actions)
	{
		os << ch << ' ';
	}
	os << std::endl << std::endl;
}
catch (std::exception& err)
{
	os << "Ошибка: " << err.what() << std::endl;
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	if (argc <= 1)
		return 1;

	std::ofstream os("output.txt");
	if (!os)
		return 2;

	for (int i = 1; i < argc; i++)
	{ 
		std::ifstream ifs(argv[i]);
		if (!ifs)
		{
			os << "Файл: " << argv[i] << " не открывается!\n\n";
			continue;
		}
		os << "Файл: " << argv[i] << std::endl;
		process_input(ifs, os);
	}
	return 0;
}



