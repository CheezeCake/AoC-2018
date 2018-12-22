#include <array>
#include <algorithm>
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

std::array<Tools, 2> regionTools(Type region)
{
	if (region == Type::Rocky)
		return {Tools::Torch, Tools::ClimbingGear};
	if (region == Type::Wet)
		return {Tools::Neither, Tools::ClimbingGear};
	return {Tools::Neither, Tools::Torch};
}

std::vector<std::tuple<int, int>> neighbors(const Grid& grid, int x, int y)
{
	const std::array<const std::tuple<int, int>, 4> directions = {{
		{-1, 0},
		{0, -1},
		{1, 0},
		{0, 1},
	}};

	std::vector<std::tuple<int, int>> result;
	result.reserve(directions.size());

	for (const auto& dir : directions) {
		auto [dx, dy] = dir;
		auto nx = x + dx;
		auto ny = y + dy;
		if (valid(grid, nx, ny))
			result.push_back({nx, ny});
	}

	return result;
}

int fastestPath(const Grid& grid, unsigned targetX, unsigned targetY)
{
	std::priority_queue<TimeState, std::vector<TimeState>, std::greater<TimeState>> q;
	std::set<State> seen;

	q.emplace(0, 0, 0, Tools::Torch);

	while (!q.empty()) {
		const auto [time, x, y, tool] = q.top();
		q.pop();

		if (x == targetX && y == targetY && tool == Tools::Torch)
			return time;
		if (seen.find({x, y, tool}) != std::end(seen))
			continue;
		seen.emplace(x, y, tool);

		for (auto newTool : regionTools(static_cast<Type>(grid[y][x]))) {
			if (newTool != tool)
				q.emplace(time + 7, x, y, newTool);
		}

		for (const auto& n : neighbors(grid, x, y)) {
			auto [nx, ny] = n;
			const auto validTools = regionTools(static_cast<Type>(grid[ny][nx]));
			if (std::find(std::begin(validTools), std::end(validTools), tool) != std::end(validTools))
				q.emplace(time + 1, nx, ny, tool);
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

	const auto grid = createGrid(depth, targetX, targetY);
	std::cout << "part 1: " << riskLevel(grid, targetX, targetY) << '\n';
	std::cout << "part 2: " << fastestPath(grid, targetX, targetY) << '\n';
}
