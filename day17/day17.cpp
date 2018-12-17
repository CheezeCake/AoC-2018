#include <iostream>
#include <tuple>
#include <string>
#include <map>
#include <utility>
#include <unordered_set>
#include <set>
#include <vector>
#include <limits>
#include <algorithm>

struct Point
{
	int x;
	int y;
};

bool operator<(const Point& lhs, const Point& rhs)
{
	return (lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x));
}

enum class Type : char { Clay = '#', /*Sand = '.',*/ WetSand = '|', Water = '~'};

struct Range
{
	int min;
	int max;
};

Range range(const std::string& range)
{
	Range r;
	r.min = std::stoi(range.substr(0, range.find('.')));
	r.max = std::stoi(range.substr(range.rfind('.') + 1));
	return r;
}

std::vector<Point> xRange(const std::string& x, int y)
{
	std::vector<Point> points;
	auto [start, end] = range(x);

	for (int x = start; x <= end; ++x)
		points.push_back({x, y});

	return points;
}

std::vector<Point> yRange(int x, const std::string& y)
{
	std::vector<Point> points;
	auto [start, end] = range(y);

	for (int y = start; y <= end; ++y)
		points.push_back({x, y});

	return points;
}

std::map<Point, Type> readInput()
{
	std::map<Point, Type> input;

	std::string line;
	while (std::getline(std::cin, line)) {
		char var1{line[0]};
		int var1Val{std::stoi(line.substr(2, line.find(',') - 2))};
		auto range{line.substr(line.rfind('=') + 1)};

		std::vector<Point> points;
		if (var1 == 'x')
			points = yRange(var1Val, range);
		else
			points = xRange(range, var1Val);

		for (const auto& p : points)
			input.emplace(p, Type::Clay);
	}

	return input;
}

struct Bounds
{
	Range X;
	Range Y;
};

Bounds bounds(const std::map<Point, Type>& points)
{
	int yMin{std::numeric_limits<int>::max()};
	int yMax{std::numeric_limits<int>::min()};
	int xMin{std::numeric_limits<int>::max()};
	int xMax{std::numeric_limits<int>::min()};

	for (const auto& [p, t] : points) {
		yMin = std::min(yMin, p.y);
		yMax = std::max(yMax, p.y);

		xMin = std::min(xMin, p.x);
		xMax = std::max(xMax, p.x);
	}

	return {{xMin, xMax}, {yMin, yMax}};
}

void setOverflow(int xFrom, int xTo, int y, int dir, std::map<Point, Type>& tiles)
{
	for (int x{xFrom}; x != xTo + dir; x += dir) {
		auto tile = tiles.find({x, y});
		if (tile != std::end(tiles)) {
			if (tile->second == Type::Clay)
				break;
			tile->second = Type::WetSand;
		}
	}
}

bool flow(Point current, std::map<Point, Type>& tiles, const Bounds& bounds)
{
	if (tiles.find(current) != std::end(tiles))
		return false;

	auto [x, y] = current;
	if (y > bounds.Y.max)
		return true;

	tiles[current] = Type::WetSand;

	if (tiles.find({x, y + 1}) != std::end(tiles) && tiles.at({x, y + 1}) == Type::WetSand)
		return true;

	// down
	if (flow({x, y + 1}, tiles, bounds))
		return true;

	// left and right
	auto l = flow({x - 1, y}, tiles, bounds);
	auto r = flow({x + 1, y}, tiles, bounds);
	if (r)
		setOverflow(x - 1, bounds.X.min, y, -1, tiles);
	if (l)
		setOverflow(x + 1, bounds.X.max, y, 1, tiles);
	if (!l && !r)
		tiles[current] = Type::Water;
	return (l || r);
}

void print(const std::map<Point, Type>& tiles, const Bounds& b)
{
	for (int y{1}; y <= b.Y.max; ++y) {
		for (int x{b.X.min}; x <= b.X.max; ++x) {
			const auto& tile{tiles.find({x, y})};
			if (tile != std::end(tiles))
				std::cout << static_cast<char>(tile->second);
			else
				std::cout << '.';
		}
		std::cout << '\n';
	}
}

int main()
{
	auto input{readInput()};
	auto b = bounds(input);

	flow({500, 0}, input, b);
	/* print(input, b); */

	std::cout << "part 1: " << std::count_if(std::begin(input), std::end(input), [&b](const auto& tile) {
													const auto& [x, y] = tile.first;
													return (tile.second != Type::Clay && y >= b.Y.min && y <= b.Y.max);
												 })<< '\n';
	std::cout << "part 2: " << std::count_if(std::begin(input), std::end(input), [&b](const auto& tile) {
													const auto& [x, y] = tile.first;
													return (tile.second == Type::Water && y >= b.Y.min && y <= b.Y.max);
												 })<< '\n';
}
