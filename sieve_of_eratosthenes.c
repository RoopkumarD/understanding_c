#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./exec [number]\n");
        return 1;
    }

    int num = atoi(argv[1]);
    int all_nums[num];
    // cause 0 and 1 can't be prime numbers
    all_nums[0] = 1;
    all_nums[1] = 1;
    for (int i = 2; i < num; i++) {
        all_nums[i] = 0;
    }

    // now let's go through each number
    // then if that number is prime, so let's go through
    // those multiple's and mark them 1
    for (int i = 2; i < num; i++) {
        // i * i as previous i * 2 to i * (i-1) were already
        // marked 1 by 2 to (i-1) prime number
        if (all_nums[i] == 0 && i * i <= num) {
            for (int j = i * i; j <= num; j += i) {
                all_nums[j] = 1;
            }
        }
    }

    for (int i = 2; i < num; i++) {
        if (all_nums[i] == 0) {
            printf("%d  ", i);
        }
    }
    printf("\n");

    return 0;
}
