#include <iostream>
#include <vector>
#include <numeric>

struct Node
{
	int getValue()
	{
		if (!valueComputed) {
			if (children.empty()) {
				value = std::accumulate(std::begin(metadata), std::end(metadata), 0);
			}
			else {
				for (int childIndex : metadata) {
					if (childIndex > 0 && childIndex <= children.size())
						value += children[childIndex - 1].getValue();
				}
			}
			valueComputed = true;
		}
		return value;
	}

	int metadataSum() const
	{
		int sum = std::accumulate(std::begin(metadata), std::end(metadata), 0);
		for (const auto& child : children)
			sum += child.metadataSum();

		return sum;
	}

	std::vector<Node> children;
	std::vector<int> metadata;
	bool valueComputed = false;
	int value = 0;
};

std::vector<Node> readNodes(int n);

Node readNode()
{
	int children;
	int metadata;
	std::cin >> children >> metadata;

	Node n;
	n.children = readNodes(children);

	for (int i = 0; i < metadata; ++i) {
		int v;
		std::cin >> v;
		n.metadata.push_back(v);
	}

	return n;
}

std::vector<Node> readNodes(int n)
{
	std::vector<Node> nodes;
	nodes.reserve(n);

	for (int i = 0; i < n; ++i)
		nodes.push_back(readNode());

	return nodes;
}

int main()
{
	auto root = readNode();

	std::cout << "part 1: " << root.metadataSum() << '\n';
	std::cout << "part 2: " << root.getValue() << '\n';
}
