#include <iostream>
#include <unordered_set>
#include <vector>

int main()
{
	std::vector<int> changes;
	int frequency{0};
	int change;

	while (std::cin >> change) {
		frequency += change;
		changes.push_back(change);
	}

	std::cout << "part 1: " << frequency << '\n';

	std::unordered_set<int> seen = {0};
	frequency = 0;
	while (true) {
		for (int change : changes) {
			frequency += change;
			if (seen.find(frequency) != std::end(seen)) {
				std::cout << "part 2: " << frequency << '\n';
				return 0;
			}
			seen.emplace(frequency);
		}
	}
}
