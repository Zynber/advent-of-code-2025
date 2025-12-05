#include <fstream>
#include <sstream>
#include <string>

int main() {
  long l, r, sum = 0;
  std::ifstream file("input.txt");
  std::string range;

  while (std::getline(file, range, ',')) {
    sscanf(range.c_str(), "%ld-%ld", &l, &r);

    for (long i = l; i <= r; ++i) {
      std::string str = std::to_string(i);
      bool is_repeated = false;

      for (int seg_len = str.size() / 2; seg_len >= 1; --seg_len) {
        auto [quot, rem] = div(str.size(), seg_len);

        if (rem != 0) continue;

        bool invalid_flag = false;

        for (int j = 0; j < quot - 1; ++j)
          if (str.substr(seg_len * j, seg_len) !=
              str.substr(seg_len * (j + 1), seg_len)) {
            invalid_flag = true;
            break;
          }

        if (!invalid_flag) {
          is_repeated = true;
          break;
        }
      }

      if (is_repeated) sum += i;
    }
  }

  printf("%ld\n", sum);
  return 0;
}
