#include <stdio.h>

int main(void) {
	char *filename = "test.csv";
	// opened the file stream
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Couldn't open %s file\n", filename);
		return 1;
	}

	int c;
	while((c = getc(fp)) != EOF) {
		// putchar(c);
		putc(c, stdout);
	}

	// closing the file stream
	fclose(fp);
	return 0;
}
