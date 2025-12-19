#include <fstream>
#include <iostream>
#include <vector>

struct Vec2 {
  const static Vec2 ONE;

  int x, y;

  Vec2 abs() const { return {std::abs(x), std::abs(y)}; }

  Vec2 operator+(const Vec2 &other) const { return {x + other.x, y + other.y}; }

  Vec2 operator-(const Vec2 &other) const { return {x - other.x, y - other.y}; }
};

const Vec2 Vec2::ONE = {1, 1};

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<Vec2> positions;

  while (std::getline(file, line)) {
    Vec2 pos;
    sscanf(line.c_str(), "%d,%d", &pos.x, &pos.y);
    positions.emplace_back(pos);
  }

  long max_area = 0;

  for (int i = 0; i < (int) positions.size() - 1; ++i) {
    for (int j = i + 1; j < (int) positions.size(); ++j) {
      const Vec2 diff = (positions[i] - positions[j]).abs() + Vec2::ONE;
      max_area = std::max(max_area, 1L * diff.x * diff.y);
    }
  }

  std::cout << max_area << std::endl;
  return 0;
}
