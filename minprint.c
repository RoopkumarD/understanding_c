#include <stdarg.h>
#include <stdio.h>

// ... for variable arguement like how printf handles variable args
// stdarg has below stuff to handle variable args
void minprintf(char *fmt, ...) {
	va_list ap;
	char *p, *sval;
	int ival;
	double dval;

	va_start(ap, fmt);
	for (p = fmt; *p; p++) {
		if (*p != '%') {
			putchar(*p);
			continue;
		}

		// this won't work as idk, it should have
		// like it should have increased the pointer value and
		// then dereferenced it, but it doesn't
		// switch (*(p++)) {
		switch (*++p) {
			case 'd':
				ival = va_arg(ap, int);
				printf("%d", ival);
				break;
			case 'f':
				dval = va_arg(ap, double);
				printf("%f", dval);
				break;
			case 's':
				for (sval = va_arg(ap, char*); *sval; sval++) {
					putchar(*sval);
				}
				break;
			default:
				putchar(*p);
				break;
		}
	}
	va_end(ap);

	return;
}

int main(void) {
	minprintf("Hello %d World %s %f\n", 3, "I am roop", 1000.0000);

	return 0;
}
