#include <stdio.h>

int main() {
  FILE *file = fopen("input.txt", "r");

  char direction;
  int magnitude;
  int dial = 50;
  int counter = 0;

  while (fscanf(file, "%c%d\n", &direction, &magnitude) != EOF) {
    printf("%c%d => ", direction, magnitude);
    int change = (direction == 'L' ? -1 : 1) * magnitude;
    dial += change;
    dial %= 100;

    if (dial < 0) dial += 100;

    printf("%d\n", dial);

    if (!dial) ++counter;
  }

  printf("%d\n", counter);
  fclose(file);
  return 0;
}
