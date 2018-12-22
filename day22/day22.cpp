#include <array>
#include <cstdio>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <tuple>
#include <vector>

using Grid = std::vector<std::vector<int>>;

constexpr unsigned BEYOND_TARGET{100};

Grid createGrid(unsigned depth, unsigned targetX, unsigned targetY)
{
	Grid grid;
	std::vector<unsigned> prevLineErosionLevel(targetX + BEYOND_TARGET, 0);
	grid.resize(targetY + BEYOND_TARGET);

	for (unsigned y{0}; y < grid.size(); ++y) {
		std::vector<unsigned> curLineErosionLevel(targetX + BEYOND_TARGET, 0);
		grid[y].resize(targetX + BEYOND_TARGET);

		for (unsigned x{0}; x < grid[y].size(); ++x) {
			unsigned geoIndex;
			if ((x == 0 && y == 0) || (x == targetX && y == targetY))
				geoIndex = 0;
			else if (y == 0)
				geoIndex = x * 16807;
			else if (x == 0)
				geoIndex = y * 48271;
			else
				geoIndex = curLineErosionLevel[x - 1] * prevLineErosionLevel[x];

			curLineErosionLevel[x] = (geoIndex + depth) % 20183;
			grid[y][x] = curLineErosionLevel[x] % 3;
		}

		prevLineErosionLevel = curLineErosionLevel;
	}

	return grid;
}

unsigned riskLevel(const Grid& grid, unsigned targetX, unsigned targetY)
{
	unsigned risk{0};
	for (unsigned y{0}; y <= targetY; ++y)
		risk += std::accumulate(std::begin(grid[y]), std::begin(grid[y]) + targetX + 1, 0);
	return risk;
}

enum Type { Rocky, Wet, Narrow };
enum Tools { Neither, Torch, ClimbingGear };

using TimeState = std::tuple<int, int, int, int>; // time, x, y, tool
using State = std::tuple<int, int, int>; // x, y, tool

inline bool valid(const Grid& grid, int x, int y)
{
	return (y >= 0 && y < grid.size() && x >= 0 && x < grid[y].size());
}

int fastestPath(const Grid& grid, unsigned targetX, unsigned targetY)
{
	std::priority_queue<TimeState, std::vector<TimeState>, std::greater<TimeState>> q;
	std::set<State> seen;

	const std::array<const std::tuple<int, int>, 4> directions = {{
		{-1, 0},
		{0, -1},
		{1, 0},
		{0, 1},
	}};

	q.emplace(0, 0, 0, Tools::Torch);

	while (!q.empty()) {
		const auto [time, x, y, tool] = q.top();
		q.pop();

		if (x == targetX && y == targetY && tool == Tools::Torch)
			return time;
		if (seen.find({x, y, tool}) != std::end(seen))
			continue;
		seen.emplace(x, y, tool);

		if (tool != Tools::Neither && (grid[y][x] == Type::Wet || grid[y][x] == Type::Narrow))
			q.emplace(time + 7, x, y, Tools::Neither);
		if (tool != Tools::Torch  && (grid[y][x] == Type::Rocky || grid[y][x] == Type::Narrow))
			q.emplace(time + 7, x, y, Tools::Torch);
		if (tool != Tools::ClimbingGear  && (grid[y][x] == Type::Rocky || grid[y][x] == Type::Wet))
			q.emplace(time + 7, x, y, Tools::ClimbingGear);

		for (const auto& dir : directions) {
			auto [dx, dy] = dir;
			auto nx = x + dx;
			auto ny = y + dy;
			if (valid(grid, nx, ny)) {
				if ((grid[ny][nx] == Type::Rocky && tool != Tools::Neither) ||
					(grid[ny][nx] == Type::Wet && tool != Tools::Torch) ||
					(grid[ny][nx] == Type::Narrow && tool != Tools::ClimbingGear))
				{
					q.emplace(time + 1, nx, ny, tool);
				}
			}
		}
	}

	return -1;
}

int main()
{
	unsigned depth;
	unsigned targetX;
	unsigned targetY;
	std::scanf("depth: %u\n", &depth);
	std::scanf("target: %u,%u\n", &targetX, &targetY);

	auto grid = createGrid(depth, targetX, targetY);
	std::cout << "part 1: " << riskLevel(grid, targetX, targetY) << '\n';
	std::cout << "part 2: " << fastestPath(grid, targetX, targetY) << '\n';
}
