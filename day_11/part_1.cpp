#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

struct Node {
  std::vector<const Node *> to;
};

void explore(const Node *const curr, const Node *const dest, int &path_count) {
  if (curr == dest) {
    ++path_count;
    return;
  }

  for (const auto node : curr->to) explore(node, dest, path_count);
}

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::unordered_map<std::string, Node *> nodes;

  while (std::getline(file, line)) {
    const auto [pair, _] = nodes.try_emplace(line.substr(0, 3), new Node);
    std::istringstream iss(line.substr(5));
    std::string connected_node_key;

    while (iss >> connected_node_key) {
      const auto [connected_pair, _] =
        nodes.try_emplace(connected_node_key, new Node);
      pair->second->to.emplace_back(connected_pair->second);
    }
  }

  const Node *const start = nodes.at("you"), *const end = nodes.at("out");
  int path_count = 0;
  explore(start, end, path_count);
  std::cout << path_count << std::endl;
  return 0;
}
