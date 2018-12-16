#include <array>
#include <cstdio>
#include <iostream>
#include <set>
#include <stdexcept>

class VM
{
public:
	struct Instr
	{
		int opcode;
		int A;
		int B;
		int C;
	};

	using RegisterSet = std::array<int, 4>;

	VM()
	{
	}

	VM(const RegisterSet& rs)
	{
		setRegisters(rs);
	}

	int getRegister(int r) const
	{
		if (r < 0 || r >= regs.size())
			throw std::runtime_error("getting register: " + std::to_string(r));
		return regs[r];
	}

	RegisterSet getRegisters() const
	{
		return regs;
	}

	void setRegister(int r, int v)
	{
		if (r < 0 || r >= regs.size())
			throw std::runtime_error("setting register: " + std::to_string(r));
		regs[r] = v;
	}

	void setRegisters(const RegisterSet& rs)
	{
		std::copy(std::begin(rs), std::end(rs), std::begin(regs));
	}

	void exec(const Instr& i)
	{
		switch (i.opcode) {
			case 0:
				setRegister(i.C, getRegister(i.A) + getRegister(i.B));
				break;
			case 1:
				setRegister(i.C, getRegister(i.A) + i.B);
				break;
			case 2:
				setRegister(i.C, getRegister(i.A) * getRegister(i.B));
				break;
			case 3:
				setRegister(i.C, getRegister(i.A) * i.B);
				break;
			case 4:
				setRegister(i.C, getRegister(i.A) & getRegister(i.B));
				break;
			case 5:
				setRegister(i.C, getRegister(i.A) & i.B);
				break;
			case 6:
				setRegister(i.C, getRegister(i.A) | getRegister(i.B));
				break;
			case 7:
				setRegister(i.C, getRegister(i.A) | i.B);
				break;
			case 8:
				setRegister(i.C, getRegister(i.A));
				break;
			case 9:
				setRegister(i.C, i.A);
				break;
			case 10:
				setRegister(i.C, i.A > getRegister(i.B));
				break;
			case 11:
				setRegister(i.C, getRegister(i.A) > i.B);
				break;
			case 12:
				setRegister(i.C, getRegister(i.A) > getRegister(i.B));
				break;
			case 13:
				setRegister(i.C, i.A == getRegister(i.B));
				break;
			case 14:
				setRegister(i.C, getRegister(i.A) == i.B);
				break;
			case 15:
				setRegister(i.C, getRegister(i.A) == getRegister(i.B));
				break;
			default:
				throw std::runtime_error("invalid opcode: " + std::to_string(i.opcode));
		}
	}

private:
	RegisterSet regs = {0, 0, 0, 0};
};

int main()
{
	std::array<std::set<int>, 16> opcodeMatches;
	for (auto& matches : opcodeMatches)
		matches = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	int matchThree = 0;

	while (true) {
		VM::RegisterSet before;
		if (scanf("Before: [%d, %d, %d, %d]\n", &before[0], &before[1], &before[2], &before[3]) != 4)
			break;

		struct VM::Instr instr;
		int iOpcode;
		scanf("%d %d %d %d\n", &iOpcode, &instr.A, &instr.B, &instr.C);

		VM::RegisterSet after;
		scanf("After: [%d, %d, %d, %d]\n", &after[0], &after[1], &after[2], &after[3]);

		int matchOpcodes = 0;
		for (int mOpcode = 0; mOpcode <= 15; ++mOpcode) {
			VM vm{before};

			instr.opcode = mOpcode;
			vm.exec(instr);
			if (vm.getRegisters() == after)
				++matchOpcodes;
			else
				opcodeMatches[iOpcode].erase(mOpcode);
		}

		if (matchOpcodes >= 3)
			++matchThree;
	}

	std::cout << "part 1: " << matchThree << '\n';

	bool done;
	do {
		done = true;
		for (int i{0}; i < opcodeMatches.size(); ++i) {
			if (opcodeMatches[i].size() > 1) continue;
			int m{*std::begin(opcodeMatches[i])};
			for (int j{0}; j < opcodeMatches.size(); ++j) {
				if (j == i) continue;
				if (opcodeMatches[j].find(m) != std::end(opcodeMatches[j])) {
					opcodeMatches[j].erase(m);
					done = false;
				}
			}
		}
	} while (!done);

	std::array<int, 16> iToM;
	for (int i = 0; i < opcodeMatches.size(); ++i) {
		iToM[i] = *std::begin(opcodeMatches[i]);
	}

	VM vm;
	while (true) {
		struct VM::Instr instr;
		int n = scanf("%d %d %d %d\n", &instr.opcode, &instr.A, &instr.B, &instr.C);
		if (n == EOF) break;
		if (n < 4) continue;

		instr.opcode = iToM[instr.opcode];
		vm.exec(instr);
	}

	std::cout << "part 2: " << vm.getRegister(0) << '\n';;
}
