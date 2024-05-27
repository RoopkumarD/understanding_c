#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// max inclusive
unsigned int rand_range(unsigned int min, unsigned int max) {
  assert(max >= min);
  unsigned int range = max - min + 1;
  return min + (int)(((double)rand() / RAND_MAX) * range);
}

int main(void) {
  // let's say i have 0 to 7 bins
  // with containing 1 to 8 number inside it
  srand(time(NULL));

  // row means each elem in numbers and cols means each number
  int checking[7][7] = {
      {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0},
  };

  int total_num = 7;
  unsigned int max = total_num - 1;

  int numbers[total_num];
  for (int k = 0; k < 1000; k++) {
    for (int i = 0; i < total_num; i++) {
      numbers[i] = i;
    }

    for (int i = max; i > -1; i--) {
      unsigned int replacement = rand_range(0, i);
      int temp = numbers[replacement];
      numbers[replacement] = numbers[i];
      numbers[i] = temp;
    }

    for (int i = 0; i < total_num; i++) {
      checking[i][numbers[i]]++;
    }
  }

  for (int i = 0; i < total_num; i++) {
    for (int j = 0; j < total_num; j++) {
      printf("%d\t", checking[i][j]);
    }
    printf("\n");
  }

  /*
  for (int i = 0; i < total_num; i++) {
    printf("%d\t", numbers[i]);
  }
  printf("\n");
  */

  return 0;
}
