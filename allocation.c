#include <stdio.h>
#include <stdlib.h>

/*
#define ALLOCATE_MEM(size, type, pointer)	\
	type buffer[size];			\
	pointer = &buffer[0];			\
*/

// ({ }) is an expression whose value is returned
// when written like below. Here, &buffer[0] is returned
#define ALLOCATE_MEM(size, type)		\
	({ type buffer[size];			\
		&buffer[0]; })			\


int main(void) {
	char *s = NULL;
	printf("%p\n", s);
	s = ALLOCATE_MEM(5, char);
	printf("%p\n", s);

	for (int i = 0; i < 5; i++) {
		*(s + i) = '2';
		// s[i] = '1';
	}

	for (int i = 0; i < 5; i++) {
		printf("buffer[%d] -> %c\n", i, s[i]);
	}

	return 0;
}
