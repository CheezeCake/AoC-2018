#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

struct Point
{
	int x;
	int y;
	int z;

	int distance(const Point& p) const
	{
		return (std::abs(x - p.x) + std::abs(y - p.y) + std::abs(z - p.z));
	}
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
	os << '(' << p.x << ", " << p.y << ", " << p.z << ')';
	return os;
}

Point operator+(const Point& lhs, const Point& rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

struct Nanobot
{
	Point pos;
	int r;

	bool inRange(const Point& p) const
	{
		return (pos.distance(p) <= r);
	}

	bool inRange(const Nanobot& b) const
	{
		return inRange(b.pos);
	}
};

int main()
{
	std::vector<Nanobot> nanobots;

	Nanobot n;
	while (std::scanf("pos=<%d, %d, %d>, r=%d\n", &n.pos.x, &n.pos.y, &n.pos.z, &n.r) != EOF)
		nanobots.push_back(n);

	const auto& m = *std::max_element(std::begin(nanobots), std::end(nanobots), [](const auto& lhs, const auto& rhs) { return (lhs.r < rhs.r); });
	std::cout << "part 1: " << std::count_if(std::begin(nanobots), std::end(nanobots), [&m](const auto& b) { return m.inRange(b); }) << '\n';

	int maxBots{0};
	Point best{0, 0, 0};
	for (int factor{1'000'000}; factor >= 1; factor /= 10) {
	/* for (int factor{1}; factor >= 1; factor /= 10) { */
		maxBots = 0;

		best.x *= 10;
		best.y *= 10;
		best.z *= 10;

		auto bots = nanobots;
		for (auto& b : bots) {
			b.pos.x /= factor;
			b.pos.y /= factor;
			b.pos.z /= factor;
			b.r /= factor;
		}

		Point newBest;
		constexpr int range{50};
		for (int dx{-range}; dx <= range; ++dx) {
			for (int dy{-range}; dy <= range; ++dy) {
				for (int dz{-range}; dz <= range; ++dz) {
					int cnt{0};
					Point p = best + Point{dx, dy, dz};

					for (const auto& b : bots) {
						if (b.inRange(p)) {
							/* std::cout << p << " is in range of " << b.pos << " r = " << b.r << '\n'; */
							++cnt;
						}
					}

					if (cnt > maxBots) {
						/* std::cout << p << ", cnt=" << cnt << '\n'; */
						maxBots = cnt;
						newBest = p;
					}
				}
			}
		}
		best = newBest;
	}

	/* std::cout << "max bots: " << maxBots << " for " << best << '\n'; */
	std::cout << best.distance({0, 0, 0}) << '\n';

}
