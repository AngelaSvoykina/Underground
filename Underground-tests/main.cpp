#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include "Map.h"
#include "A_search.h"
#include "gtest/gtest.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(Input, read_field)
{
	extern std::string read_field(std::istream& is);
	
	std::istringstream ss("\tFall\n\t \t");
	std::string buff;
	char ch = 0;

	buff = read_field(ss);
	EXPECT_EQ(buff.size(), 0);
	ss.get(ch);
	EXPECT_EQ(ch, '\t');

	buff = read_field(ss);
	EXPECT_EQ(buff, "Fall");
	ss.get(ch);
	EXPECT_EQ(ch,'\n');

	buff = read_field(ss);
	EXPECT_EQ(buff.size(), 0);
	ss.get(ch);
	EXPECT_EQ(ch, '\t');

	buff = read_field(ss);
	EXPECT_EQ(buff, " ");
	ss.get(ch);
	EXPECT_EQ(ch, '\t');

	buff = read_field(ss);
	EXPECT_EQ(buff.size(), 0);
	EXPECT_TRUE(ss.eof());

	std::istringstream empty;
	buff = read_field(empty);
	EXPECT_EQ(buff.size(), 0);
	EXPECT_TRUE(empty.eof());

	std::istringstream bad;
	bad.clear(std::ios::badbit);
	EXPECT_ANY_THROW(read_field(bad));
}
TEST(Input, read_line)
{
	extern std::vector<Cell> read_line(std::istream& is);
	
	std::istringstream ss("F\tF\t\nS\nG\n\n");
	std::vector<Cell> line;

	line = read_line(ss);
	EXPECT_EQ(line, std::vector<Cell>({Cell::space, Cell::space, Cell::wall}));

	line = read_line(ss);
	EXPECT_EQ(line, std::vector<Cell>({Cell::start}));

	line = read_line(ss);
	EXPECT_EQ(line, std::vector<Cell>({Cell::end}));

	line = read_line(ss);
	EXPECT_EQ(line, std::vector<Cell>({Cell::wall}));
	
	line = read_line(ss);
	EXPECT_EQ(line, std::vector<Cell>());
	EXPECT_TRUE(ss.eof());

	std::istringstream bad1("\tg\t");
	EXPECT_ANY_THROW(read_line(bad1));

	std::istringstream bad2("hello world!");
	EXPECT_ANY_THROW(read_line(bad2));
}
TEST(Input, read_map)
{
	extern void read_map(std::istream&, std::vector<Cell>&, int& w, int& h);

	std::istringstream ss1("");
	std::vector<Cell> v1; int w1, h1;
	read_map(ss1, v1, w1, h1);
	EXPECT_EQ(w1, 0);
	EXPECT_EQ(h1, 0);
	EXPECT_TRUE(v1.empty());

	std::istringstream ss2("\t\t\nF\tF\tF\nS\tG\t\n");
	std::vector<Cell> v2; int w2, h2;
	read_map(ss2, v2, w2, h2);
	EXPECT_EQ(w2, 3);
	EXPECT_EQ(h2, 3);
	EXPECT_EQ(v2, std::vector<Cell>({Cell::wall, Cell::wall, Cell::wall, Cell::space, Cell::space, Cell::space, Cell::start, Cell::end, Cell::wall}));

	std::istringstream ss3("\t\t\n\t\n");
	std::vector<Cell> v3; int w3, h3;
	EXPECT_ANY_THROW(read_map(ss3, v3, w3, h3));

	std::istringstream ss4;
	ss4.clear(std::ios::badbit);
	std::vector<Cell> v4; int w4, h4;
	EXPECT_ANY_THROW(read_map(ss4, v4, w4, h4));
}
TEST(Input, construct_map)
{
	std::istringstream ss1("S\t\n");
	EXPECT_ANY_THROW(Map::Map(ss1));
	
	std::istringstream ss2("G\t\n");
	EXPECT_ANY_THROW(Map::Map(ss2));

	std::istringstream ss3("S\tG\n");
	Map map3(ss3);
	EXPECT_EQ(map3.get(0), Cell::start);
	EXPECT_EQ(map3.get(1), Cell::end);
	EXPECT_TRUE(map3.index(map3.start()) == 0);
	EXPECT_TRUE(map3.index(map3.goal()) == 1);
}

class MapTest : public ::testing::Test
{
public:
	/*	WS_
		G_W 
	*/
	Map m1 = Map(std::vector<Cell>({Cell::wall, Cell::start, Cell::space, Cell::end, Cell::space, Cell::wall}), 3);
	Map m2 = Map(std::vector<Cell>({Cell::wall, Cell::start, Cell::space, Cell::end}), 2);
	Map m3 = Map(std::vector<Cell>({Cell::start, Cell::space, Cell::space, Cell::space, Cell::space, Cell::end}), 3);
};
TEST_F(MapTest, BadVectorInit)
{
	EXPECT_ANY_THROW(Map(std::vector<Cell>({ Cell::wall, Cell::start, Cell::end }), 2));
}
TEST_F(MapTest, check_position)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 2; ++j)
			EXPECT_TRUE(m1.check_position(Position(i,j)));
	EXPECT_FALSE(m1.check_position(Position(2,2)));

	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			EXPECT_TRUE(m2.check_position(Position(i,j)));
	EXPECT_FALSE(m2.check_position(Position(1, 2)));
}
TEST_F(MapTest, check_index)
{
	for (int i = 0; i < 6; ++i)
		EXPECT_TRUE(m1.check_index(i));
	EXPECT_FALSE(m1.check_index(6));

	for (int i = 0; i < 4; ++i)
		EXPECT_TRUE(m2.check_index(i));
	EXPECT_FALSE(m2.check_index(4));
}
TEST_F(MapTest, get)
{
	std::vector<Cell> v1({ Cell::wall, Cell::start, Cell::space, Cell::end, Cell::space, Cell::wall});
	for (int i = 0; i < 6; ++i)
		EXPECT_EQ(m1.get(i), v1[i]);

	std::vector<Cell> v2({ Cell::wall, Cell::start, Cell::space, Cell::end });
	for (int i = 0; i < 4; ++i)
		EXPECT_EQ(m2.get(i), v2[i]);
}
TEST_F(MapTest, passable)
{
	std::vector<bool> v1({ false, true, true, true, true, false });
	for (int i = 0; i < 6; ++i)
		EXPECT_EQ(m1.passable(i), v1[i]);

	std::vector<bool > v2({ false, true, true, true });
	for (int i = 0; i < 4; ++i)
		EXPECT_EQ(m2.passable(i), v2[i]);
}
TEST_F(MapTest, neighbors)
{
	std::vector<MapIndex> n1 = m1.neighbors(1);
	std::vector<MapIndex> v1 = {2, 4};
	for (MapIndex i : n1)
		EXPECT_NE(std::find(v1.begin(), v1.end(), i), v1.end());

	std::vector<MapIndex> n2 = m1.neighbors(2);
	std::vector<MapIndex> v2 = {1};
	for (MapIndex i : n2)
		EXPECT_NE(std::find(v2.begin(), v2.end(), i), v2.end());
			
	std::vector<MapIndex> n3 = m1.neighbors(3);
	std::vector<MapIndex> v3 = { 4 };
	for (MapIndex i : n3)
		EXPECT_NE(std::find(v3.begin(), v3.end(), i), v3.end());
}
TEST_F(MapTest, start)
{
	EXPECT_EQ(m1.start(), Position(1, 0));
	EXPECT_EQ(m2.start(), Position(1, 0));
}
TEST_F(MapTest, goal)
{
	EXPECT_EQ(m1.goal(), Position(0,1));
	EXPECT_EQ(m2.goal(), Position(1,1));
}
TEST_F(MapTest, index)
{
	for (int x = 0; x < 3; ++x)
		for (int y = 0; y < 2; ++y)
			EXPECT_EQ(m1.index(Position(x,y)), y*3+x);

	for (int x = 0; x < 2; ++x)
		for (int y = 0; y < 2; ++y)
			EXPECT_EQ(m2.index(Position(x, y)), y * 2 + x);
}
TEST_F(MapTest, position)
{
	for (MapIndex i = 0; i < 6; ++i)
		EXPECT_EQ(m1.position(i), Position(i%3,i/3));

	for (MapIndex i = 0; i < 4; ++i)
		EXPECT_EQ(m2.position(i), Position(i % 2, i / 2));
}
TEST_F(MapTest, width)
{
	EXPECT_EQ(m1.width(), 3);
	EXPECT_EQ(m2.width(), 2);
}
TEST_F(MapTest, set_path)
{
//	Map m3 = Map(std::vector<Cell>({Cell::start, Cell::space, Cell::space, Cell::space, Cell::space, Cell::end}), 3);
	std::vector<MapIndex> v1({2,3,4});
	m3.set_path(v1);
	std::vector<Cell> w1({ Cell::start, Cell::space, Cell::path, Cell::path, Cell::path, Cell::end });
	for (int i = 0; i < 6; ++i)
		EXPECT_EQ(m3.get(i), w1[i]);

	std::vector<MapIndex> v2({0,1,2,3});
	m2.set_path(v2);
	std::vector<Cell> w2({ Cell::wall, Cell::start, Cell::path, Cell::end });
		for (int i = 0; i < 4; ++i)
			EXPECT_EQ(m2.get(i), w2[i]);
}
TEST(A_star, finding)
{
	std::istringstream ss1(R"(														
	F	F	F	F	F	F	F	F	F	F	F	F	G	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	S	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
	F	F	F	F	F	F	F	F	F	F	F	F	F	
														)");
	Map m1(ss1);
	std::vector<MapIndex> w1 = a_star_search(m1, m1.start(), m1.goal());
	EXPECT_EQ(w1.size(), 12);

	std::istringstream ss2(R"(														
	G	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F	F	F	F	F	F	F	F	
	F	F	F		F	F				F	F	F	F	
	F	F	F		F	F		F	F	F	F	F	F	
	F	F	F		F	F		F	F					
	F	F	F	F	F	F		F	F	F	F	F	S	
														)");
	Map m2(ss2);
	std::vector<MapIndex> w2 = a_star_search(m2, m2.start(), m2.goal());
	EXPECT_EQ(w2.size(), 35);
}