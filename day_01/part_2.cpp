#include <math.h>
#include <stdio.h>

int main() {
  FILE *file = fopen("input.txt", "r");

  char direction;
  int magnitude;
  int dial = 50;
  int counter = 0;

  while (fscanf(file, "%c%d\n", &direction, &magnitude) != EOF) {
    const int new_dial = dial + (direction == 'L' ? -1 : 1) * magnitude;
    const auto [quot, rem] = div(new_dial, 100);
    const int counter_delta =
      std::abs(quot) + (new_dial <= 0 && dial > 0 ? 1 : 0);
    int canonical_dial = new_dial % 100;

    if (canonical_dial < 0) canonical_dial += 100;

    printf(
      "%2d --[%c%3d]-> %4d/%2d | delta: %d\n",
      dial,
      direction,
      magnitude,
      new_dial,
      canonical_dial,
      counter_delta
    );

    counter += counter_delta;
    dial = canonical_dial;
  }

  printf("%d\n", counter);
  fclose(file);
  return 0;
}
