#include <array>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<std::array<long, 2>> ranges;

  while (std::getline(file, line)) {
    std::array<long, 2> new_range;

    if (sscanf(line.c_str(), "%ld-%ld", &new_range[0], &new_range[1]) != 2)
      break;

    std::vector<decltype(ranges)::iterator> ranges_to_union;

    for (auto it = ranges.begin(); it != ranges.end(); ++it) {
      auto range = *it;

      if ((range[0] <= new_range[1] && new_range[1] <= range[1]) ||
          (range[1] >= new_range[0] && new_range[0] >= range[0]) ||
          (new_range[0] <= range[1] && range[1] <= new_range[1]) ||
          (new_range[1] >= range[0] && range[0] >= new_range[0]))
        ranges_to_union.emplace_back(it);
    }

    while (ranges_to_union.size()) {
      auto it = ranges_to_union.back();
      auto range_to_union = *it;
      new_range[0] = std::min(range_to_union[0], new_range[0]);
      new_range[1] = std::max(range_to_union[1], new_range[1]);
      ranges.erase(it);
      ranges_to_union.pop_back();
    }

    ranges.emplace_back(new_range);
  }

  long count = 0;

  for (auto range : ranges)
    count += range[1] - range[0] + 1;

  std::cout << count << std::endl;
  return 0;
}
