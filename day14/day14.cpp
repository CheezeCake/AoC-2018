#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	if (argc < 1) {
		std::cerr << "usage: " << argv[0] << " input\n";
		return 1;
	}

	const std::string inputStr{argv[1]};
	auto input{std::stoul(inputStr)};

	std::string recipes{'3', '7'};
	std::size_t current1{0};
	std::size_t current2{1};
	auto inputPos{std::string::npos};

	while (recipes.size() < input + 10 || inputPos == std::string::npos) {
		int current1Value{recipes[current1] - '0'};
		int current2Value{recipes[current2] - '0'};

		int sum{current1Value + current2Value};
		if (sum >= 10)
			recipes.push_back(sum / 10 + '0');
		recipes.push_back(sum % 10 + '0');

		current1 = (current1 + (current1Value + 1)) % recipes.size();
		current2 = (current2 + (current2Value + 1)) % recipes.size();

		if (recipes.size() > inputStr.size())
			inputPos = recipes.find(inputStr, recipes.size() - inputStr.size() - 1);
	}

	std::cout << "part 1: " << recipes.substr(input, 10) << '\n';
	std::cout << "part 2: " << inputPos << '\n';
}
