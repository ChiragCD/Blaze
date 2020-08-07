#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "global.h"

citizen * get_obj_citizen(var * variable, char * str) {
	for(int i = 0; i < variable->num_citizens; i++) {
		if(strcmp(str, variable->citizens[i]->name)) continue;
		return variable->citizens[i];
	}
	return NULL;
}

void assign (var * variable, val * value) {
	variable -> value = value;
}

val * copy_val (val * source) {
	val * v = (val *) malloc(sizeof(val));
	v->size = source->size;
	v->value = (char *) malloc(v->size * sizeof(char));
	return v;
}

var * deep_copy_var(var * source) {
	var * v = (var *) malloc(sizeof(var));
	v->name = NULL;
	v->num_citizens = source->num_citizens;
	v->value = copy_val(source->value);
	v->citizens = (citizen **) malloc(v->num_citizens * sizeof(var *));
	for(int i = 0; i < v->num_citizens; i++) {
		v->citizens[i] = deep_copy(source->citizens[i]);
		v->citizens[i]->name = source->citizens[i]->name;
	}
	return v;
}

var * shallow_copy_var(var * source) {
	var * v = (var *) malloc(sizeof(var));
	v->name = NULL;
	v->num_citizens = source->num_citizens;
	v->value = source->value;
	v->citizens = (citizen **) malloc(v->num_citizens * sizeof(var *));
	for(int i = 0; i < v->num_citizens; i++) v->citizens[i] = source->citizens[i];
	return v;
}

citizen * shallow_copy(citizen * source) {
	citizen * cz = (citizen *) malloc(sizeof(citizen));
	cz->type = source->type;
	if(source->type == VARIABLE) cz->variable = shallow_copy_var(source->variable);
	if(source->type == FUNCTION) cz->function = source->function;
	return cz;
}

citizen * deep_copy(citizen * source) {
	citizen * cz = (citizen *) malloc(sizeof(citizen));
	cz->type = source->type;
	if(source->type == VARIABLE) cz->variable = deep_copy_var(source->variable);
	if(source->type == FUNCTION) cz->function = source->function;
	return cz;
}

int truth_value (var * v) {
	if(v->value && v->value->value[0]) return 1;
	return 0;
}

void gf_print(namespace * n) {
	printf("Hello World\n");
}

void gf_print_val(namespace * n) {
	citizen * printable = get_citizen(n, "printable", NULL);
	char * value;
	if(printable->type == FUNCTION) value = printable->name;
	else if(printable->type == VARIABLE) value = printable->variable->value->value;
	printf("%s\n", value);
}

void gf_assign(namespace * n) {
	citizen * variable = get_citizen(n, "variable", NULL);
	citizen * value = get_citizen(n, "value", NULL);
	variable->type = value->type;
	if(value->type == VARIABLE) {
		var * v;
		if(value->variable->num_citizens) v = shallow_copy_var(value->variable);
		else v = deep_copy_var(value->variable);
		variable->variable = v;
	}
	if(value->type == FUNCTION) variable->function = value->function;
}

void call_global(char * name, int num_references, int num_values, namespace * n) {
	int is_global;
	if(strcmp(name, "print") == 0 && num_references == 0 && num_values == 0) gf_print(n);
	if(strcmp(name, "print") == 0 && num_references == 0 && num_values == 1) gf_print_val(n);
	if(strcmp(name, "assign") == 0 && num_references == 1 && num_values == 1) gf_assign(n);
}

char ** get_parameters(char * params, int total) {
	if(total == 0) return NULL;
	char ** parameters = (char **) malloc(total * sizeof(char *));
	for(int i = 0; i < total; i++) {
		char * limit = strchr(params, '|');
		if(!limit) {
			printf("Parameter generation error\n");
			return NULL;
		}

		int length = limit - params;
		char * parameter = (char *) malloc((length + 1) * sizeof(char));
		for(int j = 0; j < length; j++) parameter[j] = params[j];
		parameter[length] = '\0';

		parameters[i] = parameter;
		params = limit + 1;
	}
	return parameters;
}

citizen * generate_global_func(char * name, int num_references, int num_values, char * params) {
	citizen * cz = (citizen *) malloc(sizeof(citizen));
	cz->type = FUNCTION;
	cz->function = (func *) malloc(sizeof(func));

	cz->function->name = (char *) malloc((strlen(name) + 1) * sizeof(char));
	strcpy(cz->function->name, name);
	cz->name = cz->function->name;

	cz->function->outer = NULL;
	cz->function->num_references = num_references;
	cz->function->num_values = num_values;
	cz->function->parameters = get_parameters(params, num_references + num_values);
	return cz;
}

namespace * generate_global_namespace () {
	namespace * n = (namespace *) malloc(sizeof(namespace));
    for(int i = 0; i < 64; i++) n->buckets[i] = NULL;
	n->outer = NULL;
	citizen * cz;
	cz = generate_global_func("print", 0, 0, "|");
	put_citizen(n, cz);
	cz = generate_global_func("print", 0, 1, "printable|");
	put_citizen(n, cz);
	cz = generate_global_func("__main__", 0, 0, "|");
	put_citizen(n, cz);
	cz = generate_global_func("assign", 1, 1, "variable|value|");
	put_citizen(n, cz);
	return n;
}