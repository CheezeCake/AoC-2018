#include <iostream>
#include <cstdio>
#include <string>
#include <array>
#include <stdexcept>
#include <set>
#include <vector>
#include <utility>

struct VM
{
	struct Instr
	{
		int opcode;
		int A;
		int B;
		int C;
	};

	std::array<int, 4> regs = {0, 0, 0, 0};

	int fetchRegister(int r)
	{
		if (r < 0 || r >= regs.size())
			throw std::runtime_error("fetching register: " + std::to_string(r));
		return regs[r];
	}

	void setRegister(int r, int v)
	{
		if (r < 0 || r >= regs.size())
			throw std::runtime_error("setting register: " + std::to_string(r));
		regs[r] = v;
	}

	void exec(const Instr& i)
	{
		switch (i.opcode) {
			case 0:
				setRegister(i.C, fetchRegister(i.A) + fetchRegister(i.B));
				break;
			case 1:
				setRegister(i.C, fetchRegister(i.A) + i.B);
				break;
			case 2:
				setRegister(i.C, fetchRegister(i.A) * fetchRegister(i.B));
				break;
			case 3:
				setRegister(i.C, fetchRegister(i.A) * i.B);
				break;
			case 4:
				setRegister(i.C, fetchRegister(i.A) & fetchRegister(i.B));
				break;
			case 5:
				setRegister(i.C, fetchRegister(i.A) & i.B);
				break;
			case 6:
				setRegister(i.C, fetchRegister(i.A) | fetchRegister(i.B));
				break;
			case 7:
				setRegister(i.C, fetchRegister(i.A) | i.B);
				break;
			case 8:
				setRegister(i.C, fetchRegister(i.A));
				break;
			case 9:
				setRegister(i.C, i.A);
				break;
			case 10:
				setRegister(i.C, i.A > fetchRegister(i.B));
				break;
			case 11:
				setRegister(i.C, fetchRegister(i.A) > i.B);
				break;
			case 12:
				setRegister(i.C, fetchRegister(i.A) > fetchRegister(i.B));
				break;
			case 13:
				setRegister(i.C, i.A == fetchRegister(i.B));
				break;
			case 14:
				setRegister(i.C, fetchRegister(i.A) == i.B);
				break;
			case 15:
				setRegister(i.C, fetchRegister(i.A) == fetchRegister(i.B));
				break;
			default:
				throw std::runtime_error("invalid opcode: " + std::to_string(i.opcode));
		}
	}
};

int main()
{
	int matchThree = 0;

	std::array<std::array<bool, 16>, 16> opcodeMatches;
	for (auto& matches : opcodeMatches)
		matches.fill(true);

	while (true) {
		int before[4];
		if (scanf("Before: [%d, %d, %d, %d]\n", &before[0], &before[1], &before[2], &before[3]) != 4)
			break;

		struct VM::Instr instr;
		int iOpcode;
		if (scanf("%d %d %d %d\n", &iOpcode, &instr.A, &instr.B, &instr.C) != 4)
			break;

		int after[4];
		if (scanf("After: [%d, %d, %d, %d]\n", &after[0], &after[1], &after[2], &after[3]) != 4)
			break;

		int matchOpcodes = 0;
		for (int mOpcode = 0; mOpcode <= 15; ++mOpcode) {
			VM vm;
			for (int i = 0; i < 4; ++i)
				vm.setRegister(i, before[i]);

			instr.opcode = mOpcode;
			vm.exec(instr);
			bool match{true};
			for (int i = 0; match && i < 4; ++i) {
				if (vm.fetchRegister(i) != after[i])
					match = false;
			}
			if (match) {
				++matchOpcodes;
				/* opcodeMatches[iOpcode].[mOpcode] = true; */
			}
			else {
				opcodeMatches[iOpcode][mOpcode] = false;
			}
		}

		std::cout << "match : " << matchOpcodes << '\n';
		if (matchOpcodes >= 3)
			++matchThree;
	}

	std::cout << "part 1: " << matchThree << '\n';

	std::vector<std::pair<int, int>> known;
	do {
		for (int j = 0; j < opcodeMatches.size(); ++j) {
			for (auto k : known) {
				if (j != k.first)
					opcodeMatches[j][k.second] = false;
			}
		}
		known.clear();
		for (int j = 0; j < opcodeMatches.size(); ++j) {
			int n = 0;
			int match;
			for (int i = 0; i < opcodeMatches[j].size(); ++i) {
				if (opcodeMatches[j][i]) {
					++n;
					match = i;
				}
			}
			std::cout << n << '\n';
			if (n == 1) known.push_back({j, match});
		}
		std::cout << '\n';
	} while (known.size() < 16);

	std::array<int, 16> iToM;
	for (int i = 0; i < opcodeMatches.size(); ++i) {
		int m;
		for (int j = 0; j < opcodeMatches[i].size(); ++j)
			if (opcodeMatches[i][j])
				m = j;
		iToM[i] = m;
	}

	VM vm;
	// read prog
	while (true) {
		struct VM::Instr instr;
		int n = scanf("%d %d %d %d\n", &instr.opcode, &instr.A, &instr.B, &instr.C);
		if (n == EOF) break;
		if (n < 4) continue;

		instr.opcode = iToM[instr.opcode];
		vm.exec(instr);
	}

	std::cout << "part 2: " << vm.fetchRegister(0) << '\n';;
}
