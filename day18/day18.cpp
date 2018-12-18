#include <array>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

class Area
{
	std::vector<std::string> mMap;

	bool valid(int x, int y) const
	{
		return (y >= 0 && y < mMap.size() && x >= 0 && x < mMap[y].size());
	}

	// threes, lumberyard
	std::tuple<int, int> adjacentCount(int x, int y) const
	{
		static const std::array<const std::tuple<int, int>, 8> directions = {{
			{-1, 0}, // left
			{-1, -1}, // top-left
			{0, -1}, // top
			{1, -1}, // top-right
			{1, 0}, // right
			{1, 1}, // bottom-right
			{0, 1}, // bottom
			{-1, 1}, // bottom-left
		}};

		int threes{0};
		int lumber{0};

		for (const auto& dir : directions) {
			auto [dx, dy] = dir;
			int ax{x + dx};
			int ay{y + dy};
			if (valid(ax, ay)) {
				auto acre{mMap[ay][ax]};
				if (acre == '|')
					++threes;
				else if (acre == '#')
					++lumber;
			}
		}

		return {threes, lumber};
	}

public:
	Area(const std::vector<std::string> map) :
		mMap(map)
	{
	}

	void advance()
	{
		auto map{mMap};

		for (int y{0}; y < mMap.size(); ++y) {
			for (int x{0}; x < mMap.size(); ++x) {
				auto acre{mMap[y][x]};
				auto [adjThrees, adjLumber] = adjacentCount(x, y);
				switch (acre) {
					case '.':
						if (adjThrees >= 3)
							map[y][x] = '|';
						break;
					case '|':
						if (adjLumber >= 3)
							map[y][x] = '#';
						break;
					case '#':
						if (adjLumber == 0 || adjThrees == 0)
							map[y][x] = '.';
						break;
				}
			}
		}

		mMap = map;
	}

	int resourceValue() const
	{
		int threes{0};
		int lumber{0};
		for (const auto& row : mMap) {
			for (const auto& acre : row) {
				if (acre == '|')
					++threes;
				else if (acre == '#')
					++lumber;
			}
		}
		return (threes * lumber);
	}

	void print() const
	{
		for (int y{0}; y < mMap.size(); ++y) {
			for (int x{0}; x < mMap.size(); ++x) {
				std::cout << mMap[y][x];
			}
			std::cout << '\n';
		}
	}
};

int main()
{
	std::vector<std::string> input;
	std::string line;
	while (std::getline(std::cin, line))
		input.push_back(line);

	Area a(input);
	unsigned time{1};
	for (; time <= 10; ++time)
		a.advance();
	std::cout << "part 1: " << a.resourceValue() << '\n';

	std::unordered_map<int, int> resourceValues;
	int repeatedValue{0};
	int consecutiveRepeats{0};
	while (true) {
		a.advance();
		int rv{a.resourceValue()};
		if (resourceValues.find(rv) != std::end(resourceValues)) {
			repeatedValue = rv;
			if (consecutiveRepeats++ == 50)
				break;
		}
		else {
			consecutiveRepeats = 0;
		}
		resourceValues[rv] = time;
		++time;
	}

	unsigned cycle{time - resourceValues.at(repeatedValue)};
	std::vector<int> values(cycle, 0);
	values[0] = repeatedValue;
	for (int i = 1; i < cycle; ++i) {
		a.advance();
		values[i] = a.resourceValue();
	}
	unsigned diff{1'000'000'000 - time};

	std::cout << "part 2: " << values[diff % cycle] << '\n';
}
