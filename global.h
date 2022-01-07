#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct val val;
typedef struct var var;
typedef struct func func;
typedef enum citizen_type citizen_type;
typedef struct citizen citizen;

#include "parser.h"

struct val {
	int size;
	char * value;
};

struct func {
	char * name;
	int num_references;
	int num_values;
    char ** parameters;
    block * action;
    namespace * outer;
};

struct var {
	char * name;
	val * value;
	int num_citizens;
	citizen ** citizens;
};

enum citizen_type {
	FUNCTION, VARIABLE
};

struct citizen {
	citizen_type type;
	char * name;
	union {
		var * variable;
		func * function;
	};
};

citizen * get_obj_citizen(var * variable, char * str);

void assign(var * variable, val * value);
var * deep_copy_var(var * source);
var * shallow_copy_var(var * source);
citizen * shallow_copy(citizen * source);
citizen * deep_copy(citizen * source);

int truth_value(var * v);

void call_global(char * name, int num_references, int num_values, namespace * n);

namespace * generate_global_namespace();
void clean_global_namespace(namespace * n);

#endif