#include <fstream>
#include <vector>

int main() {
  std::fstream file("input.txt");
  char c;
  // 2nd element is like a recycle bin I guess
  int max_val[] = {0, 0, 0}, max_idx[] = {-1, -1, -1}, i = 0, sum = 0;

  while (file.get(c)) {
    if (c != '\n') {
      int v = c - '0';

      if (v > max_val[0]) {
        max_val[2] = max_val[0];
        max_idx[2] = max_idx[0];
        max_val[0] = v;
        max_idx[0] = i;
        max_val[1] = 0;
        max_idx[1] = -1;
      } else if (v > max_val[1]) {
        max_val[1] = v;
        max_idx[1] = i;
      }

      ++i;
      continue;
    }

    if (max_idx[1] == -1) {
      max_val[1] = max_val[2];
      max_idx[1] = max_idx[2];
    }

    sum += max_idx[0] < max_idx[1] ? max_val[0] * 10 + max_val[1]
                                   : max_val[1] * 10 + max_val[0];
    i = 0;
    max_val[0] = max_val[1] = 0;
    max_idx[0] = max_idx[1] = -1;
  }

  printf("%d\n", sum);
  return 0;
}
