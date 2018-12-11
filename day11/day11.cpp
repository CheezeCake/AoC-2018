#include <iostream>
#include <string>
#include <tuple>
#include <vector>

constexpr int HEIGHT = 300;
constexpr int WIDTH = 300;

using Grid = std::vector<std::vector<int>>;

static void computeGrid(Grid& grid, int serial)
{
	for (int y = 1; y <= HEIGHT; ++y) {
		for (int x = 1; x <= WIDTH; ++x) {
			int rackId = x + 10;
			int power = ((rackId * y) + serial) * rackId;
			power = ((power / 100) % 10) - 5;
			grid[y][x] = power;
		}
	}
}

static int colSum(int x, int y, int size, const Grid& grid)
{
	int sum = 0;
	for (int i = 0; i < size; ++i)
		sum += grid[y + i][x];
	return sum;
}

static inline void shiftVectorRight(std::vector<int>& v)
{
	for (int i = v.size() - 1; i > 0; --i)
		v[i] = v[i - 1];
}

// tuple<x, y, power>
static std::tuple<int, int, int> findMaxPowerSquare(int serial, int size, const Grid& grid)
{
	std::tuple<int, int, int> maxSquare{0, 0, 0};

	for (int y = HEIGHT - size + 1; y > 0; --y) {
		std::vector<int> colSums(size, 0);
		int x = WIDTH - size + 1;
		int squarePower = 0;

		for (int j = 1; j < size; ++j) {
			colSums[j - 1] = colSum(x + j, y, size, grid);
			squarePower += colSums[j - 1];
		}

		for (; x > 0; --x) {
			if (x + size <= WIDTH) {
				squarePower -= colSums[size - 1];
				colSums[size - 1] = 0;
			}
			shiftVectorRight(colSums);
			colSums[0] = colSum(x, y, size, grid);
			squarePower += colSums[0];

			if (squarePower > std::get<2>(maxSquare))
				maxSquare = {x, y, squarePower};
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
	Grid grid{HEIGHT + 1, std::vector<int>(HEIGHT + 1, 0)};
	computeGrid(grid, serial);

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
