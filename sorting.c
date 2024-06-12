#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merge(int *arr1, size_t arr1_len, int *arr2, size_t arr2_len) {
    // one neat thing about this is that, arr1 and arr2
    // are continous. Thus i can just create a temporary array
    // in stack and store merging and later copy that temp array
    // to original arr

    int combined_len = arr1_len + arr2_len;
    int temp[combined_len];
    int i = 0, j = 0, k = 0;

    while (k < combined_len) {
        if (i == arr1_len) {
            temp[k] = arr2[j];
            j++;
        } else if (j == arr2_len) {
            temp[k] = arr1[i];
            i++;
        } else {
            if (arr1[i] > arr2[j]) {
                temp[k] = arr2[j];
                j++;
            } else {
                temp[k] = arr1[i];
                i++;
            }
        }
        k++;
    }

    for (int i = 0; i < combined_len; i++) {
        arr1[i] = temp[i];
    }

    return;
}

void merge_sort(int *arr, size_t arr_length) {
    if (arr_length == 1) {
        return;
    }

    // splitting to left
    int left_end = arr_length / 2;
    merge_sort(arr, left_end);

    // splitting to right
    int right_start = arr_length - left_end;
    merge_sort(arr + left_end, right_start);

    // after sorting individual, let's merge them
    merge(arr, left_end, arr + left_end, right_start);

    return;
}

typedef struct ll {
    int num;
    struct ll *nxt;
} LinkedList;

void insertion_sort(int *arr, size_t arr_length) {
    LinkedList *nodes = malloc(sizeof(LinkedList[arr_length + 1]));
    if (nodes == NULL) {
        printf("Couldn't assign memory for nodes\n");
        return;
    }
    LinkedList *pointer = nodes;
    LinkedList *base = pointer;
    base->num = 0;
    base->nxt = NULL;

    for (size_t i = 0; i < arr_length; i++) {
        LinkedList *elem = ++pointer;
        elem->num = arr[i];
        elem->nxt = NULL;

        // insertion time
        LinkedList *before = base;
        LinkedList *curr = base->nxt;
        while (curr != NULL) {
            if (curr->num > elem->num) {
                break;
            }
            before = curr;
            curr = curr->nxt;
        }
        elem->nxt = before->nxt;
        before->nxt = elem;
    }

    LinkedList *curr = base->nxt;
    size_t i = -1;
    while (curr != NULL) {
        arr[++i] = curr->num;
        curr = curr->nxt;
    }

    free(nodes);
    return;
}

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./exec 0|1{for printing the sorted arr}\n");
        return 0;
    }

    srand(time(NULL));

    size_t arr_length = 1000000;
    int arr[arr_length];
    for (int i = 0; i < arr_length; i++) {
        arr[i] = i;
    }

    // shuffling
    shuffle(arr, arr_length);

    // sorting
    // bubble_sort(arr, arr_length);
    // selection_sort(arr, arr_length);
    // insertion_sort(arr, arr_length);
    merge_sort(arr, arr_length);

    // checking sorting
    int should_print = atoi(argv[1]);
    if (should_print == 1) {
        for (int i = 0; i < arr_length; i++) {
            printf("%d, ", arr[i]);
        }
        printf("\n");
    }

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
