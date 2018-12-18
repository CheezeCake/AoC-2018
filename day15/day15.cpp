#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>

struct Point
{
	int x;
	int y;
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
	os << '(' << p.x << ", " << p.y << ')';
	return os;
}

bool operator<(const Point& lhs, const Point& rhs)
{
	return (lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x));
}

bool operator==(const Point& lhs, const Point& rhs)
{
	return (lhs.x == rhs.x && lhs.y == rhs.y);
}

struct Unit
{
	enum class Type : char { Goblin = 'G', Elf  = 'E' };

	Type type;
	Point position;
	int attack{3};
	int hp{200};

	void hit(int attackValue)
	{
		hp -= attackValue;
	}

	bool isAlive() const
	{
		return (hp > 0);
	}

	Type getEnemyType() const
	{
		return (type == Type::Goblin) ? Type::Elf : Type::Goblin;
	}
};

class Battle
{
	std::vector<std::vector<bool>> mMap;
	std::vector<Unit> mUnits;

	bool isValid(const Point& p) const
	{
		return (p.y >= 0 && p.y < mMap.size() &&
				p.x >= 0 && p.x < mMap[p.y].size());
	}

	std::vector<Point> adjacentPoints(const Point& from) const
	{
		static const std::array<const Point, 4> directions = {{
			{0, -1},
			{-1, 0},
			{1, 0},
			{0, 1}
		}};

		std::vector<Point> adj;
		adj.reserve(4);

		for (const auto& dir : directions) {
			Point p{from.x + dir.x, from.y + dir.y};
			if (isValid(p))
				adj.push_back(p);
		}

		return adj;
	}

	std::optional<Point> getEnemyInRange(const Point& from, Unit::Type enemyType) const
	{
		std::optional<Point> e;
		int eHp{std::numeric_limits<int>::max()};

		for (const auto& p : adjacentPoints(from)) {
			if (unitAt(p)) {
				const auto& unit{getUnitAt(p)};
				if (unit.type == enemyType && unit.hp < eHp) {
					e = p;
					eHp = unit.hp;
				}
			}
		}

		return e;
	}

	Unit& getUnitAt(const Point& p)
	{
		return *std::find_if(std::begin(mUnits), std::end(mUnits),
							 [&p](const auto& u) { return (u.isAlive() && u.position == p); });
	}

	const Unit& getUnitAt(const Point& p) const
	{
		return *std::find_if(std::begin(mUnits), std::end(mUnits),
							 [&p](const auto& u) { return (u.isAlive() && u.position == p); });
	}

	bool unitAt(const Point& p) const
	{
		return (std::find_if(std::begin(mUnits), std::end(mUnits),
							 [&p](const auto& u) { return (u.isAlive() && u.position == p); })
				!= std::end(mUnits));
	}

	bool hitUnitAt(const Point& p, int attackValue)
	{
		auto& unit{getUnitAt(p)};
		unit.hit(attackValue);
		return unit.isAlive();
	}

	std::vector<Point> pointsInRange(Unit::Type enemyType) const
	{
		std::vector<Point> results;
		results.reserve((mUnits.size() - 1) * 4);

		for (const auto& unit : mUnits) {
			if (unit.type != enemyType || !unit.isAlive())
				continue;

			const auto& adj{adjacentPoints(unit.position)};
			for (const auto& p : adj) {
				auto [x, y] = p;
				if (!mMap[y][x] && !unitAt({x, y}))
					results.push_back(p);
			}
		}

		return results;
	}

	std::optional<std::pair<Point, unsigned>> findPath(const Point& from, const Point& to) const
	{
		std::vector<std::vector<bool>> visited{mMap.size(), std::vector<bool>(mMap[0].size(), false)};

		std::queue<std::pair<Point, unsigned>> q;
		q.push({from, 0});
		visited[from.y][from.x] = true;

		while (!q.empty()) {
			const auto [p, dist] = q.front();
			q.pop();

			for (const auto& adj : adjacentPoints(p)) {
				if (adj == to)
					return {{p, dist}};
				if (!visited[adj.y][adj.x] && !mMap[adj.y][adj.x] && !unitAt(adj)) {
					q.push({adj, dist + 1});
					visited[adj.y][adj.x] = true;
				}
			}
		}

		return {};
	}

	std::optional<Point> determineMove(const Point& from, Unit::Type enemyType) const
	{
		std::optional<Point> ret;
		unsigned retDist{std::numeric_limits<unsigned>::max()};

		auto points{pointsInRange(enemyType)};
		std::sort(std::begin(points), std::end(points));

		for (const auto& p : points) {
			const auto dist{findPath(p, from)};
			if (dist) {
				if (dist->second < retDist) {
					ret = dist->first;
					retDist = dist->second;
				}
			}
		}

		return ret;
	}

	void moveUnit(const Point& from, const Point& to)
	{
		auto& unit{getUnitAt(from)};
		unit.position = to;
	}

	void update()
	{
		std::sort(std::begin(mUnits), std::end(mUnits),
				  [](const auto& lhs, const auto& rhs) { return (lhs.position < rhs.position); });
	}

	bool enemiesLeft() const
	{
		int goblins{0};
		int elves{0};
		for (const auto& unit : mUnits) {
			if (!unit.isAlive())
				continue;
			if (unit.type == Unit::Type::Goblin)
				++goblins;
			else
				++elves;
		}

		return (goblins > 0 && elves > 0);
	}

public:
	Battle(const std::vector<std::string>& map, int elftAttack = 3)
	{
		mMap.resize(map.size());

		for (int y{0}; y < map.size(); ++y) {
			mMap[y].resize(map[y].size());

			for (int x{0}; x < map[y].size(); ++x) {
				mMap[y][x] = (map[y][x] == '#');

				if (map[y][x] == 'G')
					mUnits.push_back({Unit::Type::Goblin, {x, y}});
				else if (map[y][x] == 'E')
					mUnits.push_back({Unit::Type::Elf, {x, y}, elftAttack});
			}
		}
	}

	using ElfDead = int;

	bool round(bool stopOnElfDeath = false)
	{
		for (const auto& unit : mUnits) {
			if (!unit.isAlive())
				continue;
			if (!enemiesLeft())
				return false;

			const auto eType{unit.getEnemyType()};
			auto enemyPos{getEnemyInRange(unit.position, eType)};

			if (!enemyPos) {
				const auto to{determineMove(unit.position, eType)};
				if (to) {
					moveUnit(unit.position, *to);
					enemyPos = getEnemyInRange(*to, eType);
				}
			}
			if (enemyPos) {
				if (hitUnitAt(*enemyPos, unit.attack) &&
					eType == Unit::Type::Elf && stopOnElfDeath)
					throw ElfDead{};
			}
		}

		update();

		return enemiesLeft();
	}

	int play()
	{
		int rounds{0};
		while (round())
			++rounds;
		return rounds;
	}

	bool playUntilElfDeath()
	{
		while (true) {
			try {
				if (!round(true))
					return true;
			}
			catch (ElfDead) {
				return false;
			}
		}
	}

	void printHpForLine(int y) const
	{
		for (int x{0}; x < mMap[y].size(); ++x) {
			if (unitAt({x, y})) {
				const auto& unit{getUnitAt({x, y})};
				std::cout << ' ' << static_cast<char>(unit.type) << '(' << unit.hp << ')';
			}
		}
	}

	void print() const
	{
		for (int y{0}; y < mMap.size(); ++y) {
			for (int x{0}; x < mMap[y].size(); ++x) {
				if (unitAt({x, y})) {
					std::cout << static_cast<char>(getUnitAt({x, y}).type);
				}
				else {
					if (mMap[y][x])
						std::cout << '#';
					else
						std::cout << '.';
				}
			}
			printHpForLine(y);
			std::cout << '\n';
		}
	}

	int hpSum() const
	{
		int sum{0};
		for (const auto& u : mUnits) {
			if (u.isAlive())
				sum += u.hp;
		}
		return sum;
	}
};

int main(int argc, char** argv)
{
	std::vector<std::string> input;
	std::string line;
	while (std::getline(std::cin, line))
		input.push_back(line);

	{
		Battle b{input};
		int rounds{b.play()};
		int sum{b.hpSum()};
		std::cout << "part 1: " << rounds << " * " << sum << " = " << rounds * sum << '\n';
	}

	for (int attack{4}; ; ++attack) {
		Battle b{input, attack};
		if (b.playUntilElfDeath()) {
			std::cout << "part 2: " << attack << '\n';
			return 0;
		}
	}
}
