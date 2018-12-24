#include <algorithm>
#include <bitset>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

enum class Army { ImmuneSystem, Infection };

enum AttackType
{
	Bludgeoning,
	Cold,
	Fire,
	Radiation,
	Slashing,
};
using AttackTypes = std::bitset<5>;

static const std::unordered_map<std::string, AttackType> AttackName2AttackType = {
	{"bludgeoning", AttackType::Bludgeoning},
	{"cold", AttackType::Cold},
	{"fire", AttackType::Fire},
	{"radiation", AttackType::Radiation},
	{"slashing", AttackType::Slashing},
};

struct Group
{
	int units;
	int hpPerUnit;
	int attackPoints;
	int initiative;
	AttackType attackType;
	AttackTypes immunities;
	AttackTypes weaknesses;

	Army army;

	int getEffectivePower() const
	{
		return (units * attackPoints);
	}

	bool isAlive() const
	{
		return (units > 0);
	}

	int possibleDamage(const Group& target) const
	{
		if (target.immunities.test(attackType))
			return 0;

		auto damage = getEffectivePower();
		if (target.weaknesses.test(attackType))
			damage *= 2;
		return damage;
	}

	int attack(Group& target) const
	{
		int deaths = possibleDamage(target) / target.hpPerUnit;
		target.units -= deaths;
		if (target.units < 0)
			target.units = 0;
		return deaths;
	}
};


class Battle
{
	std::vector<Group> mGroups;

	void update()
	{
		std::sort(std::begin(mGroups), std::end(mGroups), [](const auto& lhs, const auto& rhs) {
					auto lep = lhs.getEffectivePower();
					auto rep = rhs.getEffectivePower();
					return (lep > rep || (lep == rep && lhs.initiative > rhs.initiative));
				  });
	}

	std::optional<int> chooseTarget(int attacker, const std::vector<bool>& selected) const
	{
		std::optional<int> target;
		int possibleDamage{0};

		for (int i = 0; i < mGroups.size(); ++i) {
			if (!mGroups[i].isAlive() || mGroups[i].army == mGroups[attacker].army || selected[i])
				continue;

			auto damage = mGroups[attacker].possibleDamage(mGroups[i]);
			if (damage == 0)
				continue;

			if (damage >= possibleDamage) {
				if (target) {
					if (damage == possibleDamage &&
						mGroups[i].getEffectivePower() < mGroups[*target].getEffectivePower())
						continue;
					else if (mGroups[i].getEffectivePower() == mGroups[*target].getEffectivePower() &&
							 mGroups[i].initiative < mGroups[*target].initiative)
						continue;
				}

				target = i;
				possibleDamage = damage;
			}
		}

		return target;
	}

	bool done() const
	{
		bool is{false};
		bool infection{false};

		for (const auto& g : mGroups) {
			if (!g.isAlive())
				continue;

			if (g.army == Army::ImmuneSystem)
				is = true;
			else
				infection = true;

			if (is && infection)
				return false;
		}

		return true;
	}

	Army winner() const
	{
		for (const auto& g : mGroups) {
			if (g.isAlive())
				return g.army;
		}
		std::cerr << "no winner???\n";
		return Army::Infection;
	}

	void printGroups() const
	{
		for (int i = 0; i < mGroups.size(); ++i) {
			if (!mGroups[i].isAlive())
				continue;

			std::cout << '[' << i << "] ";
			if (mGroups[i].army == Army::ImmuneSystem)
				std::cout << "IS: ";
			else
				std::cout << "Infection: ";
			std::cout << mGroups[i].units << " units";

			std::cout << '\n';
		}
	}

	int fight()
	{
		std::vector<std::pair<int, int>> targets;
		std::vector<bool> selected(mGroups.size(), false);

		for (int i = 0; i < mGroups.size(); ++i) {
			if (!mGroups[i].isAlive())
				continue;

			auto target = chooseTarget(i, selected);
			if (target) {
				targets.push_back({i, *target});
				selected[*target] = true;
			}
		}

		std::sort(std::begin(targets), std::end(targets), [this](const auto& lhs, const auto& rhs) {
					return (mGroups[lhs.first].initiative > mGroups[rhs.first].initiative);
				  });

		int damageSum{0};
		for (auto [attacker, target] : targets) {
			if (!mGroups[attacker].isAlive())
				continue;

			damageSum += mGroups[attacker].attack(mGroups[target]);
		}

		update();

		return damageSum;
	}
public:
	Battle(const std::vector<Group>& groups, int isBoost = 0) :
		mGroups{groups}
	{
		update();

		for (auto& g : mGroups) {
			if (g.army == Army::ImmuneSystem)
				g.attackPoints += isBoost;
		}
	}

	std::optional<Army> play()
	{
		while (!done()) {
			auto dmg = fight();
			if (dmg == 0) {
				/* std::cout << "stopping\n"; */
				return {};
			}
		}

		return winner();
	}

	int unitSum() const
	{
		int sum{0};
		for (const auto& g : mGroups) {
			if (!g.isAlive())
				continue;
			sum += g.units;
		}
		return sum;
	}
};

AttackTypes parseTypes(const std::string& str)
{
	AttackTypes types;

	int start{0};
	while (start < str.length()) {
		int end = str.find(',', start);
		auto size = (end == std::string::npos) ? end : (end - start);
		auto name = str.substr(start, size);

		types.set(AttackName2AttackType.at(name));

		start = end;
		if (start != std::string::npos)
			start += 2;
	}

	return types;
}

std::vector<Group> parseGroups()
{
	std::string line;
	std::getline(std::cin, line);

	Army currentArmy{Army::ImmuneSystem};
	if (line != "Immune System:")
		currentArmy = Army::Infection;

	std::vector<Group> groups;

	while (std::getline(std::cin, line)) {
		Group g;
		g.army = currentArmy;
		const char digits[] = "0123456789";

		if (line.empty()) continue;
		if (line.find_first_of(digits) != 0) {
			currentArmy = (line == "Immune System:") ? Army::ImmuneSystem : Army::Infection;
			continue;
		}

		auto space = line.find(' ');
		g.units = std::stoi(line.substr(0, space));

		auto start{line.find_first_of(digits, space + 1)};
		space = line.find(' ', start);
		g.hpPerUnit = std::stoi(line.substr(start, space - start));

		auto op = line.find('(');
		std::string buf;
		if (op != std::string::npos) {
			auto cp = line.find(')');
			buf = line.substr(op + 1, cp - op - 1);
			space = cp;
		}
		else {
			space = line.find(' ', space + 1);
		}

		start = line.find_first_of(digits, space);
		space = line.find(' ', start);
		g.attackPoints = std::stoi(line.substr(start, space - start));

		auto atkType = line.substr(space + 1, line.find(' ', space + 1) - space - 1);
		g.attackType = AttackName2AttackType.at(atkType);

		g.initiative = std::stoi(line.substr(line.rfind(' ') + 1));

		if (!buf.empty()) {
			start = buf.find(' ') + 4;
			auto sc = buf.find(';');
			auto types = parseTypes(buf.substr(start, sc - start));
			if (buf[0] == 'i') {
				g.immunities = types;
			}
			else {
				g.weaknesses = types;
			}

			if (sc != std::string::npos) {
				start = buf.find(' ', sc + 2) + 4;
				types = parseTypes(buf.substr(start));
				if (buf[0] == 'i') {
					g.weaknesses = types;
				}
				else {
					g.immunities = types;
				}
			}
		}

		groups.push_back(g);
	}

	return groups;
}

int main()
{
	const auto groups = parseGroups();

	{
		Battle b{groups};
		b.play();
		std::cout << "part 1: " << b.unitSum() << '\n';
	}

	for (int boost = 1; ; ++boost) {
		/* std::cout << "trying boost=" << boost << '\n'; */
		Battle b{groups, boost};
		auto winner = b.play();
		if (winner && *winner == Army::ImmuneSystem) {
			/* std::cout << "boost=" << boost << '\n'; */
			std::cout << "part 2: " << b.unitSum() << '\n';
			return 0;
		}
	}
}
