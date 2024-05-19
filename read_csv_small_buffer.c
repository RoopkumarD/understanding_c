#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "/home/timepass/tid/pcd/mat_c/mat.h"

#define BUFFER_SIZE 1024
#define SMALL_BUFFER_SIZE 200

/*
It is like finding a middle ground between getc
and whole buffer thing. Where we read for small
time and use buffer to do that
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

	char sml_buffer[SMALL_BUFFER_SIZE];
	size_t bytes_read;

	size_t file_idx = 0;
	int got_new_line = 0;
	int row = 0, col = 0;
	while((bytes_read = fread(sml_buffer, 1, SMALL_BUFFER_SIZE, fp)) > 0) {
		for (size_t i = 0; i < bytes_read; i++) {
			if (sml_buffer[i] == '\n') {
				file_idx += i;
				got_new_line = 1;
				// for last column
				col++;
				break;
			}
			col += (sml_buffer[i] == ',');
		}
		if (got_new_line == 1) {
			break;
		}
		file_idx += bytes_read;
	}
	if (ferror(fp)) {
		fprintf(stderr, "Error reading the file\n");
		retval = 8;
		goto cleanup;
	}
	// to move to next char after '\n'
	file_idx++;

	if (fseek(fp, file_idx, SEEK_SET)) {
		fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
		fclose(fp);
		return 7;
	}

	char buffer[BUFFER_SIZE];
	int check = -1;
	while((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
		for (size_t i = 0; i < bytes_read; i++) {
			row += (buffer[i] == '\n');
			check = buffer[i];
		}
	}
	if (ferror(fp)) {
		fprintf(stderr, "Error reading the file\n");
		retval = 8;
		goto cleanup;
	}
	// for last element in right below corner when it encounters
	// EOF than '\n'
	// but there is a problem which is if the last element does indeed
	// have \n then, below will be extra
	// i have to compare if last elem is \n or not but can't use c as it is EOF now
	row += (check != '\n');

	if (fseek(fp, file_idx, SEEK_SET)) {
		fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
		fclose(fp);
		return 7;
	}

	size_t char_start = 0;

	csv_mat = Matrix_create(row, col);
	int read_columns = 0;
	int csv_indx = -1;
	double num = 0;

	int how_many_times_seek = 0;
	while((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
		char_start = 0;
		for (size_t i = 0; i < bytes_read; i++) {
			if ((buffer[i] == ',') || (buffer[i] == '\n')) {
				if (buffer[char_start] == ',') {
					// handling missing data
					csv_indx++;
					csv_mat->data[csv_indx] = 0;
					read_columns++;

					char_start = i+1;
				} else {
					char temp = buffer[i];
					buffer[i] = '\0';
					num = atof(&buffer[char_start]);
					if ((num == 0.0) && (
						(strcmp("0.0", &buffer[char_start]) != 0) &&
						(strcmp("0", &buffer[char_start]) != 0)
					)) {
						for (int m = char_start-10; m < i+10; m++) {
							if (m == i) {
								printf("%c", temp);
							} else {
								printf("%c", buffer[m]);
							}
						}
						printf("\n");
						printf("i: %zu, char_start: %zu\n", i, char_start);
						fprintf(stderr, "Couldn't convert: %s\n", &buffer[char_start]);
						retval = 10;
						goto cleanup;
					}
					csv_indx++;
					csv_mat->data[csv_indx] = num;
					buffer[i] = temp;
					read_columns++;

					char_start = i+1;
				}

				if (buffer[i] == '\n') {
					if (read_columns != col) {
						puts("Inside main loop");
						fprintf(stderr, "Incorrect file format\n");
						retval = 3;
						goto cleanup;
					}
					read_columns = 0;
				}
			}
		}
		// for cases where file doesn't end with \n
		if ((bytes_read < BUFFER_SIZE) && (buffer[bytes_read-1] != '\n')) {
			// then last saved elem to matrix
			if (buffer[char_start] == ',') {
				csv_indx++;
				csv_mat->data[csv_indx] = 0;
				read_columns++;
			} else {
				num = atof(&buffer[char_start]);
				csv_indx++;
				csv_mat->data[csv_indx] = num;
				read_columns++;
			}

			if (read_columns != col) {
				puts("Inside Ending with non \\n");
				fprintf(stderr, "Incorrect file format\n");
				retval = 4;
				goto cleanup;
			}
		} else if (
			(bytes_read == BUFFER_SIZE) &&
			(buffer[bytes_read-1] != ',') &&
			(buffer[bytes_read-1] != '\n')
		) {
			file_idx += char_start;
			if (fseek(fp, file_idx, SEEK_SET)) {
				fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
				retval = 7;
				goto cleanup;
			}
			how_many_times_seek++;
		} else {
			file_idx += bytes_read;
		}
	}

	// printf("This much is times program seeked file: %d\n", how_many_times_seek);

	// matrix_print(csv_mat);
	// to check if interpreted the csv right
	// matrix_dump_csv(csv_mat, "temp_dump_small_buffer.csv");

	cleanup:
	matrix_free(csv_mat);
	fclose(fp);

	return retval;
}
