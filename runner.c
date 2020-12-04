#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "global.h"
#include "fileops.h"
#include "scanner.h"
#include "parser.h"
#include "namespace.h"
#include "interpret.h"

int main (int argc, char * argv[]) {

	printf("Starting\n");

	long filesize;
	char * content = loadfile(argv[1], &filesize);
	// printf("%s\n", content);

	long token_num = 0L;
	token * tokens = tokenize(content, filesize, &token_num);
	free(content);
	printf("%ld tokens\n", token_num);

	int error = 0;
	block * __main__action = read_block(tokens, &error);
	for(long l = 0; l < token_num; l++) free_token(tokens + l);
	free(tokens);
	if(error == -1) {
		printf("Error\n");
		return 0;
	}
	// printf("Safe\n");

	// display(*__main__action, 0);
	
	namespace * global = generate_global_namespace();
	citizen * cz_main = get_citizen(global, "__main__", NULL);
	cz_main->function->action = __main__action;
	cz_main->function->outer = global;
	call_func(cz_main->function, NULL, NULL);

	free_block(__main__action);
	purge_namespace(global);

	return 0;
}
