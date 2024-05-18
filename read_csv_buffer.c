#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "/home/timepass/tid/pcd/mat_c/mat.h"

/*
This is more faster than getc implementation even though
i handle missing value case here where there i didn't

I need a concrete answer to why this is faster
Isn't file stream already loaded in c and shouldn't
it behave like buffer below??

From what i understood, getc calls syscalls to read
Thus lower syscalls the better and since we are taking whole
file at one call, thus it is faster
*/
/*
Another problem: It doesn't interpret it rightly
refer compare.py

Actually it is about precision of double, since i am
using format specifier in fprintf -> by default it decimal
point precision is 6. Thus i get string of 6 decimal of double
in csv temp_dump

I have increased the precision to 10.

Another thing is i can't get proper precision as that given in csv
because once storing double in memory, it isn't like after all
digits everything is zero. Thus can't find a way to measure proper
precision which was given in csv.

Can save precision here, but it is overhead and i don't thing it will
matter much
*/
/*
```
Buffer Size: 2344984
Warning: client switching stacks?  SP change: 0x1ffedc38c0 --> 0x1fff0000e0
         to suppress, use: --max-stackframe=2344992 or greater
```
above is valgrind result and clearly means that we are exceeding the stack
limits as buffer memory is on stack. Thus, we need to discretise, rather than
taking whole thing as buffer we need to have 1KiB buffer which is 1024 bytes
```c
#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
size_t bytes_read;

while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    for (size_t i = 0; i < bytes_read; i++) {
        // Process the character
        printf("%c", buffer[i]);
    }
}
```
and then do below stuff. Which means we have to call fread again and again and
also need to change strategy with below char_pointer
*/

int buffer_read_csv(char *progname, char *filename);

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: executable filename\n");
		return 1;
	}

	// measuring performance with getc implementation
	int iterations = atoi(argv[2]);
	double time_taken = 0;

	for (int i = 0; i < iterations; i++) {
		double start = clock();
		int success = buffer_read_csv(argv[0], argv[1]);
		double end = clock();

		time_taken += (end - start);
	}

	time_taken /= iterations;

	printf("Mean Time Taken: %f\n", time_taken);

	return 0;
}

int buffer_read_csv(char *progname, char *filename) {
	int retval = 0;
	Matrix *csv_mat = NULL;

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: can't open %s\n", progname, filename);
		return 2;
	}

	if (fseek(fp, 0, SEEK_END)) {
		fprintf(stderr, "Can't Seek at End\n");
		fclose(fp);
		return 7;
	}
	long total_char = ftell(fp);
	if (total_char == -1) {
		fprintf(stderr, "Err while using ftell\n");
		fclose(fp);
		return 8;
	}
	if (fseek(fp, 0, SEEK_SET)) {
		fprintf(stderr, "Can't Seek at Start\n");
		fclose(fp);
		return 7;
	}

	printf("Buffer Size: %lu\n", total_char);
	char buffer[total_char];
	fread(buffer, 1, total_char, fp);
	if (ferror(fp)) {
		fprintf(stderr, "Error reading the file\n");
		retval = 8;
		goto cleanup;
	}

	int c = 0;
	int row = 0, col = 0;
	while ((buffer[c] != '\n') && (c < total_char)) {
		col += (buffer[c] == ',');
		c++;
	}
	// for last col where it encounters \n than comma
	col++;

	// c is index at after \n of that first row
	// which means 
	c++;
	int curr_posn = c;
	int check = -1;
	while (c < total_char) {
		row += (buffer[c] == '\n');
		check = buffer[c];
		c++;
	}
	// for last element in right below corner when it encounters
	// EOF than '\n'
	// but there is a problem which is if the last element does indeed
	// have \n then, below will be extra
	// i have to compare if last elem is \n or not but can't use c as it is EOF now
	row += (check != '\n');

	c = curr_posn;
	char *char_pointer = &buffer[curr_posn];

	csv_mat = Matrix_create(row, col);
	int read_columns = 0;
	int csv_indx = -1;
	double num = 0;

	while (++c < total_char) {
		if ((buffer[c] == ',') || (buffer[c] == '\n')) {
			if (*char_pointer == ',') {
				// handling missing data
				csv_indx++;
				csv_mat->data[csv_indx] = 0;
				read_columns++;

				char_pointer = &buffer[c+1];
			} else {
				char temp = buffer[c];
				buffer[c] = '\0';
				num = atof(char_pointer);
				csv_indx++;
				csv_mat->data[csv_indx] = num;
				buffer[c] = temp;
				read_columns++;

				char_pointer = &buffer[c+1];
			}

			if (buffer[c] == '\n') {
				if (read_columns != col) {
					fprintf(stderr, "Incorrect file format\n");
					retval = 3;
					goto cleanup;
				} else {
					read_columns = 0;
				}
			}
		}

		check = buffer[c];
	}
	// for cases where file doesn't end with \n
	if (check != '\n') {
		// then last saved elem to matrix
		if (*char_pointer == ',') {
			csv_indx++;
			csv_mat->data[csv_indx] = 0;
			read_columns++;
		} else {
			num = atof(char_pointer);
			csv_indx++;
			csv_mat->data[csv_indx] = num;
			read_columns++;
		}

		if (read_columns != col) {
			fprintf(stderr, "Incorrect file format\n");
			retval = 4;
			goto cleanup;
		}
	}

	// matrix_print(csv_mat);
	// to check if interpreted the csv right
	// matrix_dump_csv(csv_mat, "temp_dump_buffer.csv");

	cleanup:
	matrix_free(csv_mat);
	fclose(fp);

	return retval;
}
