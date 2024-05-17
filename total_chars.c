#include <stdio.h>

int main(int argc, char *argv[]) {
	FILE *fp = fopen(argv[1], "r");

	fseek(fp, 0, SEEK_END);
	int total_char = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// with above fseek => last char is EOF
	char buffer[total_char];
	for (int i = 0; i < total_char; i++) {
		buffer[i] = '0';
	}

	// so fread reads all the character of file and only gives
	// till last character. It won't give EOF. It makes sense as
	// EOF means End of file and why would it be copied, it is not a char
	fread(buffer, 1, total_char, fp);
	// total char means total character leaving EOF out.
	printf("feof value: %d\n", feof(fp));
	int c = getc(fp);
	printf("c value: %c\n", c);
	printf("c value integer: %d\n", c);
	printf("\\n value integer: %d\n", '\n');
	printf("feof value: %d\n", feof(fp));
	// this means that above we read all the character in file
	// then last character was left in file stream which was EOF
	// thus after reading it we get EOF and thus feof returns true
	// which is 1 as we read EOF

	/*
	printf("If Truth then value: %d\n", 2 == 2);
	printf("Value for buffer[total - 1] == \\n: %d\n", buffer[total_char - 1] == '\n');
	printf("Value for buffer[total] == EOF: %d\n", buffer[total_char] == EOF);
	printf("Value for buffer[total] == '0': %d\n", buffer[total_char] == '0');
	*/

	fclose(fp);
	return 0;
}
