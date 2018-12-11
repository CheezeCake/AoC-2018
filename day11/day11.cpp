#include <array>
#include <iostream>
#include <string>
#include <tuple>

constexpr int HEIGHT = 300;
constexpr int WIDTH = 300;

using Grid = std::array<std::array<int, WIDTH + 1>, HEIGHT + 1>;

static void computeAreas(Grid& grid, int serial)
{
	for (int y = 1; y <= HEIGHT; ++y) {
		for (int x = 1; x <= WIDTH; ++x) {
			int rackId = x + 10;
			int power = ((rackId * y) + serial) * rackId;
			power = ((power / 100) % 10) - 5;
			grid[y][x] = power + grid[y][x - 1] + grid[y - 1][x] - grid[y - 1][x - 1];
		}
	}
}

// tuple<x, y, power>
static std::tuple<int, int, int> findMaxPowerSquare(int serial, int size, const Grid& grid)
{
	std::tuple<int, int, int> maxSquare{0, 0, 0};

	for (int y = 1; y <= HEIGHT - size; ++y) {
		for (int x = 1; x <= WIDTH - size; ++x) {
			int squarePower = grid[y][x] + grid[y+size][x+size] -
				grid[y + size][x] - grid[y][x + size];

			if (squarePower > std::get<2>(maxSquare))
				maxSquare = {x + 1, y + 1, squarePower};
		}
	}

	return maxSquare;
}

int main(int argc, char** argv)
{
	if (argc <= 1) {
		std::cerr << "usage: " << argv[0] << " input\n";
		return 1;
	}

	int serial = std::stoi(argv[1]);
	Grid grid{{{0}}};
	computeAreas(grid, serial);

	int maxX, maxY, maxSize;
	int maxPower = 0;
	for (int squareSize = 1; squareSize <= WIDTH; ++squareSize) {
		auto [x, y, power] = findMaxPowerSquare(serial, squareSize, grid);
		if (power > maxPower) {
			maxPower = power;
			maxX = x;
			maxY = y;
			maxSize = squareSize;
		}
		if (squareSize == 3)
			std::cout << "part 1: " << x << ',' << y << '\n';
	}

	std::cout << "part 2: " << maxX << ',' << maxY << ',' << maxSize << '\n';
}
