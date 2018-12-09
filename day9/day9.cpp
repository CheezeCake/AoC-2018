#include <algorithm>
#include <cstdio>
#include <iostream>
#include <list>
#include <vector>

unsigned play(unsigned players, unsigned lastMarble)
{
	std::vector<unsigned> scores(players);
	int currentPlayer = 0;
	unsigned maxScore = 0;

	std::list<int> circle;
	circle.push_back(0);
	auto currentMarble = circle.begin();

	for (unsigned marble = 1; marble <= lastMarble; ++marble) {
		if (marble % 23 == 0) {
			scores[currentPlayer] += marble;
			auto toRemove = currentMarble;
			for (int i = 0; i < 7; ++i) {
				if (toRemove == std::begin(circle))
					toRemove = std::end(circle);
				--toRemove;
			}
			scores[currentPlayer] += *toRemove;
			maxScore = std::max(maxScore, scores[currentPlayer]);

			currentMarble = std::next(toRemove);
			if (currentMarble == std::end(circle))
				currentMarble = std::begin(circle);

			circle.erase(toRemove);
		}
		else {
			auto nextPos = currentMarble;
			for (int i = 0; i < 2; ++i) {
				++nextPos;
				if (nextPos == std::end(circle))
					nextPos = std::begin(circle);
			}
			if (nextPos == std::begin(circle))
				nextPos = std::end(circle);

			currentMarble = circle.insert(nextPos, marble);
		}

		currentPlayer = (currentPlayer + 1) % players;
	}

	return maxScore;
}

int main()
{
	unsigned players;
	unsigned lastMarble;
	scanf("%d players; last marble is worth %d points", &players, &lastMarble);

	std::cout << "part 1: " << play(players, lastMarble) << '\n';
	std::cout << "part 2: " << play(players, lastMarble * 100) << '\n';
}
