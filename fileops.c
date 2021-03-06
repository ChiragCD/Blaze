#include <stdlib.h>
#include <stdio.h>

#include "fileops.h"

char * loadfile (char * filename, long * sizeptr) {
	FILE * file = fopen(filename, "r");
	
	fseek(file, 0L, SEEK_END);
	*sizeptr = ftell(file);
	fseek(file, 0L, SEEK_SET);

	char * content = (char *) malloc((*sizeptr + 1) * sizeof(char));
//	content[*sizeptr] = '\n';
	content[*sizeptr] = '\0';
	for(long i = 0; i < *sizeptr; i++) {
		content[i] = (char) fgetc(file);
	}
	
	fclose(file);
	return content;
}
