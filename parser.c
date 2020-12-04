#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

block * read_block(token * tokens, int * skip_tokens);
returnable * read_returnable(token * tokens, int * skip_tokens);
identifier * read_identifier(token * tokens, int * skip_tokens);

func_call * read_call (token * tokens, int * skip_tokens) {

    func_call * call = (func_call *) malloc(sizeof(func_call));
    call->func_name = NULL;
    call->num_references = 0;
    call->num_values = 0;
    call->parameters = (returnable **) malloc(0);

    identifier * id = read_identifier(tokens, skip_tokens);
    if(*skip_tokens < 0) {
        free_call(call);
        return NULL;
    }
    call->func_name = id;
    int i = *skip_tokens;

    if(tokens[i].lex != LEFT_PAREN) {
        *skip_tokens = -1;
        free_call(call);
        return NULL;
    }
    i++;
    int comma_flag = 1;
    while(tokens[i].lex != PIPE) {
        if(comma_flag && tokens[i].lex == COMMA) {
            *skip_tokens = -1;
            free_call(call);
            return NULL;
        }
        if(tokens[i].lex == COMMA) {
            comma_flag = 1;
            i++;
            continue;
        }
        if(!comma_flag && tokens[i].lex != COMMA) {
            *skip_tokens = -1;
            free_call(call);
            return NULL;
        }
        comma_flag = 0;
        // printf("%s %s %s\n", tokens[0].val, tokens[1].val, tokens[2].val);
        returnable * r = read_returnable(tokens + i, skip_tokens);
        if(*skip_tokens < 0) {
            free_call(call);
            return NULL;
        }
        i += *skip_tokens;
        call->parameters = (returnable **) realloc(call->parameters, (call->num_references + 1) * sizeof(returnable *));
        call->parameters[call->num_references] = r;
        call->num_references++;
    }
    if(comma_flag && call->num_references) {
        *skip_tokens = -1;
        free_call(call);
        return NULL;
    }
    comma_flag = 1;
    i++;
    call->num_values = call->num_references;
    while(tokens[i].lex != RIGHT_PAREN) {
        if(comma_flag && tokens[i].lex == COMMA) {
            *skip_tokens = -1;
            free_call(call);
            return NULL;
        }
        if(tokens[i].lex == COMMA) {
            comma_flag = 1;
            i++;
            continue;
        }
        if(!comma_flag && tokens[i].lex != COMMA) {
            *skip_tokens = -1;
            free_call(call);
            return NULL;
        }
        comma_flag = 0;
        returnable * r = read_returnable(tokens + i, skip_tokens);
        if(*skip_tokens < 0) {
            free_call(call);
            return NULL;
        }
        i += *skip_tokens;
        call->parameters = (returnable **) realloc(call->parameters, (call->num_values + 1) * sizeof(returnable *));
        call->parameters[call->num_values] = r;
        call->num_values++;
    }
    call->num_values -= call->num_references;
    if(comma_flag && call->num_values) {
        *skip_tokens = -1;
        free_call(call);
        return NULL;
    }
    i++;
    *skip_tokens = i;
    return call;
}

func_def * read_def (token * tokens, int * skip_tokens) {
    int i = 0;
    func_call * f = read_call(tokens, skip_tokens);
    if(*skip_tokens < 0) return NULL;
    i = *skip_tokens;
    if(f->func_name->attr_depth != 1) {
        *skip_tokens = -1;
        free_call(f);
        return NULL;
    }
    for(int i = 0; i < f->num_references; i++) {
        if(f->parameters[i]->type != IDENT || f->parameters[i]->value->attr_depth != 1) {
            *skip_tokens = -1;
            free_call(f);
            return NULL;
        }
        for(int j = 0; j < i; j++) {
            if(strcmp(f->parameters[j]->value->attrs[0], f->parameters[i]->value->attrs[0])) continue;
            printf("Duplicate identifiers in function definition\n");
            *skip_tokens = -1;
            free_call(f);
            return NULL;
        }
    }
    for(int i = f->num_references; i < f->num_references + f->num_values; i++) {
        if(f->parameters[i]->type != IDENT || f->parameters[i]->value->attr_depth != 1) {
            *skip_tokens = -1;
            free_call(f);
            return NULL;
        }
        for(int j = 0; j < i; j++) {
            if(strcmp(f->parameters[j]->value->attrs[0], f->parameters[i]->value->attrs[0])) continue;
            printf("Duplicate identifiers in function definition\n");
            *skip_tokens = -1;
            free_call(f);
            return NULL;
        }
    }
    block * b = read_block(tokens + i, skip_tokens);
    if(*skip_tokens < 0) {
        free_call(f);
        return NULL;
    }
    i += *skip_tokens;
    *skip_tokens = i;

    func_def * d = (func_def *) malloc(sizeof(func_def));
    d->func_name = (char *) malloc((strlen(f->func_name->attrs[0]) + 1) * sizeof(char));
    strcpy(d->func_name, f->func_name->attrs[0]);
    d->num_references = f->num_references;
    d->num_values = f->num_values;
    d->parameters = (char **) malloc(d->num_references + d->num_values * sizeof(char *));
    for(int i = 0; i < f->num_references; i++) {
        int l = strlen(f->parameters[i]->value->attrs[0]) + 1;
        d->parameters[i] = (char *) malloc(l * sizeof(char));
        strcpy(d->parameters[i], f->parameters[i]->value->attrs[0]);
    }
    for(int i = f->num_references; i < f->num_references + f->num_values; i++) {
        int l = strlen(f->parameters[i]->value->attrs[0]) + 1;
        d->parameters[i] = (char *) malloc(l * sizeof(char));
        strcpy(d->parameters[i], f->parameters[i]->value->attrs[0]);
    }
    d->action = b;
    
    free_call(f);
    return d;
}

collection * read_collection (token * tokens, int * skip_tokens) {
    if(tokens[0].lex != LEFT_SQUARE) {
        *skip_tokens = -1;
        return NULL;
    }
    int i = 1, comma_flag = 1;
    collection * c = (collection *) malloc(sizeof(collection));
    c->num_elements = 0;
    c->elements = (returnable **) malloc(0);
    while(tokens[i].lex != RIGHT_SQUARE) {
        if(comma_flag && tokens[i].lex == COMMA) {
            *skip_tokens = -1;
            free_collection(c);
            return NULL;
        }
        if(tokens[i].lex == COMMA) {
            comma_flag = 1;
            i++;
            continue;
        }
        if(!comma_flag && tokens[i].lex != COMMA) {
            *skip_tokens = -1;
            free_collection(c);
            return NULL;
        }
        comma_flag = 0;
        returnable * r = read_returnable(tokens + i, skip_tokens);
        if(*skip_tokens < 0) {
            free_collection(c);
            return NULL;
        }
        i += *skip_tokens;
        c->elements = (returnable **) realloc(c->elements, (c->num_elements + 1) * sizeof(returnable *));
        c->elements[c->num_elements] = r;
        c->num_elements++;
    }
    if(comma_flag && c->num_elements) {
        *skip_tokens = -1;
        free_collection(c);
        return NULL;
    }
    i++;
    *skip_tokens = i;
    return c;
}

identifier * read_identifier(token * tokens, int * skip_tokens) {
    int dot_flag = 1, i = 0;
    identifier * o = (identifier *) malloc(sizeof(identifier));
    o->attr_depth = 0;
    o->attrs = (char **) malloc (0);
    while(tokens[i].lex == DOT || tokens[i].lex == IDENTIFIER) {
        if(tokens[i].lex == IDENTIFIER && !dot_flag) break;
        if(tokens[i].lex == DOT && dot_flag) {
            free_identifier(o);
            *skip_tokens = -1;
            return NULL;
        }
        if(tokens[i].lex == DOT && !dot_flag) {
            dot_flag = 1; i++;
            continue;
        }
        dot_flag = 0;
        o->attrs = (char **) realloc(o->attrs, (o->attr_depth + 1) * sizeof(char *));
        o->attrs[o->attr_depth] = (char *) malloc((tokens[i].size + 1) * sizeof(char));
        strcpy(o->attrs[o->attr_depth], tokens[i].val);
        o->attr_depth++; i++;
    }
    if(i == 0 || dot_flag) {
        free_identifier(o);
        *skip_tokens = -1;
        return NULL;
    }
    *skip_tokens = i;
    return o;
}

returnable * read_returnable(token * tokens, int * skip_tokens) {
    returnable * ret = (returnable *) malloc(sizeof(returnable));
    func_call * call = read_call(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        ret->type = FUNC;
        ret->returned = call;
        return ret;
    }
    collection * coll = read_collection(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        ret->type = COLL;
        ret->array = coll;
        return ret;
    }
    identifier * id = read_identifier(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        ret->type = IDENT;
        ret->value = id;
        return ret;
    }
    if (tokens[0].lex == CONSTANT || tokens[0].lex == TRUE || tokens[0].lex == FALSE) {
        ret->type = CONST;
        ret->constant = (token *) malloc(sizeof(token));
        ret->constant->lex = tokens[0].lex;
        ret->constant->size = tokens[0].size;
        ret->constant->val = (char *) malloc((tokens[0].size + 1) * sizeof(char));
        if(tokens[0].lex == CONSTANT) strcpy(ret->constant->val, tokens[0].val);
        *skip_tokens = 1;
        return ret;
    }
    *skip_tokens = -1;
    free(ret);
    return NULL;
}

conditional * read_conditional(token * tokens, int * skip_tokens) {
    int bracket_flag = 0, i = 1;
    if(tokens[0].lex != SWITCH || tokens[1].lex != LEFT_PAREN) {
        *skip_tokens = -1;
        return NULL;
    }
    if(tokens[1].lex == LEFT_PAREN) {
        bracket_flag = 1;
        i = 2;
    }
    returnable * r = read_returnable(tokens + i, skip_tokens);
    if(*skip_tokens < 0) return NULL;
    i += *skip_tokens;
    if(bracket_flag && tokens[i].lex != RIGHT_PAREN) {
        *skip_tokens = -1;
        free_returnable(r);
        return NULL;
    }
    if(bracket_flag) i++;
    if(tokens[i].lex != LEFT_BRACE) {
        *skip_tokens = -1;
        free_returnable(r);
        return NULL;
    }
    i++;
    conditional * c = (conditional *) malloc(sizeof(conditional));
    c->condition = r;
    c->num_cases = 0;
    c->cases = (returnable **) malloc(0);
    c->actions = (block **) malloc(0);
    while(tokens[i].lex != RIGHT_BRACE) {
        if(tokens[i].lex != CASE) {
            *skip_tokens = -1;
            free_conditional(c);
            return NULL;
        }
        i++;
        bracket_flag = 0;
        if(tokens[i].lex == LEFT_PAREN) {
            bracket_flag = 1; i++;
        }
        returnable * r = read_returnable(tokens + i, skip_tokens);
        if(*skip_tokens < 0) {
            free_conditional(c);
            return NULL;
        }
        i += *skip_tokens;
        if(bracket_flag && tokens[i].lex != RIGHT_PAREN) {
            *skip_tokens = -1;
            free_conditional(c);
            return NULL;
        }
        if(bracket_flag) i++;
        block * b = read_block(tokens + i, skip_tokens);
        if(*skip_tokens < 0) {
            free_conditional(c);
            return NULL;
        }
        i += *skip_tokens;
        c->cases = (returnable **) realloc(c->cases, (c->num_cases + 1) * sizeof(returnable *));
        c->actions = (block **) realloc(c->actions, (c->num_cases + 1) * sizeof(block *));
        c->cases[c->num_cases] = r;
        c->actions[c->num_cases] = b;
        c->num_cases += 1;
    }
    i++;
    *skip_tokens = i;
    return c;
}

loop * read_loop (token * tokens, int * skip_tokens) {
    int bracket_flag = 0, i = 1;
    if(tokens[0].lex != WHILE) {
        *skip_tokens = -1;
        return NULL;
    }
    if(tokens[1].lex == LEFT_PAREN) {
        bracket_flag = 1;
        i=2;
    }
    returnable * r = read_returnable(tokens + i, skip_tokens);
    if(*skip_tokens < 0) return NULL;
    i += *skip_tokens;
    if(bracket_flag && tokens[i].lex != RIGHT_PAREN) {
        *skip_tokens = -1;
        free_returnable(r);
        return NULL;
    }
    if(bracket_flag) i++;
    block * b = read_block(tokens + i, skip_tokens);
    if(*skip_tokens < 0) {
        free_returnable(r);
        return NULL;
    }
    i += *skip_tokens;
    *skip_tokens = i;
    loop * l = (loop *) malloc(sizeof(loop));
    l->condition = r;
    l->action = b;
    return l;
}

statement * read_statement(token * tokens, int * skip_tokens) {
    statement * s = (statement *) malloc(sizeof(statement));
    *skip_tokens = 0;
    conditional * c = read_conditional(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        s->type = COND;
        s->cond = c;
        return s;
    }
    loop * l = read_loop(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        s->type = LOOP;
        s->loop = l;
        return s;
    }
    block * b = read_block(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        s->type = BLOCK;
        s->blk = b;
        return s;
    }
    func_def * d = read_def(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        s->type = FUNC_DEF;
        s->def = d;
        return s;
    }
    func_call * call = read_call(tokens, skip_tokens);
    if(*skip_tokens > 0) {
        s->type = FUNC_CALL;
        s->call = call;
        return s;
    }
    *skip_tokens = 1;
    if(tokens[0].lex == CONTINUE) {
        s->type = COMMAND_CONTINUE;
        return s;
    }
    if(tokens[0].lex == BREAK) {
        s->type = COMMAND_BREAK;
        return s;
    }
    if(tokens[0].lex == RETURN) {
        s->type = COMMAND_RETURN;
        return s;
    }
    printf("No match for statement\n");
    free(s);
    *skip_tokens = -1;
    return NULL;
}

block * read_block(token * tokens, int * skip_tokens) {
    int i = 1;
    if(tokens[0].lex != LEFT_BRACE) {
        *skip_tokens = -1;
        return NULL;
    }
    block * b = (block *) malloc(sizeof(block));
    b->num_statements = 0;
    b->statements = (statement **) malloc(0);
    while(tokens[i].lex != RIGHT_BRACE) {
        statement * s = read_statement(tokens + i, skip_tokens);
        if(*skip_tokens < 0) {
            free_block(b);
            return NULL;
        }
        i += *skip_tokens;
        b->statements = (statement **) realloc(b->statements, (b->num_statements + 1) * sizeof(statement *));
        b->statements[b->num_statements] = s;
        b->num_statements++;
    }
    i++;
    *skip_tokens = i;
    return b;
}

void display_re(returnable * r, int forward);

void display_call (func_call * call, int forward) {
    for(int i = 0; i < forward; i++) printf("\t");
    printf("%s\n", call->func_name->attrs[0]);
    for(int i = 0; i < call -> num_references + call -> num_values; i++) display_re(call->parameters[i], forward + 1);
}

void display_coll(collection * c, int forward) {
    for(int i = 0; i < c->num_elements; i++) display_re(c->elements[i], forward + 1);
}

void display_re(returnable * r, int forward) {
    if(r->type == IDENT) {
        for(int i = 0; i < forward; i++) printf("\t");
        for(int i = 0; i < r->value->attr_depth; i++) printf("%s.", r->value->attrs[i]);
        printf("\n");
    }
    else if(r->type == FUNC_CALL) display_call(r->returned, forward);
    else if(r->type == COLL) display_coll(r->array, forward);
    else if(r->type==CONST && r->constant->lex == CONSTANT) {
        for(int i = 0; i < forward; i++) printf("\t");
        printf("%s\n", r->constant->val);
    }
}

void display_s(statement * s, int forward) {
	switch(s->type) {
        case FUNC_CALL:
            display_call(s->call, forward);
            break;
        case FUNC_DEF:
            for(int i = 0; i < forward; i++) printf("\t");
            printf("Definition - \n");
            display(*(s->def->action), forward);
            break;
        case LOOP:
            for(int i = 0; i < forward; i++) printf("\t");
            printf("Loop - \n");
            display(*(s->loop->action), forward);
            break;
        case COND:
            for(int i = 0; i < forward; i++) printf("\t");
            printf("Cond - \n");
            for(int i =0; i < s->cond->num_cases; i++) display(*(s->cond->actions[i]), forward);
            break;
        case BLOCK:
            display(*(s->blk), forward);
            break;
        default:
            for(int i = 0; i < forward; i++) printf("\t");
            printf("Statement\n");
            break;
    }
}

void display(block b, int forward) {
	for(int i = 0; i < b.num_statements; i++) {
		display_s(b.statements[i], forward + 1);
	}
}