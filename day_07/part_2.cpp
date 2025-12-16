#include <fstream>
#include <iostream>
#include <unordered_map>

void merge_or_insert(
  std::unordered_map<int, long> &beams, int idx, long stack
) {
  auto [_, ok] = beams.try_emplace(idx, stack);

  if (!ok) beams.at(idx) += stack;
}

int main() {
  std::fstream file("input.txt");
  std::string line;
  // key = index, value = timeline count
  std::unordered_map<int, long> beams;

  std::getline(file, line);
  beams.emplace((int) line.find('S'), 1);

  while (std::getline(file, line)) {
    std::unordered_map<int, long> new_beams;

    for (const auto [idx, stack] : beams) {
      if (line[idx] != '^') {
        merge_or_insert(new_beams, idx, stack);
        continue;
      }

      for (const int i : {idx - 1, idx + 1})
        merge_or_insert(new_beams, i, stack);
    }

    beams = new_beams;
  }

  long split_count = 0;

  for (const auto [_, stack] : beams)
    split_count += stack;

  std::cout << split_count << std::endl;

  return 0;
}
