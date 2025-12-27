#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// I don't know if this is the intended way of solving it
int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<int> shape_block_counts;
  int guaranteed = 0, impossible = 0, uncertain = 0;

  while (std::getline(file, line)) {
    if (line.contains('x')) {
      int width, height, shape_count;
      sscanf(line.c_str(), "%dx%d:", &width, &height);
      int i = 0, total_shape_count = 0, total_block_count = 0;
      std::istringstream iss(line.substr(line.find_first_of(' ')));

      while (iss >> shape_count) {
        total_shape_count += shape_count;
        total_block_count += shape_block_counts[i] * shape_count;
        ++i;
      }

      int shape_count_bound = (width / 3) * (height / 3),
          block_count_bound = width * height;

      std::cout << line << ": ";

      if (total_shape_count <= shape_count_bound) {
        std::cout << "fit guaranteed";
        ++guaranteed;
      } else if (total_block_count > block_count_bound) {
        std::cout << "fit impossible";
        ++impossible;
      } else {
        std::cout << "uncertain";
        ++uncertain;
      }

      std::cout << std::endl;
    } else {
      int count = 0;

      do {
        std::getline(file, line);
        count += std::count(line.begin(), line.end(), '#');
      } while (line.size());

      shape_block_counts.emplace_back(count);
    }
  }

  std::cout
    << "Guaranteed: "
    << guaranteed
    << "\nImpossible: "
    << impossible
    << "\nUncertain: "
    << uncertain
    << std::endl;
  return 0;
}
