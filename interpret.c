#include <stdlib.h>
#include <stdio.h>

#include "interpret.h"

void interpret_block(block * b, namespace * n);
citizen * interpret_returnable(returnable * r, namespace * n);

void call_func (func * f, citizen * parameters, int * is_global) {
    namespace * n = (namespace *) malloc(sizeof(namespace));
    for(int i = 0; i < 64; i++) n->buckets[i] = NULL;
    n->outer = f->outer;
    n->num_elements = 0;

    for(int i = 0; i < f->num_references + f->num_values; i++) put_citizen(n, parameters + i);
    
    if(is_global) call_global(f->name, f->num_references, f->num_values, n);
    else interpret_block(f->action, n);

    for(int i = 0; i < f->num_references + f->num_values; i++) del_citizen(n, parameters + i);
    if(n->num_elements) printf("Error - Unremoved elements in namespace\n");
    free(n);
}

void interpret_call (func_call * c, namespace * n_out) {

    identifier * id = c->func_name;
    int is_global = 0;

    func * f;
    if(id->attr_depth == 1) {
        f = get_callable(n_out, id->attrs[0], c->num_references, c->num_values, &is_global);
        if(!f) {
            printf("Error - func not found\n");
            return;
        }
    }
    else {
        citizen * cz = get_citizen(n_out, id->attrs[0], NULL);
        for(int i = 1; i < id->attr_depth; i++) {
            if(!cz) {
                printf("Error - Identifier not found\n");
                return;
            }
            cz = get_obj_citizen(cz->variable, id->attrs[i]);
        }
        if(cz->type != FUNCTION) {
            printf("Error - func not found\n");
            return;
        }
        f = cz->function;
    }

    namespace * n = (namespace *) malloc(sizeof(namespace));
    for(int i = 0; i < 64; i++) n->buckets[i] = NULL;
    n->outer = f->outer;
    n->num_elements = 0;

    citizen * members[f->num_references + f->num_values];

    for(int i = 0; i < c->num_references; i++) {
        citizen * cz = interpret_returnable(c->parameters[i], n);
        if(c->parameters[i]->type == IDENT) cz = shallow_copy(cz);
        cz->variable->name = f->parameters[i];
        cz->name = cz->variable->name;
        put_citizen(n, cz);
        members[i] = cz;
    }
    for(int i = c->num_references; i < c->num_references + c->num_values; i++) {
        citizen * cz = interpret_returnable(c->parameters[i], n);
        if(c->parameters[i]->type == IDENT || c->parameters[i]->type == FUNC) cz = deep_copy(cz);
        cz->variable->name = f->parameters[i];
        cz->name = cz->variable->name;
        put_citizen(n, cz);
        members[i] = cz;
    }

    if(is_global) call_global(f->name, c->num_references, c->num_values, n);
    else interpret_block(f->action, n);

    for(int i = 0; i < c->num_references + c->num_values; i++) del_citizen(n, members[i]);

    for(int i = 0; i < c->num_references; i++) free_shallow_citizen(members[i]);
    for(int i = c->num_references; i < c->num_references + c->num_values; i++) free_citizen(members[i]);

    if(n->num_elements) printf("Error - Unremoved elements in namespace, %s\n", c->func_name->attrs[0]);

    free(n);
}

citizen * interpret_def(func_def * d, namespace * n) {
    func * f = (func *) malloc(sizeof(func));
    f->name = d->func_name;
    f->num_references = d->num_references;
    f->num_values = d->num_values;
    f->parameters = d->parameters;
    f->action = d->action;
    f->outer = n;
    citizen * cz = (citizen *) malloc(sizeof(citizen));
    cz->type = FUNCTION;
    cz->function = f;
    cz->name = f->name;
    put_citizen(n, cz);
    return cz;
}

void interpret_statement(statement * s, namespace * n) {
    if(s->type == FUNC_CALL) interpret_call(s->call, n);
    else if(s->type == BLOCK) interpret_block(s->blk, n);
}

void interpret_block(block * b, namespace * n) {
    int num_funcs = 0, temp = 0;
    for(int i = 0; i < b->num_statements; i++) if(b->statements[i]->type == FUNC_DEF) num_funcs++;
    citizen * funcs[num_funcs];
    for(int i = 0; i < b->num_statements; i++) if(b->statements[i]->type == FUNC_DEF) funcs[temp++] = interpret_def(b->statements[i]->def, n);
    for(int i = 0; i < b->num_statements; i++) {
        interpret_statement(b->statements[i], n);
    }
    for(int i = 0; i < num_funcs; i++) del_citizen(n, funcs[i]);
    for(int i = 0; i < num_funcs; i++) free_citizen(funcs[i]);
}

citizen * interpret_returnable(returnable * r, namespace * n) {

    citizen * cz = (citizen *) malloc(sizeof(citizen));

    if(r->type == FUNC) {
        ;
    }

    if(r->type == IDENT) {
        ;
    }

    if(r->type == CONST) {
        cz->type = VARIABLE;
        cz->variable = (var *) malloc(sizeof(var));
        cz->variable->num_citizens = 0;
        cz->variable->citizens = (citizen **) malloc(0);
        val * value = (val *) malloc(sizeof(val));
        value->size = r->constant->size;
        value->value = (char *) malloc((value->size + 1) * sizeof(char));
        for(int i = 0; i < value->size; i++) value->value[i] = r->constant->val[i];
        value->value[value->size] = '\0';
        assign(cz->variable, value);
    }
    return cz;
}