#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

struct Vec2 {
  int x, y;

  template <typename T> T &access(std::vector<T> &grid, int width) const {
    return grid[width * y + x];
  }

  Vec2 operator+(const Vec2 &other) const { return {x + other.x, y + other.y}; }

  bool operator==(const Vec2 &other) const {
    return x == other.x && y == other.y;
  }

  struct Hash {
    std::size_t operator()(const Vec2 &v) const noexcept {
      return v.x ^ (v.y << 1);
    }
  };
};

const Vec2 CARDINALS[] = {
  {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

const int MAX_THRESHOLD = 4;

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<u_int8_t> grid;
  std::getline(file, line);
  const int w = line.size();

  do {
    for (const char c : line)
      grid.emplace_back(c == '@');
  } while (std::getline(file, line));

  const int h = grid.size() / w;
  int total = 0;
  std::unordered_set<Vec2, Vec2::Hash> pos_to_check,
    pos_to_check_next(grid.size());

  for (int i = 0; i < (int) grid.size(); ++i) {
    auto [y, x] = div(i, w);
    pos_to_check_next.emplace(Vec2{x, y});
  }

  do {
    std::vector<Vec2> removable_rolls;
    pos_to_check = pos_to_check_next;
    pos_to_check_next.clear();

    for (Vec2 pos : pos_to_check) {
      if (!pos.access(grid, w)) continue;

      int count = 0;
      std::vector<Vec2> local_to_check_next;

      for (int j = 0; j < (int) std::size(CARDINALS); ++j) {
        const Vec2 check_pos = pos + CARDINALS[j];

        if (check_pos.x < 0 || check_pos.x >= w || check_pos.y < 0 ||
            check_pos.y >= h)
          continue;

        if (check_pos.access(grid, w)) {
          ++count;
          local_to_check_next.emplace_back(check_pos);
        }
      }

      if (count >= MAX_THRESHOLD) continue;

      removable_rolls.emplace_back(pos);
      pos_to_check_next.insert(
        std::make_move_iterator(local_to_check_next.begin()),
        std::make_move_iterator(local_to_check_next.end())
      );
    }

    total += removable_rolls.size();
    std::string grid_str;

    for (u_int8_t is_roll : grid)
      grid_str += is_roll ? '@' : '.';

    for (Vec2 removable_roll : removable_rolls) {
      removable_roll.access(grid, w) = false;
      grid_str[removable_roll.y * w + removable_roll.x] = 'X';
    }

    for (int i = 0; i < h; ++i)
      std::cout << grid_str.substr(i * w, w) << std::endl;

    std::cout << "Removed: " << removable_rolls.size() << std::endl;
    std::cout << std::string(w, '=') << std::endl;
  } while (pos_to_check_next.size());

  std::cout << "Total: " << total << std::endl;
  return 0;
}
