#ifndef PARSER_H
#define PARSER_H

typedef struct func_call func_call;
typedef struct func_def func_def;
typedef struct collection collection;
typedef struct identifier identifier;
typedef enum parameter parameter;
typedef struct returnable returnable;
typedef struct conditional conditional;
typedef struct loop loop;
typedef enum statement_type statement_type;
typedef struct statement statement;
typedef struct block block;

#include "scanner.h"
#include "namespace.h"

struct func_call {
    identifier * func_name;
    int num_references;
    int num_values;
    returnable ** parameters;
};

struct func_def {
    char * func_name;
    int num_references;
    int num_values;
    char ** parameters;
    block * action;
};

struct collection {
    int num_elements;
    returnable ** elements;
};

struct identifier {
    int attr_depth;
    char ** attrs;
};


enum parameter {
    FUNC, IDENT, COLL, CONST
};

struct returnable {
    parameter type;
    func_call * returned;
    identifier * value;
    collection * array;
    token * constant;
};

struct conditional {
    returnable * condition;
    int num_cases;
    returnable ** cases;
    block ** actions;
};

struct loop {
    returnable * condition;
    block * action;
};

enum statement_type {
    FUNC_CALL, FUNC_DEF, COND, LOOP, BLOCK,
    COMMAND_CONTINUE, COMMAND_BREAK, COMMAND_RETURN
};

struct statement {
    statement_type type;
    func_call * call;
    func_def * def;
    conditional * cond;
    loop * loop;
    block * blk;
};

struct block {
    int num_statements;
    statement ** statements;
    namespace * names;
};

block * read_block (token * tokens, int * error);
void display(block b, int forward);

#include "release.h"

#endif