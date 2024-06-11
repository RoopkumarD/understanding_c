#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// O(N^2) time and O(1) space
void selection_sort(int *arr, size_t arr_length) {
    for (int i = 0; i < arr_length - 1; i++) {
        int smallest = i;
        for (int j = i + 1; j < arr_length; j++) {
            if (arr[j] < arr[smallest]) {
                smallest = j;
            }
        }
        int temp = arr[i];
        arr[i] = arr[smallest];
        arr[smallest] = temp;
    }
    return;
}

// O(N^2) time and O(1) space
void bubble_sort(int *arr, size_t arr_length) {
    int temp;
    for (int i = 0; i < arr_length - 1; i++) {
        for (int j = 0; j < arr_length - (i + 1); j++) {
            // ascending order
            if (arr[j] > arr[j + 1]) {
                temp = arr[j + 1];
                arr[j + 1] = arr[j];
                arr[j] = temp;
            }
        }
    }
    return;
}

void shuffle(int *arr, size_t arr_length);

int main(void) {
    srand(time(NULL));

    int arr[100];
    for (int i = 0; i < 100; i++) {
        arr[i] = i;
    }

    // shuffling
    shuffle(arr, 100);

    // sorting
    // bubble_sort(arr, 100);
    selection_sort(arr, 100);

    // checking sorting
    for (int i = 0; i < 100; i++) {
        printf("%d, ", arr[i]);
    }
    printf("\n");

    return 0;
}

unsigned int rand_range(unsigned int min, unsigned int max) {
    assert(max >= min);
    unsigned int range = max - min + 1;
    return min + (int)(((double)rand() / RAND_MAX) * range);
}

void shuffle(int *arr, size_t arr_length) {
    for (int i = arr_length - 1; i > -1; i--) {
        unsigned int replacement = rand_range(0, i);
        int temp = arr[replacement];
        arr[replacement] = arr[i];
        arr[i] = temp;
    }

    return;
}
