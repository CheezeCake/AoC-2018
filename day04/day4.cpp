#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

class SleepLog
{
	int minutesSlept;
	int minuteMostAsleep;
	std::unordered_map<int, int> minuteCount;

	public:
	SleepLog() : minutesSlept{0}, minuteMostAsleep{0}
	{}

	int getMinutesSlept() const
	{
		return minutesSlept;
	}

	int getMinuteMostAsleep() const
	{
		return minuteMostAsleep;
	}

	int getMinuteMostAsleepCount() const
	{
		return minuteCount.at(minuteMostAsleep);
	}

	void addSleepTime(int startingMinute, int endingMinute)
	{
		minutesSlept += endingMinute - startingMinute;

		for (int minute = startingMinute; minute < endingMinute; ++minute) {
			if (++minuteCount[minute] > minuteCount[minuteMostAsleep])
				minuteMostAsleep = minute;
		}
	}
};

int main()
{
	std::vector<std::string> inputs;
	std::string line;
	while (std::getline(std::cin, line))
		inputs.push_back(line);
	std::sort(std::begin(inputs), std::end(inputs));

	int guardMostAsleep = -1;
	int currentGuard = 0;
	int sleepStart;
	std::unordered_map<int, SleepLog> log;
	for (const auto& input : inputs) {
		std::stringstream ss{input};
		std::string firstWord;
		char c;
		int dummy;
		int minute;

		ss >> c >> dummy;
		ss >> c >> dummy;
		ss >> c >> dummy;
		ss >> c >> dummy;
		ss >> c >> minute;
		ss >> c;
		ss.ignore();

		ss >> firstWord;
		if (firstWord == "Guard") {
			ss >> c >> currentGuard;
		}
		else if (firstWord == "falls") {
			sleepStart = minute;
		}
		else {
			log[currentGuard].addSleepTime(sleepStart, minute);
			if (guardMostAsleep == -1 ||
				log[currentGuard].getMinutesSlept() > log[guardMostAsleep].getMinutesSlept())
				guardMostAsleep = currentGuard;
		}
	}

	std::cout << "part 1: " << guardMostAsleep * log[guardMostAsleep].getMinuteMostAsleep() << '\n';

	int guard = 0;
	int minuteCount = 0;
	for (const auto& entry : log) {
		int cnt = entry.second.getMinuteMostAsleepCount();
		if (cnt > minuteCount) {
			guard = entry.first;
			minuteCount = cnt;
		}
	}

	std::cout << "part 2: " << guard * log[guard].getMinuteMostAsleep() << '\n';

	return 0;
}
