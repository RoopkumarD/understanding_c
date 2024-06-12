#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    LinkedList *otherHead = pointer + 1;

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

int main(void) {
    srand(time(NULL));

    size_t arr_length = 100;
    int arr[arr_length];
    for (int i = 0; i < arr_length; i++) {
        arr[i] = i;
    }

    // shuffling
    shuffle(arr, arr_length);

    // sorting
    // bubble_sort(arr, arr_length);
    // selection_sort(arr, arr_length);
    insertion_sort(arr, arr_length);

    // checking sorting
    for (int i = 0; i < arr_length; i++) {
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
