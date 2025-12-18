#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

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
  int circuit_id = -1;
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
  std::unordered_map<int, std::vector<Box *>> id_box_map;

  while (std::getline(file, line)) {
    Vec3 pos;
    sscanf(line.c_str(), "%d,%d,%d", &pos.x, &pos.y, &pos.z);
    Box *box = new Box(pos);
    box->circuit_id = boxes.size();
    id_box_map.emplace(box->circuit_id, std::vector<Box *>{box});
    boxes.emplace_back(box);
  }

  for (int i = 0; i < (int) boxes.size() - 1; ++i)
    for (int j = i + 1; j < (int) boxes.size(); ++j)
      pq.emplace(boxes[i], boxes[j]);

  Box *a, *b;

  while (id_box_map.size() > 1) {
    std::tie(a, b) = pq.top().pair;
    pq.pop();

    if (a->circuit_id == b->circuit_id) continue;

    int big = a->circuit_id, smol = b->circuit_id;

    if (id_box_map.at(big).size() < id_box_map.at(smol).size())
      std::swap(big, smol);

    const std::vector<Box *> merge_src =
      std::move(id_box_map.extract(smol).mapped());
    std::vector<Box *> &merge_dest = id_box_map.at(big);

    for (Box *box : merge_src) {
      box->circuit_id = big;
      merge_dest.emplace_back(box);
    }
  }

  std::cout << a->pos.x * b->pos.x << std::endl;

  return 0;
}
