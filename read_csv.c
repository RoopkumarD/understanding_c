#include <stdio.h>
#include <stdlib.h>
#include "/home/timepass/tid/pcd/mat_c/mat.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: executable filename");
		return 1;
	}

	FILE *fp = fopen(argv[1], "r");
	int c;
	int row, col;
	row = col = 0;

	while ((c = getc(fp)) != '\n') {
		if (c == ',') {
			col++;
		}
	}
	// for last col where it encounters \n than comma
	col++;

	// to use fseek later to come back after finding all rows
	int curr_posn = ftell(fp);

	int check = -1;
	while ((c = getc(fp)) != EOF) {
		if (c == '\n') {
			row++;
		}
		check = c;
	}
	// for last element in right below corner when it encounters
	// EOF than '\n'
	// but there is a problem which is if the last element does indeed
	// have \n then, below will be extra
	// i have to compare if last elem is \n or not but can't use c as it is EOF now
	if (check != '\n') {
		row++;
	}

	// now coming back and reading the file and making matrix
	fseek(fp, curr_posn, SEEK_SET);

	Matrix *csv_mat = Matrix_create(row, col);
	int csv_indx = -1;

	char buffer[10];
	for (int i = 0; i < 10; i++) {
		buffer[i] = '0';
	}
	int indx = 0;
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
			for (int j = 0; j < indx; j++) {
				buffer[j] = '0';
			}
			indx = 0;
			decimal_point_used = 0;
		} else {
			buffer[indx] = c;
			if (c == '.') {
				decimal_point_used = 1;
			}
			indx++;
		}

		check = c;
	}
	// for cases where file doesn't end with \n
	if (check != '\n') {
		// then last saved elem to matrix
		num = atof(buffer);
		csv_indx++;
		csv_mat->data[csv_indx] = num;
	}

	matrix_print(csv_mat);

	matrix_free(csv_mat);
	fclose(fp);

	return 0;
}
