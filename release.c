#include <stdlib.h>

#include "release.h"

void free_block(block * b);
void free_returnable(returnable * r);

void free_token(token * t) {
    if(!t) return;
    free(t->val);
}

void free_call(func_call * c) {
    if(!c) return;
    free_identifier(c->func_name);
    for(int i = 0; i < c->num_references + c->num_values; i++) free_returnable(c->parameters[i]);
    free(c->parameters);
    free(c);
}

void free_def(func_def * d) {
    if(!d) return;
    free(d->func_name);
    for(int i = 0; i < d->num_references + d->num_values; i++) free(d->parameters[i]);
    free(d->parameters);
    free_block(d->action);
    free(d);
}

void free_collection(collection * c) {
    if(!c) return;
    for(int i = 0; i < c->num_elements; i++) free_returnable(c->elements[i]);
    free(c->elements);
    free(c);
}

void free_identifier(identifier * o) {
    if(!o) return;
    for(int i = 0; i < o->attr_depth; i++) free(o->attrs[i]);
    free(o->attrs);
    free(o);
}

void free_returnable(returnable * r) {
    if(!r) return;
    if(r->type==FUNC) free_call(r->returned);
    else if(r->type==IDENT) free_identifier(r->value);
    else if(r->type==COLL) free_collection(r->array);
    else if (r->type==CONST) {
        free_token(r->constant);    
        free(r->constant);
    }
    free(r);
}

void free_conditional(conditional * c) {
    if(!c) return;
    free_returnable(c->condition);
    for(int i = 0; i < c->num_cases; i++) free_returnable(c->cases[i]);
    for(int i = 0; i < c->num_cases; i++) free_block(c->actions[i]);
    free(c->cases);
    free(c->actions);
    free(c);
}

void free_loop(loop * l) {
    if(!l) return;
    free_returnable(l->condition);
    free_block(l->action);
    free(l);
}

void free_statement(statement * s) {
    if(!s) return;
    if(s->type == FUNC_CALL) free_call(s->call);
    if(s->type == FUNC_DEF) free_def(s->def);
    if(s->type == COND) free_conditional(s->cond);
    if(s->type == LOOP) free_loop(s->loop);
    if(s->type == BLOCK) free_block(s->blk);
    free(s);
}

void free_block(block * b) {
    if(!b) return;
    for(int i = 0; i < b->num_statements; i++) free_statement(b->statements[i]);
    free(b->statements);
    free(b);
}

void free_func(func * f) {
    if(!f) return;
    f->name = NULL;
    f->action = NULL;
    f->outer = NULL;
    f->parameters = NULL;
    free(f);
}

void free_var(var * v) {
    if(!v) return;
    free(v->name);
    for(int i = 0; i < v->num_citizens; i++) free_citizen(v->citizens[i]);
    free(v->citizens);
    free_val(v->value);
    free(v);
}

void free_shallow_var(var * v) {
    if(!v) return;
    free(v->citizens);
    free(v);
}

void free_val(val * v) {
    if(!v) return;
    free(v->value);
    free(v);
}

void free_citizen(citizen * cz) {
    if(!cz) return;
    if(cz->type == FUNCTION) free_func(cz->function);
    if(cz->type == VARIABLE) free_var(cz->variable);
    free(cz);
}

void free_shallow_citizen(citizen * cz) {
    if(!cz) return;
    if(cz->type == FUNCTION) free_func(cz->function);
    if(cz->type == VARIABLE) free_shallow_var(cz->variable);
    free(cz);
}