#include <stdio.h>

// getting segaults
// investigate glibc source code at 
// libio/struct_FILE.h

int main(void) {
	FILE *fp = fopen("/tmp/temp.txt", "r");
	if (fp == NULL) {
		return 1;
	}

	char k = *((unsigned char *) (fp)->_IO_read_ptr++);

	printf("k -> %c\n", k);

	fclose(fp);
	return 0;
}
