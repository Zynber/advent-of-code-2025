#include <fstream>
#include <iostream>
#include <unordered_set>

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::unordered_set<int> beam_indices;
  int split_count = 0;

  std::getline(file, line);
  beam_indices.emplace((int) line.find('S'));

  while (std::getline(file, line)) {
    std::unordered_set<int> new_beam_indices;

    for (int beam_idx : beam_indices) {
      if (line[beam_idx] != '^') {
        new_beam_indices.emplace(beam_idx);
        continue;
      }

      new_beam_indices.emplace(beam_idx - 1);
      new_beam_indices.emplace(beam_idx + 1);
      ++split_count;
    }

    beam_indices = new_beam_indices;
  }

  std::cout << split_count << std::endl;

  return 0;
}
