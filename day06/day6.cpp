#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <array>
#include <limits>
#include <cstdlib>
#include <cmath>

struct Point
{
	int x;
	int y;

	int area = 0;
	int pointsOnEdge = 0;
};

bool operator<(const Point& lhs, const Point& rhs)
{
	if (lhs.pointsOnEdge == 0 && rhs.pointsOnEdge > 0)
		return false;
	if (lhs.pointsOnEdge > 0 && rhs.pointsOnEdge == 0)
		return true;
	return (lhs.area < rhs.area);
}

std::vector<Point> readInput(int& maxX, int& maxY)
{
	std::vector<Point> points;

	maxX = 0;
	maxY = 0;
	int x, y;
	while (scanf("%d, %d\n", &x, &y) != EOF) {
		points.push_back({x, y});

		maxX = std::max(maxX, x);
		maxY = std::max(maxY, y);
	}

	return points;
}

static inline bool onEdge(int x, int y, int maxX, int maxY)
{
	return (x == 0 || x == maxX || y == 0 || y == maxY);
}

int main()
{
	int maxX, maxY;
	auto points = readInput(maxX, maxY);

	Point maxAreaPoint;
	int regionSize = 0;

	for (int y = 0; y <= maxY; ++y) {
		for (int x = 0; x <= maxX; ++x) {
			int minP;
			int minDist = std::numeric_limits<int>::max();
			int distSum = 0;
			bool equal = false;

			for (std::size_t p = 0; p < points.size(); ++p) {
				const int dist = std::abs(x - points[p].x) + std::abs(y - points[p].y);
				distSum += dist;

				if (dist < minDist) {
					minP = p;
					minDist = dist;
					equal = false;
				}
				else if (dist == minDist) {
					equal = true;
				}
			}
			if (!equal) {
				++points[minP].area;
				if (onEdge(x, y, maxX, maxY))
					++points[minP].pointsOnEdge;
			}

			maxAreaPoint = std::max(maxAreaPoint, points[minP]);

			if (distSum < 10'000)
				++regionSize;
		}
	}

	std::cout << "part 1: " << maxAreaPoint.area << '\n';
	std::cout << "part 2: " << regionSize << '\n';
}
