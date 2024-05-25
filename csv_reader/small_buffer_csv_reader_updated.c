#include "/home/roopkumar/tid/LRN/nn_in_c/mat.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VALIDATE_ZERO_STRING(char_start, i, temp)                              \
  int invalid_string = 0;                                                      \
  int got_dot = 0;                                                             \
  int end_posn = (buffer[i - 1] == '\r') ? (i - 1) : i;                        \
  for (int zi = char_start; zi < end_posn; zi++) {                             \
    if (buffer[zi] != '0' && buffer[zi] != '.') {                              \
      invalid_string = 1;                                                      \
      break;                                                                   \
    }                                                                          \
    if (got_dot == 1 && buffer[zi] != '0') {                                   \
      invalid_string = 1;                                                      \
      break;                                                                   \
    } else if (got_dot == 0) {                                                 \
      if (buffer[zi] == '.') {                                                 \
        got_dot = 1;                                                           \
        continue;                                                              \
      } else if (buffer[zi] != '0') {                                          \
        invalid_string = 1;                                                    \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  if (invalid_string == 1) {                                                   \
    int pps = ((char_start - 10) < 0) ? 0 : (char_start - 10);                 \
    int ppe = ((i + 10) > (bytes_read)) ? (bytes_read) : (i + 10);             \
    buffer[i] = temp;                                                          \
    for (int m = pps; m < ppe; m++) {                                          \
      if ((m >= char_start) && (m < i)) {                                      \
        fprintf(stderr, "\033[1;33m%c\033[0m", buffer[m]);                     \
        continue;                                                              \
      }                                                                        \
      fprintf(stderr, "%c", buffer[m]);                                        \
    }                                                                          \
    fprintf(stderr, "\n");                                                     \
    fprintf(stderr, "char_start: %zu, i: %zu\n", char_start, i);               \
    buffer[i] = '\0';                                                          \
    fprintf(stderr, "Couldn't convert: %s\n", &buffer[char_start]);            \
    fprintf(stderr, "--------------\n");                                       \
    time_taken = -10;                                                          \
    goto cleanup;                                                              \
  }

#define BUFFER_SIZE 1024
#define SMALL_BUFFER_SIZE 200

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
  char dump_filename[total_filename_str_size + 1];

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
  while ((bytes_read = fread(sml_buffer, 1, SMALL_BUFFER_SIZE, fp)) > 0) {
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

  int check = 0;
  char buffer[BUFFER_SIZE];
  while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    for (size_t i = 0; i < bytes_read; i++) {
      row += (buffer[i] == '\n');
    }
    check = buffer[bytes_read - 1];
  }
  if (ferror(fp)) {
    fprintf(stderr, "Error reading the file\n");
    time_taken = -8;
    goto cleanup;
  }
  row += (check != '\n');

  if (fseek(fp, file_idx, SEEK_SET)) {
    fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
    fclose(fp);
    return -7;
  }

  size_t char_start = 0;

  csv_mat = Matrix_create(row, col);
  int read_columns = 0;
  int csv_indx = -1;
  double num = 0;

  while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    char_start = 0;
    for (size_t i = 0; i < bytes_read; i++) {
      if ((buffer[i] == ',') || (buffer[i] == '\n')) {
        char temp = buffer[i];
        buffer[i] = '\0';
        // for "" case it gives 0.0
        // "" case when ,, or ,\n thus missing value
        // case. Below handling this when not
        // encountered number. Saving those missing values
        // as NAN
        num = atof(&buffer[char_start]);

        if (num == 0.0) {
          // let's check for "" or "\r" as these two are
          // possible for missing value
          // '\r' cause i got this when i tried editing
          // csv with neovim
          if ((char_start == i) ||
              ((buffer[char_start] == '\r') && (i == (char_start + 1)))) {
            num = NAN;
          } else {
            // now we need the format of
            // (0 to n zeros)(either a dot or not)(if
            // dot then 0 to m zeros)
            // any other string then it is invalid

            VALIDATE_ZERO_STRING(char_start, i, temp);
          }
        }

        buffer[i] = temp;
        csv_indx++;
        csv_mat->data[csv_indx] = num;
        read_columns++;

        char_start = i + 1;

        if (buffer[i] == '\n') {
          if (read_columns != col) {
            fprintf(stderr, "Incorrect file format\n");
            time_taken = -3;
            goto cleanup;
          }
          read_columns = 0;
        }
      }
    }

    if (bytes_read == BUFFER_SIZE) {
      if ((buffer[bytes_read - 1] != '\n') || (buffer[bytes_read - 1] != ',')) {
        file_idx += char_start;
        if (fseek(fp, file_idx, SEEK_SET)) {
          fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
          time_taken = -7;
          goto cleanup;
        }
      } else {
        file_idx += bytes_read;
      }
    } else if (buffer[bytes_read - 1] != '\n') {
      // now only possiblity is either a number
      // or any other unexpected char
      buffer[bytes_read] = '\0';
      num = atof(&buffer[char_start]);

      if (num == 0.0) {
        if ((char_start == (bytes_read - 1) && buffer[char_start] == '\r') ||
            (char_start == bytes_read)) {
          num = NAN;
        } else {
          VALIDATE_ZERO_STRING(char_start, bytes_read, '\0');
        }
      }

      csv_indx++;
      csv_mat->data[csv_indx] = num;
      read_columns++;

      if (read_columns != col) {
        fprintf(stderr, "Inside Ending with non \\n\n");
        fprintf(stderr, "Incorrect file format\n");
        time_taken = -4;
        goto cleanup;
      }
    }
  }

  long end = clock();
  time_taken = (end - start);

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
