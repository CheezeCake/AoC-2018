#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int checksum(const std::vector<std::string>& boxIds)
{
	int two = 0;
	int three = 0;

	for (const auto& id : boxIds) {
		int nTwo = 0;
		int nThree = 0;
		std::unordered_map<char, int> chars;

		for (char c : id) {
			switch (++chars[c]) {
				case 2:
					++nTwo;
					break;
				case 3:
					--nTwo;
					++nThree;
					break;
				case 4:
					--nThree;
			}
		}
		if (nTwo > 0)
			++two;
		if (nThree > 0)
			++three;
	}

	return (two * three);
}

std::string commonLetters(const std::vector<std::string>& boxIds)
{
	for (int i = 0; i < boxIds.size(); ++i) {
		for (int j = i + 1; j < boxIds.size(); ++j) {
			bool diff = false;
			std::string answer;
			const auto& id1 = boxIds[i];
			const auto& id2 = boxIds[j];
			for (int k = 0; k < boxIds[i].length(); ++k) {
				if (id1[k] != id2[k]) {
					if (diff)
						break;
					diff = true;
				}
				else {
					answer += id1[k];
				}
			}

			if (answer.length() == id1.length() - 1) {
				return answer;
			}
		}
	}
	return {};
}

int main()
{
	std::vector<std::string> boxIds;
	while (!std::cin.eof()) {
		std::string id;
		std::cin >> id;
		boxIds.push_back(id);
	}

	std::cout << "part 1: " << checksum(boxIds) << '\n';
	std::cout << "part 2: " << commonLetters(boxIds) << '\n';
}
