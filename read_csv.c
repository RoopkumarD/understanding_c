#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "/home/timepass/tid/pcd/mat_c/mat.h"

/*
```
~/t/p/understanding_c (main)> ./read_csv.out output.csv 1
*** stack smashing detected ***: terminated
fish: Job 1, './read_csv.out output.csv 1' terminated by signal SIGABRT (Abort)
```
Why this?
*/

int getc_read_csv(char *progname, char *filename);

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
		int success = getc_read_csv(argv[0], argv[1]);
		double end = clock();

		time_taken += (end - start);
	}

	time_taken /= iterations;

	printf("Mean Time Taken: %f\n", time_taken);

	return 0;
}

int getc_read_csv(char *progname, char *filename) {
	int retval = 0;
	Matrix *csv_mat = NULL;

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: can't open %s\n", progname, filename);
		retval = 2;
		goto cleanup;
	}

	int c;
	int row, col;
	row = col = 0;

	c = getc(fp);
	while ((c != '\n') && (c != EOF)) {
		col += (c == ',');
		c = getc(fp);
	}
	// because when there is error then file error is set
	// and because file error is set, the getc will return
	// EOF even though it is not end of file, hence
	// it will exit above loop
	if (ferror(fp)) {
		fprintf(stderr, "Error reading the file\n");
		retval = 4;
		goto cleanup;
	}
	// for last col where it encounters \n than comma
	col++;

	// to use fseek later to come back after finding all rows
	int curr_posn = ftell(fp);

	int check = -1;
	while ((c = getc(fp)) != EOF) {
		row += (c == '\n');
		check = c;
	}
	if (ferror(fp)) {
		fprintf(stderr, "Error reading the file\n");
		retval = 5;
		goto cleanup;
	}
	// for last element in right below corner when it encounters
	// EOF than '\n'
	// but there is a problem which is if the last element does indeed
	// have \n then, below will be extra
	// i have to compare if last elem is \n or not but can't use c as it is EOF now
	row += (check != '\n');

	// now coming back and reading the file and making matrix
	fseek(fp, curr_posn, SEEK_SET);

	csv_mat = Matrix_create(row, col);
	int csv_indx = -1;

	char buffer[10];
	for (int i = 0; i < 10; i++) {
		buffer[i] = '0';
	}
	int indx = 0;
	int read_columns = 0;
	double num = 0;
	int decimal_point_used = 0;

	while ((c = getc(fp)) != EOF) {
		if ((c == ',') || (c == '\n')) {
			if (decimal_point_used == 0) {
				buffer[indx] = '.';
			}
			num = atof(buffer);
			csv_indx++;
			csv_mat->data[csv_indx] = num;
			read_columns++;
			for (int j = 0; j < indx; j++) {
				buffer[j] = '0';
			}
			indx = 0;
			decimal_point_used = 0;

			if (c == '\n') {
				if (read_columns != col) {
					fprintf(stderr, "Incorrect file format\n");
					retval = 3;
					goto cleanup;
				} else {
					read_columns = 0;
				}
			}
		} else {
			buffer[indx] = c;
			if (c == '.') {
				decimal_point_used = 1;
			}
			indx++;
		}

		check = c;
	}
	if (ferror(fp)) {
		fprintf(stderr, "Error reading the file\n");
		retval = 6;
		goto cleanup;
	}
	// for cases where file doesn't end with \n
	if (check != '\n') {
		// then last saved elem to matrix
		num = atof(buffer);
		csv_indx++;
		csv_mat->data[csv_indx] = num;
	}

	// matrix_print(csv_mat);
	// to check if interpreted the csv right
	// matrix_dump_csv(csv_mat, "temp_dump.csv");

	cleanup:
	matrix_free(csv_mat);
	fclose(fp);

	return retval;
}
