#include <array>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<std::array<long, 2>> ranges;
  std::vector<long> ids;

  while (std::getline(file, line)) {
    std::array<long, 2> range;
    const int count = sscanf(line.c_str(), "%ld-%ld", &range[0], &range[1]);

    if (count == 2)
      ranges.emplace_back(range);
    else if (count == 1)
      ids.emplace_back(range[0]);
  }

  int count = 0;

  for (long id : ids) {
    for (auto range : ranges) {
      if (range[0] <= id && id <= range[1]) {
        ++count;
        break;
      }
    }
  }

  std::cout << count << std::endl;
  return 0;
}
