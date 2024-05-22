#include <stdio.h>
#include <stdlib.h>

#define ALLOCATE_MEM(size, type, pointer)	\
	type buffer[size];			\
	pointer = &buffer[0];			\

int main(void) {
	char *s = NULL;
	printf("%p\n", s);
	ALLOCATE_MEM(5, char, s)
	printf("%p\n", s);

	for (int i = 0; i < 5; i++) {
		*(s + i) = '2';
		// s[i] = '1';
	}

	for (int i = 0; i < 5; i++) {
		printf("buffer[%d] -> %c\n", i, buffer[i]);
	}

	return 0;
}
