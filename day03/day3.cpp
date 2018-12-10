#include <iostream>
#include <string>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <functional>

struct Point
{
	int x;
	int y;
};

template<>
struct std::hash<Point>
{
	std::size_t operator()(const Point& p) const
	{
		return std::hash<std::string>{}(std::to_string(p.x) + ',' + std::to_string(p.y));
	}
};

template<>
struct std::equal_to<Point>
{
	constexpr bool operator()(const Point& lhs, const Point& rhs) const
	{
		return (lhs.x == rhs.x && lhs.y == rhs.y);
	}
};

struct Rect {
	int id;
	Point tl;
	int w;
	int h;
};

int main()
{
	std::vector<Rect> rects;
	std::unordered_map<Point, int> points;
	int count = 0;

	Rect r;
	while (scanf("#%d @ %d,%d: %dx%d\n", &r.id, &r.tl.x, &r.tl.y, &r.w, &r.h) != EOF) {
		rects.push_back(r);

		for (int w = 0; w < r.w; ++w) {
			for (int h = 0; h < r.h; ++h) {
				struct Point pt = {r.tl.x + w, r.tl.y + h};

				if (points[pt]++ == 1)
					++count;
			}
		}
	}

	std::cout << "part 1: " << count << '\n';

	for (Rect r : rects) {
		bool overlap = false;

		for (int w = 0; w < r.w && !overlap; ++w) {
			for (int h = 0; h < r.h && !overlap; ++h) {
				struct Point pt = {r.tl.x + w, r.tl.y + h};

				if (points[pt] > 1)
					overlap = true;
			}
		}

		if (!overlap) {
			std::cout << "part 2: " << r.id << '\n';
			break;
		}
	}
}
