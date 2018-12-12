#include <iostream>
#include <string>
#include <unordered_map>
#include <future>
#include <tuple>


std::string getLLCRR(int pot, const std::string& state)
{
	if (pot == 0)
		return ".." + state.substr(0, 3);
	if (pot == 1)
		return "." + state.substr(0, 4);
	if (pot == state.length() - 1)
		return state.substr(pot - 2, 3) + "..";
	if (pot == state.length() - 2)
		return state.substr(pot - 2, 4) + ".";
	return state.substr(pot - 2, 5);
}

std::string run(const std::string& state, std::size_t start, std::size_t end,
				const std::unordered_map<std::string, char>& rules)
{
	std::string newState;
	newState.reserve(end - start);

	for (std::size_t pot = start; pot < end; ++pot) {
		std::string llcrr{getLLCRR(pot, state)};
		if (rules.find(llcrr) == std::end(rules)) {
			newState.push_back('.');
		}
		else {
			newState.push_back(rules.at(llcrr));
		}
	}

	return newState;
}
std::string runAsync(const std::string& state, const std::unordered_map<std::string, char>& rules)
{
	const std::size_t half = state.length() / 2;
	auto f0 = std::async(run, state, 0, half, rules);
	auto f1 = std::async(run, state, half, state.length(), rules);
	return f0.get()+f1.get();
}

class State
{
	std::string mState;
	int mStartId;
	std::unordered_map<std::string, char> mRules;

public:
	State(const std::string& initialState, const std::unordered_map<std::string, char>& rules) :
		mState{initialState},
		mStartId{0},
		mRules{rules}
	{
	}

	bool advance()
	{
		bool repeating{false};
		int newStartId{mStartId};
		std::string newState;
		newState.reserve(mState.length()+4);

		char first = mRules.at("...."+mState.substr(0, 1));
		char second = mRules.at("..."+mState.substr(0, 2));
		if (first == '#') {
			newState.push_back(first);
			newState.push_back(second);
			newStartId -= 2;
		}
		else if (second == '#') {
			newState.push_back(second);
			--newStartId;
		}

		newState.append(runAsync(mState, mRules));

		first = mRules.at(mState.substr(mState.length() - 1, 1)+"....");
		second = mRules.at(mState.substr(mState.length() - 2, 2)+"...");
		if (first == '#') {
			newState.push_back(second);
			newState.push_back(first);
		}
		else if (second == '#') {
			newState.push_back(second);
		}

		auto firstPlant = newState.find('#');
		if (firstPlant != std::string::npos) {
			newState = newState.substr(firstPlant);
			newStartId+= firstPlant;
		}

		auto lastPlant = newState.rfind('#');
		if (lastPlant != std::string::npos)
			newState.resize(lastPlant + 1);

		if (newState == mState)
			repeating = true;

		mState = newState;
		mStartId = newStartId;

		return !repeating;
	}

	std::size_t getSum()
	{
		std::size_t sum = 0;
		auto id = mStartId;

		for (char c : mState) {
			if (c == '#')
				sum += id;
			++id;
		}

		return sum;
	}

	int getStartId()
	{
		return mStartId;
	}

	const std::string& getStateString()
	{
		return mState;
	}
};

std::string parseInitialState()
{
	std::string line;
	std::getline(std::cin, line);
	return line.substr(line.find(':') + 2);
}

void parseRules(std::unordered_map<std::string, char>& rules)
{
	std::string line;

	while (std::getline(std::cin, line)) {
		if (!line.empty()) {
			const std::string rule{line.substr(0, line.find(' '))};
			rules[rule] = line.back();
		}
	}
}

int main()
{
	std::string initialState{parseInitialState()};
	std::unordered_map<std::string, char> rules;
	parseRules(rules);

	State part1{initialState, rules};
	for (unsigned gen = 1; gen <= 20; ++gen)
		part1.advance();
	std::cout << "part 1: " << part1.getSum() << '\n';

	State st{initialState, rules};
	unsigned gen = 1;
	while (st.advance())
		++gen;

	std::size_t sum = 0;
	std::size_t id = static_cast<std::size_t>(st.getStartId()) + (50'000'000'000 - gen);
	for (char c : st.getStateString()) {
		if (c == '#')
			sum += id;
		++id;
	}

	std::cout << "part 2: " << sum << '\n';
}
