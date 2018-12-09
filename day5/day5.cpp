#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <stack>
#include <string>

static inline bool reactsWith(char a, char b)
{
	return (a != b && std::tolower(a) == std::tolower(b));
}

static std::size_t fullyReact(const std::string& poly)
{
	std::stack<char> st;

	for (char c : poly) {
		if (!st.empty() && reactsWith(st.top(), c))
			st.pop();
		else
			st.push(c);
	}

	return st.size();
}

int main()
{
	std::string input;
	std::getline(std::cin, input);

	std::cout << "part 1: " << fullyReact(input) << '\n';

	auto minSize = std::numeric_limits<std::size_t>::max();
	for (char unit{'a'}; unit <= 'z'; ++unit) {
		std::string poly{input};
		poly.erase(std::remove_if(std::begin(poly), std::end(poly),
								  [unit](char c) { return (std::tolower(c) == unit); }),
				   std::end(poly));
		minSize = std::min(minSize, fullyReact(poly));
	}

	std::cout << "part 2: " << minSize << '\n';

	return 0;
}
