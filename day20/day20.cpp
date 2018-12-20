#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <utility>

struct Point
{
	int x;
	int y;
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
	os << '(' << p.x << ", " << p.y << ')';
	return os;
}

bool operator<(const Point& lhs, const Point& rhs)
{
	return (lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x));
}

Point operator+(const Point& lhs, const Point& rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y};
}

enum class Type { Room, Door };

void print(const Point& start, const std::map<Point, Type>& map)
{
	int minX = std::numeric_limits<int>::max();
	int maxX = std::numeric_limits<int>::min();
	int minY = std::numeric_limits<int>::max();
	int maxY = std::numeric_limits<int>::min();
	for (const auto& [p, t] : map) {
		minX = std::min(minX, p.x);
		maxX = std::max(maxX, p.x);

		minY = std::min(minY, p.y);
		maxY = std::max(maxY, p.y);
	}

	for (int y{minY}; y <= maxY; ++y) {
		for (int x{minX}; x <= maxX; ++x) {
			if (map.find({x, y}) == std::end(map)) {
				std::cout << '#';
			}
			else if (x == 0 && y == 0 ){
				std::cout << 'X';
			}
			else {
				switch (map.at({x, y})) {
					case Type::Room:
						std::cout << '.';
						break;
					case Type::Door:
						std::cout << '|';
						break;
				}
			}
		}
		std::cout << '\n';
	}
}

static const std::map<char, const Point> directions = {
	{'N', {0, -1}},
	{'S', {0, 1}},
	{'W', {-1, 0}},
	{'E', {1, 0}},
};

int walk(int start, const std::string& s, const Point& current, std::map<Point, Type>& map)
{
	int i{start};
	Point cur{current};

	while (i < s.length()) {
		if (s[i] == ')' || s[i] == '$')
			return i;

		if (s[i] == '(' || s[i] == '^') {
			int end = walk(i + 1, s, cur, map);
			i = end + 1;
		}
		else if (s[i] == '|') {
			cur = current;
			++i;
		}
		else {
			const auto& dir{directions.at(s[i])};
			auto door{cur + dir};
			auto nextRoom{door + dir};

			map[door] = Type::Door;
			map[nextRoom] = Type::Room;

			cur = nextRoom;
			++i;
		}
	}

	return i;
}

std::pair<Point, int> findMaxPath(const std::map<Point, Type>& map, const Point& start, std::set<Point>& moreThan1000)
{
	std::pair<Point, int> maxDist = {{0, 0}, std::numeric_limits<int>::min()};

	std::set<Point> visited;
	std::queue<std::pair<Point, int>> q;

	q.emplace(start, 0);
	visited.emplace(start);

	while (!q.empty()) {
		auto [p, dist] = q.front();
		q.pop();

		if (dist >= 1000)
			moreThan1000.emplace(p);

		if (dist > maxDist.second)
			maxDist = {p, dist};

		for (const auto& [d, dir] : directions) {
			const auto door{p + dir};
			const auto nextRoom{door + dir};
			if (map.find(door) != std::end(map) && visited.find(nextRoom) == std::end(visited)) {
				q.emplace(nextRoom, dist + 1);
				visited.emplace(nextRoom);
			}
		}
	}

	return maxDist;
}

int main()
{
	std::string input;
	std::getline(std::cin, input);

	const Point start{0, 0};
	std::map<Point, Type> map = {{start, Type::Room}};
	walk(0, input, {0, 0}, map);

	/* print(start, map); */

	std::set<Point> moreThan1000;
	auto [maxP, maxDist] = findMaxPath(map, start, moreThan1000);
	std::cout << "part 1 : " << maxDist << '\n';
	std::cout << "part 2: " << moreThan1000.size() << '\n';
}
