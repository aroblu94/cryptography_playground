#include <stdio.h>
#include <gmp.h>
#include <string.h>

int main(void) {
	int c;
	FILE *file;
	file = fopen("db.bin", "r");
	if (file) {
		while ((c = getc(file)) != EOF)
			putchar(c);
		fclose(file);
	}

	return 0;
}