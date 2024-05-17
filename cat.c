#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	FILE *fp;
	// why defining function inside with those args?
	void filecopy(FILE *, FILE *);
	char *progname = argv[0];

	if (argc == 1) {
		filecopy(stdin, stdout);
	} else {
		while (--argc > 0) {
			if ((fp = fopen(*++argv, "r")) == NULL) {
				fprintf(stderr, "%s: can't open %s\n", progname, *argv);
				exit(1);
			} else {
				filecopy(fp, stdout);
				fclose(fp);
			}
		}
	}
	if (ferror(stdout)) {
		fprintf(stderr, "%s: error with writing to stdout\n", progname);
		exit(2);
	}

	exit(0);
}

void filecopy(FILE *ifp, FILE *ofp) {
	int c;
	while ((c = getc(ifp)) != EOF) {
		putc(c, ofp);
	}
	return;
}
