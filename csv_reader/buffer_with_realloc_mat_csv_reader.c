#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "/home/timepass/tid/pcd/mat_c/mat.h"

#define VALIDATE_ZERO_STRING(char_start, i, temp)					\
	int invalid_string = 0;								\
	int got_dot = 0;								\
	int end_posn = (buffer[i-1] == '\r') ? (i-1) : i;				\
	for (int zi = char_start; zi < end_posn; zi++) {				\
		if (buffer[zi] != '0' && buffer[zi] != '.') {				\
			invalid_string = 1;						\
			break;								\
		}									\
		if (got_dot == 1 && buffer[zi] != '0') {				\
			invalid_string = 1;						\
			break;								\
		} else if (got_dot == 0) {						\
			if (buffer[zi] == '.') {					\
				got_dot = 1;						\
				continue;						\
			} else if (buffer[zi] != '0') {					\
				invalid_string = 1;					\
				break;							\
			}								\
		}									\
	}										\
	if (invalid_string == 1) {							\
		int pps = ((char_start - 10) < 0) ? 0 : (char_start-10);		\
		int ppe =								\
			((i + 10) > (bytes_read)) ? (bytes_read) : (i+10);		\
		buffer[i] = temp;							\
		for (int m = pps; m < ppe; m++) {					\
			if ((m >= char_start) && (m < i)) {				\
				fprintf(stderr, "\033[1;33m%c\033[0m", buffer[m]);	\
				continue;						\
			}								\
			fprintf(stderr, "%c", buffer[m]);				\
		}									\
		fprintf(stderr, "\n");							\
		fprintf(stderr, "char_start: %zu, i: %zu\n", char_start, i);		\
		buffer[i] = '\0';							\
		fprintf(stderr, "Couldn't convert: %s\n", &buffer[char_start]);		\
		fprintf(stderr, "--------------\n");					\
		time_taken = -10;							\
		goto cleanup;								\
	}										\

#define BUFFER_SIZE 1024
#define SMALL_BUFFER_SIZE 200
#define MAT_ALLOC 500

// Here, rather than counting the number of lines first, we will
// first allocate 500 or other rows content for matrix and then realloc more 500 or other
// if need more size. Lastly i will cut down extra memory at end

long buffer_read_csv(char *progname, char *filename);

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: executable filename numtimes\n");
		return 1;
	}
	int iterations = atoi(argv[2]);
	double time_taken = 0;

	for (int i = 0; i < iterations; i++) {
		long result = buffer_read_csv(argv[0], argv[1]);
		if (result < 0) {
			return 1;
		}
		time_taken += result;
	}

	time_taken /= iterations;

	printf("Mean Time Taken: %f\n", time_taken);

	return 0;
}

long buffer_read_csv(char *progname, char *filename) {
	long time_taken = 0;
	Matrix *csv_mat = NULL;
	int total_filename_str_size = strlen("/tmp/") + strlen(filename);
	char dump_filename[total_filename_str_size+1];

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: can't open %s\n", progname, filename);
		return -2;
	}

	long start = clock();

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
		time_taken = -8;
		goto cleanup;
	}
	// to move to next char after '\n'
	file_idx++;

	if (fseek(fp, file_idx, SEEK_SET)) {
		fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
		fclose(fp);
		return -7;
	}

	char buffer[BUFFER_SIZE];

	size_t char_start = 0;

	csv_mat = Matrix_create(MAT_ALLOC, col);
	int total_allocated_rows_num = MAT_ALLOC;
	int current_total_space = col * total_allocated_rows_num;
	int read_columns = 0;
	int csv_indx = -1;
	double num = 0;

	while((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
		char_start = 0;
		for (size_t i = 0; i < bytes_read; i++) {
			if ((buffer[i] == ',') || (buffer[i] == '\n')) {
				char temp = buffer[i];
				buffer[i] = '\0';
				num = atof(&buffer[char_start]);

				if (num == 0.0) {
					if (
						(char_start == i) ||
						((buffer[char_start] == '\r') && (i == (char_start + 1)))
					) {
						num = NAN;
					} else {
						VALIDATE_ZERO_STRING(char_start, i, temp);
					}
				}

				buffer[i] = temp;
				csv_indx++;
				csv_mat->data[csv_indx] = num;
				read_columns++;

				// basically meaning that i have filled last elem
				if (csv_indx + 1 == current_total_space) {
					total_allocated_rows_num += MAT_ALLOC;
					double *temp = realloc(csv_mat->data, (col * total_allocated_rows_num) * sizeof(double));
					if (temp == NULL) {
						fprintf(stderr, "Couldn't Reallocate\n");
						time_taken = -13;
						goto cleanup;
					}
					current_total_space = col * total_allocated_rows_num;
					csv_mat->data = temp;
				}

				char_start = i + 1;

				if (buffer[i] == '\n') {
					if (read_columns != col) {
						fprintf(stderr, "Incorrect file format\n");
						time_taken = -3;
						goto cleanup;
					}
					row++;
					read_columns = 0;
				}
			}
		}

		if (bytes_read == BUFFER_SIZE) {
			if ((buffer[bytes_read-1] != '\n') || (buffer[bytes_read-1] != ',')) {
				file_idx += char_start;
				if (fseek(fp, file_idx, SEEK_SET)) {
					fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
					time_taken = -7;
					goto cleanup;
				}
			} else {
				file_idx += bytes_read;
			}
		} else if (buffer[bytes_read-1] != '\n') {
			// now only possiblity is either a number
			// or any other unexpected char

			if (csv_indx + 1 == current_total_space) {
				long last_amt = (col * total_allocated_rows_num) + 1;
				double *temp = realloc(csv_mat->data, last_amt * sizeof(double));
				if (temp == NULL) {
					fprintf(stderr, "Couldn't Reallocate at last\n");
					time_taken = -13;
					goto cleanup;
				}
				csv_mat->data = temp;
			}

			buffer[bytes_read] = '\0';
			num = atof(&buffer[char_start]);

			if (num == 0.0) {
				if (
					(char_start == (bytes_read-1) && buffer[char_start] == '\r') ||
					(char_start == bytes_read)
				) {
					num = NAN;
				} else {
					VALIDATE_ZERO_STRING(char_start, bytes_read, '\0');
				}
			}

			csv_indx++;
			csv_mat->data[csv_indx] = num;
			read_columns++;
			row++;

			if (read_columns != col) {
				fprintf(stderr, "Inside Ending with non \\n\n");
				fprintf(stderr, "Incorrect file format\n");
				time_taken = -4;
				goto cleanup;
			}
		}
	}

	// now removing extra space from csv_mat->data
	double *temp = realloc(csv_mat->data, (csv_indx+1) * sizeof(double));
	if (temp == NULL) {
		fprintf(stderr, "Couldn't Reallocate at End for final realloc\n");
		time_taken = -13;
		goto cleanup;
	}
	csv_mat->data = temp;
	csv_mat->rows = row;

	long end = clock();
	time_taken = (end-start);

	// matrix_print(csv_mat);
	/*
	dump_filename[total_filename_str_size] = '\0';
	dump_filename[0] = '/';
	dump_filename[1] = 't';
	dump_filename[2] = 'm';
	dump_filename[3] = 'p';
	dump_filename[4] = '/';
	for (int i = 5; i < total_filename_str_size; i++) {
		dump_filename[i] = filename[i];
	}
	matrix_dump_csv(csv_mat, dump_filename);
	*/

	cleanup:
	matrix_free(csv_mat);
	fclose(fp);

	return time_taken;
}
