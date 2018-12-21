#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

class VM
{
	public:
		struct Instr
		{
			std::string name;
			int A;
			int B;
			int C;

		};

		using RegisterSet = std::array<int, 6>;

		VM()
		{
		}

		VM(const RegisterSet& rs)
		{
			setRegisters(rs);
		}

		VM(int bind) :
			ipBound{true},
			boundTo{bind}
		{
		}

		void print_regs() const
		{
			std::cout << '[';
			for (int r : regs)
				std::cout << r << ", ";
			std::cout << ']';
		}

		void executeProgram(const std::vector<Instr>& prog)
		{
			int prev{0};
			std::unordered_set<int> s;
			unsigned cnt{0};
			for (; ip >= 0 && ip < prog.size(); ++cnt) {
				int r = prog[ip].B;
				// optimization for part 2
				if (ip == 13) {
					if (256 > regs[r]) {
						ip = 28;
					}
					else {
						regs[r] >>= 8;
						ip = 8;
					}
				}

				if (ip == 28) {
					if (s.size() == 0)
						std::cout << "part 1: " << regs[5] << '\n';
					if (s.find(regs[5]) != std::end(s)) {
						std::cout << "part 2: " << prev << '\n';
						return;
					}
					prev = regs[5];
					s.insert(regs[5]);
				}

				exec(prog[ip]);
			}
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
			if (ipBound)
				setRegister(boundTo, ip);

			if (i.name == "addr") {
				setRegister(i.C, getRegister(i.A) + getRegister(i.B));
			}
			else if (i.name == "addi") {
				setRegister(i.C, getRegister(i.A) + i.B);
			}
			else if (i.name == "mulr") {
				setRegister(i.C, getRegister(i.A) * getRegister(i.B));
			}
			else if (i.name == "muli") {
				setRegister(i.C, getRegister(i.A) * i.B);
			}
			else if (i.name == "banr") {
				setRegister(i.C, getRegister(i.A) & getRegister(i.B));
			}
			else if (i.name == "bani") {
				setRegister(i.C, getRegister(i.A) & i.B);
			}
			else if (i.name == "borr") {
				setRegister(i.C, getRegister(i.A) | getRegister(i.B));
			}
			else if (i.name == "bori") {
				setRegister(i.C, getRegister(i.A) | i.B);
			}
			else if (i.name == "setr") {
				setRegister(i.C, getRegister(i.A));
			}
			else if (i.name == "seti") {
				setRegister(i.C, i.A);
			}
			else if (i.name == "gtir") {
				setRegister(i.C, i.A > getRegister(i.B));
			}
			else if (i.name == "gtri") {
				setRegister(i.C, getRegister(i.A) > i.B);
			}
			else if (i.name == "gtrr") {
				setRegister(i.C, getRegister(i.A) > getRegister(i.B));
			}
			else if (i.name == "eqir") {
				setRegister(i.C, i.A == getRegister(i.B));
			}
			else if (i.name == "eqri") {
				setRegister(i.C, getRegister(i.A) == i.B);
			}
			else if (i.name == "eqrr") {
				setRegister(i.C, getRegister(i.A) == getRegister(i.B));
			}
			else {
				throw std::runtime_error("invalid name: " + i.name);
			}

			if (ipBound)
				ip = getRegister(boundTo);

			++ip;
		}

	private:
		int ip{0};
		bool ipBound{false};
		int boundTo{0};
		RegisterSet regs = {0, 0, 0, 0, 0, 0};
};

int main()
{
	std::string line;
	std::getline(std::cin, line);
	int bind{std::stoi(line.substr(4))};

	std::vector<VM::Instr> prog;
	while (std::getline(std::cin, line)) {
		std::istringstream iss{line};
		VM::Instr inst;
		iss >> inst.name >> inst.A >> inst.B >> inst.C;
		prog.push_back(inst);
	}

	VM vm{bind};
	vm.executeProgram(prog);
}
