#include <iostream>
#include <string>
#include <tuple>
#include <vector>

constexpr int HEIGHT = 300;
constexpr int WIDTH = 300;

static inline bool topLeftOfSquare(int x, int y, int squareSize)
{
	return (x <= WIDTH - squareSize - 1 && y <= HEIGHT - squareSize - 1);
}

using Grid = std::vector<std::vector<int>>;

static int squareTotalPower(int x, int y, int squareSize, const Grid& grid)
{
	int totalPower = 0;
	for (int i = y; i < y + squareSize; ++i) {
		for (int j = x; j < x + squareSize; ++j)
			totalPower += grid[i][j];
	}
	return totalPower;
}

// tuple<x, y, power>
static std::tuple<int, int, int> findMaxPowerSquare(int serial, int size)
{
	Grid grid{HEIGHT + 1, std::vector<int>(HEIGHT + 1, 0)};
	std::tuple<int, int, int> maxSquare;
	int maxSquarePower = 0;

	for (int y = HEIGHT; y > 0; --y) {
		for (int x = WIDTH; x > 0; --x) {
			int rackId = x + 10;
			int power = ((rackId * y) + serial) * rackId;
			power = ((power / 100) % 10) - 5;
			grid[y][x] = power;
			if (topLeftOfSquare(x, y, size)) {
				int squarePower = squareTotalPower(x, y, size, grid);
				if (squarePower > maxSquarePower) {
					maxSquarePower = squarePower;
					maxSquare = {x, y, squarePower};
				}
			}
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

	int maxX, maxY, maxSize;
	int maxPower = 0;
	for (int squareSize = 1; squareSize <= 300; ++squareSize) {
		auto [x, y, power] = findMaxPowerSquare(serial, squareSize);
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
