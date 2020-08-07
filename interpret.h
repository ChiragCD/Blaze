#ifndef INTERPRET_H
#define INTERPRET_H

#include "global.h"

void interpret_block(block * b, namespace * n);
void call_func(func * f, citizen * parameters, int * is_global);

#endif