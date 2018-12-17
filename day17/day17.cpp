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

using Point = std::pair<int, int>;

enum class Type { Clay, /*Sand,*/ WetSand, Water };

std::pair<int, int> range(const std::string& range)
{
	std::pair<int, int> r;
	r.first = std::stoi(range.substr(0, range.find('.')));
	r.second = std::stoi(range.substr(range.rfind('.') + 1));
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
		auto var1{line.substr(0, 1)};
		auto equals{line.find('=')};
		auto sep{line.find(", ")};
		int var1Val{std::stoi(line.substr(equals + 1, sep - equals))};

		equals = line.find('=', equals + 2);
		auto var2{line.substr(sep + 2, 1)};
		auto range{line.substr(equals + 1)};

		std::vector<Point> points;
		if (var1 == "x")
			points = yRange(var1Val, range);
		else
			points = xRange(range, var1Val);

		for (const auto& p : points)
			input.emplace(p, Type::Clay);
	}

	return input;
}

using Bounds = std::pair<int, int>;

std::pair<Bounds, Bounds> bounds(const std::map<Point, Type>& points)
{
	int yMin{std::numeric_limits<int>::max()};
	int yMax{std::numeric_limits<int>::min()};
	int xMin{std::numeric_limits<int>::max()};
	int xMax{std::numeric_limits<int>::min()};

	for (const auto& [p, t] : points) {
		yMin = std::min(yMin, p.second);
		yMax = std::max(yMax, p.second);

		xMin = std::min(xMin, p.first);
		xMax = std::max(xMax, p.first);
	}

	return {{xMin, xMax}, {yMin, yMax}};
}

bool canSpread(const Point& p, const std::pair<Bounds, Bounds>& bounds, const std::map<Point, Type>& tiles)
{
	bool ok{false};
	for (int x = p.first - 1; !ok && x >= bounds.first.first; --x) {
		const auto& tile{tiles.find({x, p.second})};
		if (tile != std::end(tiles)) {
			if (tile->second == Type::Clay)
				ok = true;
			else // visited
				return false;
		}
	}
	if (!ok) return false;
	for (int x = p.first + 1; x <= bounds.first.second; ++x) {
		const auto& tile{tiles.find({x, p.second})};
		if (tile != std::end(tiles)) {
			if (tile->second == Type::Clay)
				return true;
			else // visited
				return false;
		}
	}
	return false;
}

bool flow(Point current, std::map<Point, Type>& tiles, const std::pair<Bounds, Bounds>& bounds, bool dw=true)
{
	if (tiles.find(current) != std::end(tiles))
		return false;

	auto [x, y] = current;
	if (y > bounds.second.second)
		return true;

	tiles.emplace(current, Type::Water);
	std::cout << x << ", " << y << '\n';

	if (dw && tiles.find({x, y + 1}) != std::end(tiles) && tiles.at({x, y + 1}) != Type::Clay && !canSpread(current, bounds, tiles)) {
		tiles[current] = Type::WetSand;
		return true;
	}

	bool s = false;
	s = flow({x, y + 1}, tiles, bounds, true);

	if (!s) {
		bool l = false, r = false;
		// left and right
		l = flow({x - 1, y}, tiles, bounds, false);
		r = flow({x + 1, y}, tiles, bounds, false);
		if (l || r) {
			tiles[current] = Type::WetSand;
			if (r) {
				for (int lx{x - 1}; lx >= bounds.first.first; --lx) {
					auto tile = tiles.find({lx, y});
					if (tile != std::end(tiles)) {
						if (tile->second == Type::Clay)
							break;
						tile->second = Type::WetSand;
					}
				}
			}
			if (l) {
				for (int rx{x + 1}; rx <= bounds.first.second; ++rx) {
					auto tile = tiles.find({rx, y});
					if (tile != std::end(tiles)) {
						if (tile->second == Type::Clay)
							break;
						tile->second = Type::WetSand;
					}
				}
			}
		}
		return (l || r);
	}
	tiles[current] = Type::WetSand;
	return true;
}

int main()
{
	auto input{readInput()};
	auto b = bounds(input);

	flow({500, 0}, input, b);

	/* for (int y{b.second.first}; y <= b.second.second; ++y) { */
	for (int y{1}; y <= b.second.second; ++y) {
		for (int x{b.first.first}; x <= b.first.second; ++x) {
			const auto& tile{input.find({x, y})};
			if (tile != std::end(input)) {
				if (tile->second == Type::Water)
					std::cout << '~';
				else if (tile->second == Type::WetSand)
					std::cout << '|';
				else
					std::cout << '#';
			}
			else {
				std::cout << '.';
			}
		}
		std::cout << '\n';
	}

	std::cout << "part 1: " << std::count_if(std::begin(input), std::end(input), [&b](const auto& tile) {
													const auto& [x, y] = tile.first;
													return (tile.second != Type::Clay && y >= b.second.first && y <= b.second.second);
												 })<< '\n';
	std::cout << "part 2: " << std::count_if(std::begin(input), std::end(input), [&b](const auto& tile) {
													const auto& [x, y] = tile.first;
													return (tile.second == Type::Water && y >= b.second.first && y <= b.second.second);
												 })<< '\n';
}
