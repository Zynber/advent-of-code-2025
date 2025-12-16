#include <fstream>
#include <iostream>
#include <vector>

struct Vec2 {
  int x, y;

  template <typename T> T access(const std::vector<std::vector<T>> grid) const {
    return grid[y][x];
  }

  Vec2 operator+(const Vec2 other) const { return {x + other.x, y + other.y}; }
};

const Vec2 CARDINALS[] = {
  {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

// kind of slow but good enough :]
int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<std::vector<bool>> grid;

  while (std::getline(file, line)) {
    grid.emplace_back();

    for (const char c : line)
      grid.back().emplace_back(c == '@');
  }

  const int w = grid.front().size(), h = grid.size();
  int total = 0;

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      const Vec2 pos = Vec2{j, i};

      if (!pos.access(grid)) continue;

      int count = 0;

      for (Vec2 dir : CARDINALS) {
        const Vec2 check_pos = pos + dir;

        if (check_pos.x < 0 || check_pos.x >= w || check_pos.y < 0 ||
            check_pos.y >= h)
          continue;

        if (check_pos.access(grid)) ++count;
      }

      if (count < 4) ++total;
    }
  }

  std::cout << "Total: " << total << std::endl;
  return 0;
}
