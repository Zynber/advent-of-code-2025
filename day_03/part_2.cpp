#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <queue>
#include <set>
#include <vector>

const int MAX_COUNT = 12;

long power(long base, long exp) {
  long result = 1;

  while (exp > 0) {
    if (exp & 1) result *= base;

    base *= base;
    exp >>= 1;
  }

  return result;
}

struct Joltage {
  int index, value;

  // Prioritize larger value, then larger index
  struct MaxHeapComparator {
    bool operator()(const Joltage &a, const Joltage &b) const {
      return a.value == b.value ? a.index < b.index : a.value < b.value;
    }
  };

  // Smaller values come first
  struct IndexAscComparator {
    bool operator()(const Joltage &a, const Joltage &b) const {
      return a.index < b.index;
    }
  };
};

int main() {
  std::fstream file("input.txt");
  std::string line;
  long sum = 0;

  while (std::getline(file, line)) {
    std::
      priority_queue<Joltage, std::vector<Joltage>, Joltage::MaxHeapComparator>
        pq;

    for (int i = 0; i < (int) line.length(); ++i)
      pq.emplace(Joltage{i, line.at(i) - '0'});

    std::set<Joltage, Joltage::IndexAscComparator> arr;

    while (arr.size() < MAX_COUNT) {
      // Selected element that would be right of current joltage if selected
      const auto right_adjacent_it = arr.lower_bound(pq.top());

      bool should_emplace = [&]() {
        // That element does not exist?
        if (right_adjacent_it == arr.end()) return true;

        // First element right of current that has a larger value
        const auto right_larger_it =
          std::find_if(right_adjacent_it, arr.end(), [&pq](Joltage j) {
            return j.value > pq.top().value;
          });

        if (right_larger_it == arr.end()) return true;

        // Number of elements after right_larger_it
        const int right_to_end_dist =
          line.size() - (*right_larger_it).index - 1;
        // Number of elements after right_larger_it that are selectable
        const int candidate_count =
          right_to_end_dist - (std::distance(right_larger_it, arr.end()) - 1);

        if (candidate_count < MAX_COUNT - (int) arr.size()) return true;

        return false;
      }();

      if (should_emplace) arr.emplace(pq.top());

      pq.pop();
    }

    long i = 0;

    for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
      sum += (*it).value * power(10, i);
      ++i;
    }

    // For debugging purposes
    std::string indicators(line.size(), ' ');

    for (auto it = arr.begin(); it != arr.end(); ++it)
      indicators.at((*it).index) = '^';

    std::cout << line << std::endl << indicators << std::endl;
  }

  std::cout << "Sum: " << sum << std::endl;

  return 0;
}
