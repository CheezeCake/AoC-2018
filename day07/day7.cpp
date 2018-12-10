#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

struct Step
{
	int deps;
	std::vector<char> children;
};

std::string solvePart1(std::map<char, Step> steps)
{
	std::string order;
	order.reserve(steps.size());

	while (!steps.empty()) {
		auto next = std::find_if(std::begin(steps), std::end(steps),
								 [](const auto& it) { return (it.second.deps == 0); });
		if (next == std::end(steps)) {
			std::cerr << "dependency cycle\n";
			return {};
		}

		order.push_back(next->first);
		for (char child : next->second.children)
			--steps[child].deps;
		steps.erase(next);
	}

	return order;
}

struct Worker
{
	Step current;
	char currentName;
	unsigned finish;
};

int solvePart2(std::map<char, Step> steps, unsigned nrWorkers)
{
	unsigned time;
	unsigned finish{0};
	std::vector<Worker> workers{nrWorkers};

	for (time = 0; !steps.empty(); ++time) {
		for (auto& w : workers) {
			if (w.currentName != 0 && w.finish == time) {
				/* std::cout << '[' << time << "] end: " << w.currentName << '\n'; */
				for (char child : w.current.children)
					--steps[child].deps;
				w.currentName = 0;
				w.finish = 0;
			}

			if (w.currentName == 0) {
				auto next = std::find_if(std::begin(steps), std::end(steps),
										 [](const auto& it) { return (it.second.deps == 0); });
				if (next != std::end(steps)) {
					w.current = next->second;
					w.currentName = next->first;
					/* w.finish = time + next->first - 'A' + 1; */
					w.finish = time + next->first - 'A' + 1 + 60;
					finish = std::max(finish, w.finish);
					steps.erase(next);
					/* std::cout << '[' << time << "] assign: " << w.currentName << " -> " << w.finish << '\n'; */
				}
			}
		}
	}

	/* std::cout << "time="<< time << '\n'; */
	/* std::cout << "finish="<< finish << '\n'; */

	return std::max(time, finish);
}

int main()
{
	std::map<char, Step> steps;

	char dep;
	char step;
	while (scanf("Step %c must be finished before step %c can begin.\n", &dep, &step) != EOF) {
		++steps[step].deps;
		steps[dep].children.push_back(step);
	}

	std::cout << "part 1: " << solvePart1(steps) << '\n';
	std::cout << "part 2: " << solvePart2(steps, 5) << '\n';
	/* std::cout << "part 2: " << solvePart2(steps, 2) << '\n'; */
}
