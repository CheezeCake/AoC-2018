#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>

static inline bool isCart(char c)
{
	switch (c) {
		case '<':
		case '^':
		case '>':
		case 'v':
			return true;
		default:
			return false;
	}
}

struct Cart
{
	int x;
	int y;
	char c;
	int direction{0};

	std::tuple<int, int> nextPosition() const
	{
		static const std::unordered_map<char, const std::pair<int, int>> directions = {
			{'<', {-1, 0}},
			{'^', {0, -1}},
			{'>', {1, 0}},
			{'v', {0, 1}},
		};

		return {x + directions.at(c).first, y + directions.at(c).second};
	}

	void changeDirection(char t)
	{
		static const std::unordered_map<char, const std::unordered_map<char, char>> directionChange = {
			{'<', {
					  {'/',  'v'},
					  {'\\', '^'},
				  }
			},
			{'^', {
					  {'/',  '>'},
					  {'\\', '<'},
				  }
			},
			{'>', {
					  {'/',  '^'},
					  {'\\', 'v'},
				  }
			},
			{'v', {
					  {'/',  '<'},
					  {'\\', '>'},
				  }
			},
		};

		// left == 0
		static const std::unordered_map<char, char> intersection[2] = {
			{
				{'<', 'v'},
				{'^', '<'},
				{'>', '^'},
				{'v', '>'},
			},
			{
				{'<', '^'},
				{'^', '>'},
				{'>', 'v'},
				{'v', '<'},
			},
		};

		switch (t) {
			case '-':
			case '|':
				break;
			case '/':
			case '\\':
				c = directionChange.at(c).at(t);
				break;
			case '+':
				if (direction == 0)
					c = intersection[direction].at(c);
				else if (direction == 2)
					c = intersection[direction - 1].at(c);
				direction = (direction+1)%3;
				break;
		}
	}

	void moveTo(int nextX, int nextY, char t)
	{
		x = nextX;
		y = nextY;
		changeDirection(t);
	}
};

bool operator<(const Cart& lhs, const Cart& rhs)
{
	if (lhs.y == rhs.y)
		return (lhs.x < rhs.x);
	return (lhs.y < rhs.y);
}

class Mine
{
	std::vector<std::string> mTracks;
	std::vector<Cart> mCarts;

	void initCarts()
	{
		for (int y{0}; y < mTracks.size(); ++y) {
			for (int x{0}; x < mTracks[y].size(); ++x) {
				char c = mTracks[y][x];
				if (isCart(c)) {
					mCarts.push_back({x, y, c});
					mTracks[y][x] = (c == '<' || c == '>') ? '-' : '|';
				}
			}
		}
	}

	int findCart(int x, int y) const
	{
		for (int i = 0; i < mCarts.size(); ++i) {
			if (mCarts[i].y > y)
				return -1;
			if (mCarts[i].y == y && mCarts[i].x > x)
				return -1;
			if (mCarts[i].x == x && mCarts[i].y == y)
				return i;
		}

		return -1;
	}

	public:
	Mine(const std::vector<std::string>& tracks) :
		mTracks{tracks}
	{
		initCarts();
	}

	std::tuple<int, int> runUntilCollison()
	{
		while (true) {
			for (auto& cart : mCarts) {
				auto [nextX, nextY] = cart.nextPosition();
				if (findCart(nextX, nextY) >= 0)
					return {nextX, nextY}; // collision

				cart.moveTo(nextX, nextY, mTracks[nextY][nextX]);
			}
			std::sort(std::begin(mCarts), std::end(mCarts));
		}
	}

	std::tuple<int, int> runUntilOneLeft()
	{
		while (mCarts.size() > 1) {
			std::vector<Cart> cartsLeft;
			cartsLeft.reserve(mCarts.size());

			for (auto& cart : mCarts) {
				if (cart.x < 0) continue;
				auto [nextX, nextY] = cart.nextPosition();
				int nc = findCart(nextX, nextY);
				if (nc >= 0) {
					cart.x = -1;
					cart.y = -1;
					mCarts[nc].x = -1;
					mCarts[nc].y = -1;
				}
				else {
					cart.moveTo(nextX, nextY, mTracks[nextY][nextX]);
				}
			}

			for (auto& c : mCarts) {
				if (c.x >= 0 && c.y >= 0)
					cartsLeft.push_back(c);
			}
			mCarts = std::move(cartsLeft);
			std::sort(std::begin(mCarts), std::end(mCarts));
		}

		const auto& left = mCarts.front();
		return {left.x, left.y};
	}

	void print() const
	{
		std::cout << mCarts.size() << '\n';
		for (int y{0}; y < mTracks.size(); ++y) {
			for (int x{0}; x < mTracks[y].size(); ++x) {
				int cart = findCart(x, y);
				if (cart >= 0) {
					std::cout << mCarts[cart].c;
				}
				else {
					std::cout << mTracks[y][x];
				}
			}
			std::cout << '\n';
		}
		std::cout << '\n';
	}
};


int main()
{
	std::vector<std::string> tracks;
	std::string line;
	while (std::getline(std::cin, line))
		tracks.push_back(line);

	{
		Mine m{tracks};
		auto [x, y] = m.runUntilCollison();
		std::cout << "part 1: " << x << ',' << y << '\n';
	}

	Mine m{tracks};
	auto [x, y] = m.runUntilOneLeft();
		std::cout << "part 2: " << x << ',' << y << '\n';
}
