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
				// handling missing data
				if ((buffer[char_start] == ',') || (buffer[char_start] == '\n')) {
					csv_indx++;
					// filling the missing data as -1
					csv_mat->data[csv_indx] = -1;
					read_columns++;

					char_start = i + 1;
				} else {
					char temp = buffer[i];
					buffer[i] = '\0';
					num = atof(&buffer[char_start]);

					if (
						(num == 0.0) &&
						(strcmp("0.0", &buffer[char_start]) != 0) &&
						(strcmp("0", &buffer[char_start]) != 0)
					) {
						// printing around that char num
						int pps = ((char_start - 10) < 0) ? 0 : (char_start-10);
						int ppe = 
							((i + 10) > (bytes_read)) ? (bytes_read) : (i+10);

						buffer[i] = temp;
						for (int m = pps; m < ppe; m++) {
							if ((m >= char_start) && (m < i)) {
								fprintf(stderr, "\033[1;33m%c\033[0m", buffer[m]);
								continue;
							}
							fprintf(stderr, "%c", buffer[m]);
						}
						fprintf(stderr, "\n");

						fprintf(stderr, "char_start: %zu, i: %zu\n", char_start, i);
						buffer[i] = '\0';
						fprintf(stderr, "Couldn't convert: %s\n", &buffer[char_start]);
						fprintf(stderr, "--------------\n");
						retval = 10;
						goto cleanup;
					}

					buffer[i] = temp;
					csv_indx++;
					csv_mat->data[csv_indx] = num;
					read_columns++;

					char_start = i+1;
				}

				if (buffer[i] == '\n') {
					if (read_columns != col) {
						fprintf(stderr, "Incorrect file format\n");
						retval = 3;
						goto cleanup;
					}
					read_columns = 0;
				}
			}
		}

		if (bytes_read == BUFFER_SIZE) {
			if ((buffer[bytes_read-1] != '\n') || (buffer[bytes_read-1] != ',')) {
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
		// don't have to compare bytes<BUFFER as from above condition
		// if it is false then, it will lead to bytes < BUFFER
		// cause bytes read can't be greater than BUFFER_SIZE due to
		// fread returning value less than or equal to size
		} else if (buffer[bytes_read-1] != '\n') {
			// for cases where file doesn't end with \n
			// then last saved elem to matrix
			if (buffer[bytes_read-1] == ',') {
				csv_indx++;
				// filling the missing data as -1
				csv_mat->data[csv_indx] = -1;
				read_columns++;
			} else {
				// now only possiblity is either a number
				// or any other unexpected char
				buffer[bytes_read] = '\0';
				num = atof(&buffer[char_start]);

				if (
					(num == 0.0) &&
					(strcmp("0.0", &buffer[char_start]) != 0) &&
					(strcmp("0", &buffer[char_start]) != 0)
				) {
					// printing around that char num
					int pps = ((char_start - 10) < 0) ? 0 : (char_start-10);

					for (int m = pps; m < bytes_read; m++) {
						if (m >= char_start) {
							fprintf(stderr, "\033[1;33m%c\033[0m", buffer[m]);
							continue;
						}
						fprintf(stderr, "%c", buffer[m]);
					}
					fprintf(stderr, "\n");

					fprintf(stderr, "char_start: %zu, i: %zu\n", char_start, bytes_read-1);
					fprintf(stderr, "Couldn't convert: %s\n", &buffer[char_start]);
					fprintf(stderr, "-----------\n");
					retval = 10;
					goto cleanup;
				}

				csv_indx++;
				csv_mat->data[csv_indx] = num;
				read_columns++;
			}

			if (read_columns != col) {
				fprintf(stderr, "Inside Ending with non \\n\n");
				fprintf(stderr, "Incorrect file format\n");
				retval = 4;
				goto cleanup;
			}
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
