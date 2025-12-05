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

      if (str.size() % 2 != 0) continue;

      if (str.substr(0, str.size() / 2) == str.substr(str.size() / 2)) sum += i;
    }
  }

  printf("%ld\n", sum);
  return 0;
}
