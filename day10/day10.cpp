#include <iostream>
#include <cstdio>
#include <vector>
#include <limits>
#include <algorithm>

struct Point
{
	int x;
	int y;
	int velX;
	int velY;
};

class Sky
{
public:
	Sky(const std::vector<Point>& points) :
		mPoints{points}
	{
		resetBounds();
		for (const auto& p : mPoints)
			setBounds(p);
	}

	void display() const
	{
		for (int y = y1; y <= y2; ++y) {
			for (int x = x1; x <= x2; ++x) {
				if (findPoint(x, y))
					std::cout << '#';
				else
					std::cout << ' ';
			}
			std::cout << '\n';
		}
	}

	bool findPoint(int x, int y) const
	{
		return std::find_if(std::begin(mPoints),
							std::end(mPoints),
							[x, y](const auto& p) { return (p.x == x && p.y == y); }) != std::end(mPoints);
	}

	void advance()
	{
		resetBounds();
		for (auto& p : mPoints) {
			p.x += p.velX;
			p.y += p.velY;
			setBounds(p);
		}
	}

	void setBounds(const Point& p)
	{
		x1 = std::min(x1, p.x);
		y1 = std::min(y1, p.y);

		x2 = std::max(x2, p.x);
		y2 = std::max(y2, p.y);
	}

	void resetBounds()
	{
		x1 = std::numeric_limits<int>::max();
		y1 = std::numeric_limits<int>::max();
		x2 = 0;
		y2 = 0;
	}

	int height() const
	{
		return (y2 - y1);
	}

private:
	std::vector<Point> mPoints;
	// top left
	int x1;
	int y1;
	// bottom right
	int x2;
	int y2;
};

int main()
{
	std::vector<Point> points;
	Point p;
	while (scanf("position=<%d, %d> velocity=<%d, %d>\n", &p.x, &p.y, &p.velX, &p.velY) != EOF)
		points.push_back(p);

	Sky sky{points};
	int seconds = 0;
	while (sky.height() > 10) {
		sky.advance();
		++seconds;
	}

	std::cout << "part 1:\n";
	sky.display();
	std::cout << "part 2: " << seconds << '\n';
}
