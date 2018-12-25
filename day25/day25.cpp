#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <vector>

using Point = std::tuple<int, int, int, int>;

unsigned distance(const Point& p1, const Point& p2)
{
	auto [a1, b1, c1, d1] = p1;
	auto [a2, b2, c2, d2] = p2;

	return (std::abs(a1 - a2) + std::abs(b1 - b2) +
			std::abs(c1 - c2) + std::abs(d1 - d2));
}

int main()
{
	std::vector<Point> points;

	int a, b, c, d;
	while (std::scanf("%d, %d, %d, %d\n", &a, &b, &c, &d) != EOF)
		points.push_back({a, b, c, d});

	std::vector<std::vector<Point>> constellations;

	for (const auto& p : points) {
		std::vector<int> belongsTo;
		for (std::size_t c = 0; c < constellations.size(); ++c) {
			if (std::find_if(std::begin(constellations[c]), std::end(constellations[c]),
							 [&p](const auto& cp) { return (distance(p, cp) <= 3); })
				!= std::end(constellations[c]))
				belongsTo.push_back(c);
		}

		if (belongsTo.size() == 0) {
			constellations.push_back({p});
			continue;
		}

		int first = belongsTo[0];
		for (std::size_t i = 1; i < belongsTo.size(); ++i) {
			int cur = belongsTo[i];
			constellations[first].reserve(constellations[first].size() + constellations[cur].size());
			for (const auto& cp : constellations[cur])
				constellations[first].push_back(cp);
			constellations[cur].clear();
		}

		constellations[first].push_back(p);
	}

	std::cout << "part 1: " << std::count_if(std::begin(constellations), std::end(constellations), [](const auto& c) { return (c.size() > 0); }) << '\n';
}
