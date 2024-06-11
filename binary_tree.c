/*
Waste a lot of space due to consideration of worst case scenario
when all elements are at right side of tree
*/

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int *bintree;
    short *posn_occupied;
    size_t len;
} BINARY_TREE;

BINARY_TREE *create_binary_tree(int *arr, size_t arr_len) {
    int bin_arr_size = pow(2, arr_len) - 1;
    BINARY_TREE *tree = malloc(sizeof(BINARY_TREE));
    if (tree == NULL) {
        fprintf(stderr, "Couldn't allocated memory for tree\n");
        return NULL;
    }
    tree->posn_occupied = malloc(sizeof(short) * bin_arr_size);
    if (tree->posn_occupied == NULL) {
        free(tree);
        fprintf(stderr, "Couldn't allocated memory for tree->posn_occupied\n");
        return NULL;
    }
    tree->bintree = malloc(sizeof(int) * bin_arr_size);
    if (tree->bintree == NULL) {
        free(tree->posn_occupied);
        free(tree);
        fprintf(stderr, "Couldn't allocated memory for tree->bintree\n");
        return NULL;
    }
    tree->len = bin_arr_size;
    for (size_t i = 0; i < arr_len; i++) {
        tree->posn_occupied[i] = 0;
    }

    for (size_t i = 0; i < arr_len; i++) {
        // walk through bin_tree and then
        // add the new node which is current i
        // elem at arr
        size_t posn = 0;
        while (1) {
            if (tree->posn_occupied[posn] == 0) {
                tree->posn_occupied[posn] = 1;
                tree->bintree[posn] = arr[i];
                break;
            } else if (arr[i] == tree->bintree[posn]) {
                // if duplicate then it will skip over it
                break;
            }

            if (arr[i] < tree->bintree[posn]) {
                posn = (posn * 2) + 1;
            } else {
                posn = (posn * 2) + 2;
            }
        }
    }

    return tree;
}

void shuffle(int *arr, size_t arr_length);

int main(void) {
    srand(time(NULL));

    size_t arr_len = 10;
    int arr[arr_len];
    for (int i = 0; i < arr_len; i++) {
        arr[i] = i;
    }
    shuffle(arr, arr_len);

    BINARY_TREE *tree = create_binary_tree(arr, arr_len);
    if (tree == NULL) {
        return -1;
    }

    int *bintree = tree->bintree;

    // let's calculate how many layer there are then print
    size_t last_idx = tree->len - 1;
    for (size_t i = tree->len - 1; i > -1; i--) {
        if (tree->posn_occupied[i] == 1) {
            last_idx = i;
        }
    }
    // total layers for binary tree
    // found with help of chatgpt where observe that each layer
    // ends with 2**n - 2 where n is layer num
    // so 2 ** n - 2 >= last_idx and then shifted the inequality to
    // get n something and used ceil as closest one
    size_t layer_num = ceil(log2(((double)last_idx / 2) + 1) + 1);

    for (size_t i = 0; i < layer_num; i++) {
        size_t start = pow(2, i) - 1;
        size_t end = pow(2, i + 1) - 1;

        for (size_t j = start; j < end; j++) {
            if (tree->posn_occupied[j] == 1) {
                printf("%d  ", bintree[j]);
            }
            printf("|");
        }
        printf("\n");
    }
    free(tree->posn_occupied);
    free(tree->bintree);
    free(tree);
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
