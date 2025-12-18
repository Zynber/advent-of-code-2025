#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

const int CONNECTION_COUNT = 1000, CIRCUIT_COUNT = 3;

struct Vec3 {
  int x, y, z;

  Vec3 operator-(const Vec3 &other) const {
    return {x - other.x, y - other.y, z - other.z};
  }

  long squared_magnitude() {
    return (long) x * x + (long) y * y + (long) z * z;
  }
};

struct Box {
  Vec3 pos;
  std::vector<Box *> neighbors;
  bool traversed = false;

  Box(Vec3 pos) : pos(pos) {}
};

struct BoxPair {
  std::pair<Box *, Box *> pair;
  long squared_magnitude;

  BoxPair(Box *a, Box *b)
    : pair(a, b), squared_magnitude((a->pos - b->pos).squared_magnitude()) {}

  struct MinHeapComparator {
    bool operator()(const BoxPair &a, const BoxPair &b) const {
      return a.squared_magnitude > b.squared_magnitude;
    }
  };
};

struct VectorBoxMinHeapSizeComparator {
  bool
  operator()(const std::vector<Box *> &a, const std::vector<Box *> &b) const {
    return a.size() > b.size();
  }
};

void traverse(Box *current, std::vector<Box *> &traversed) {
  current->traversed = true;
  traversed.emplace_back(current);

  for (Box *neighbor : current->neighbors) {
    if (neighbor->traversed) continue;

    traverse(neighbor, traversed);
  }
}

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<Box *> boxes;
  std::priority_queue<BoxPair, std::vector<BoxPair>, BoxPair::MinHeapComparator>
    pq;
  std::priority_queue<
    std::vector<Box *>,
    std::vector<std::vector<Box *>>,
    VectorBoxMinHeapSizeComparator>
    circuits;

  while (std::getline(file, line)) {
    Vec3 pos;
    sscanf(line.c_str(), "%d,%d,%d", &pos.x, &pos.y, &pos.z);
    boxes.emplace_back(new Box(pos));
  }

  for (int i = 0; i < (int) boxes.size() - 1; ++i)
    for (int j = i + 1; j < (int) boxes.size(); ++j)
      pq.emplace(boxes[i], boxes[j]);

  for (int i = 0; i < CONNECTION_COUNT; ++i) {
    auto [a, b] = pq.top().pair;
    a->neighbors.emplace_back(b);
    b->neighbors.emplace_back(a);
    pq.pop();
  }

  for (Box *box : boxes) {
    if (box->traversed) continue;

    std::vector<Box *> networked_boxes;
    traverse(box, networked_boxes);
    circuits.push(std::move(networked_boxes));

    if (circuits.size() > CIRCUIT_COUNT) circuits.pop();
  }

  long total = 1;

  while (circuits.size()) {
    total *= circuits.top().size();
    circuits.pop();
  }

  std::cout << total << std::endl;
  return 0;
}
