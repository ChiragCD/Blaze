#ifndef RELEASE_H
#define RELEASE_H

#include "scanner.h"
#include "global.h"

void free_token(token * t);

void free_call(func_call * c);
void free_def(func_def * d);
void free_collection(collection * c);
void free_identifier(identifier * o);
void free_returnable(returnable * r);
void free_conditional(conditional * c);
void free_loop(loop * l);
void free_statement(statement * s);
void free_block(block * b);

void free_func(func * f);
void free_var(var * v);
void free_val(val * v);
void free_citizen(citizen * cz);
void free_shallow_citizen(citizen * cz);

#endif