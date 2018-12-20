#include <algorithm>
#include <iostream>
#include <map>
#include <stack>

struct Point
{
	int x;
	int y;
};

bool operator<(const Point& lhs, const Point& rhs)
{
	return (lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x));
}

Point operator+(const Point& lhs, const Point& rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y};
}

int main()
{
	static const std::map<char, const Point> directions = {
		{'N', {0, -1}},
		{'S', {0, 1}},
		{'W', {-1, 0}},
		{'E', {1, 0}},
	};

	Point currentPos{0, 0};
	std::stack<Point> positions;
	std::map<Point, int> distances = {{currentPos, 0}};

	char c;
	while (std::cin >> c) {
		if (c == '^' || c == '$')
			continue;
		if (c == '(') {
			positions.push(currentPos);
		}
		else if (c == ')') {
			currentPos = positions.top();
			positions.pop();
		}
		else if (c == '|') {
			currentPos = positions.top();
		}
		else {
			auto nextPosition{currentPos + directions.at(c)};
			auto dist{distances[currentPos] + 1};
			if (distances.find(nextPosition) == std::end(distances) || dist < distances.at(nextPosition))
				distances[nextPosition] = dist;
			currentPos = nextPosition;
		}
	}

	std::cout << "part 1: " << std::max_element(std::begin(distances), std::end(distances), [](const auto& lhs, const auto& rhs) { return (lhs.second < rhs.second); })->second << '\n';
	std::cout << "part 2: " << std::count_if(std::begin(distances), std::end(distances), [](const auto& e) { return (e.second >= 1000); }) << '\n';
}
