#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scanner.h"

char * slice (char * str, int start, int stop) {
	char * temp = (char *) malloc((stop - start + 1) * sizeof(char));
	temp[stop - start] = '\0';
	for(int i = 0; i < stop - start; i++) {
		temp[i] = str[start + i];
		if(str[start + i] == '\0') break;
	}
	return temp;
}

int cmp (char * str, char * var) {
	int len = strlen(str);
	char * var_slice = slice(var, 0, len);
	if(strcmp(str, var_slice) == 0) {
		free(var_slice);
		return 1;
	}
	free(var_slice);
	return 0;
}

int is_valid_for_ident (char c) {
	int valid = 0;
	valid = valid || (c >= 'A' && c <= 'Z');
	valid = valid || (c >= 'a' && c <= 'z');
	valid = valid || (c >= '0' && c <= '9');
	valid = valid || c == '_';
	return valid;
}

void add(token * tokens, lexeme lex, char * val, long size, long * token_number) {
	token tok;
	tok.lex = lex;
	tok.val = val;
	tok.size = size;
	tokens[*token_number] = tok;
	(*token_number)++;
}

int single_char (char * content, token * tokens, long * token_number) {
	switch(*content) {
		case '(':
			add(tokens, LEFT_PAREN, NULL, 0L, token_number);
			return 1;
			break;
		case ')':
			add(tokens, RIGHT_PAREN, NULL, 0L, token_number);
			return 1;
			break;
		case '{':
			add(tokens, LEFT_BRACE, NULL, 0L, token_number);
			return 1;
			break;
		case '}':
			add(tokens, RIGHT_BRACE, NULL, 0L, token_number);
			return 1;
			break;
		case '[':
			add(tokens, LEFT_SQUARE, NULL, 0L, token_number);
			return 1;
			break;
		case ']':
			add(tokens, RIGHT_SQUARE, NULL, 0L, token_number);
			return 1;
			break;
		case '|':
			add(tokens, PIPE, NULL, 0L, token_number);
			return 1;
			break;
		case ',':
			add(tokens, COMMA, NULL, 0L, token_number);
			return 1;
			break;
		case '.':
			add(tokens, DOT, NULL, 0L, token_number);
			return 1;
			break;
	}
	return 0;
}

int ignore (char * content) {
	if(*content == ' ' || *content == '\n' || *content == '\r' || *content == '\t' || *content == '\0') return 1;
	return 0;
}

int is_keyword (char * content, token * tokens, long * token_number) {
	if(cmp("switch", content) && !is_valid_for_ident(content[6])) {
		add(tokens, SWITCH, NULL, 0L, token_number);
		return 6;
	}
	// printf("	In key, not switch\n");
	if(cmp("case", content) && !is_valid_for_ident(content[4])) {
		add(tokens, CASE, NULL, 0L, token_number);
		return 4;
	}
	// printf("	In key, not case\n");
	if(cmp("while", content) && !is_valid_for_ident(content[5])) {
		add(tokens, WHILE, NULL, 0L, token_number);
		return 5;
	}
	// printf("	In key, not loop\n");
	if(cmp("break", content) && !is_valid_for_ident(content[5])) {
		add(tokens, BREAK, NULL, 0L, token_number);
		return 5;
	}
	if(cmp("continue", content) && !is_valid_for_ident(content[8])) {
		add(tokens, CONTINUE, NULL, 0L, token_number);
		return 8;
	}
	if(cmp("return", content) && !is_valid_for_ident(content[6])) {
		add(tokens, RETURN, NULL, 0L, token_number);
		return 6;
	}
	// printf("	In key, not ret\n");
	if(cmp("true", content) && !is_valid_for_ident(content[4])) {
		add(tokens, TRUE, NULL, 0L, token_number);
		return 4;
	}
	// printf("	In key, not true\n");
	if(cmp("false", content) && !is_valid_for_ident(content[5])) {
		add(tokens, FALSE, NULL, 0L, token_number);
		return 5;
	}
	// printf("	In key, no match\n");
	return 0;
}

int is_constant (char * content, token * tokens, long * token_number) {
	if(*content >= '0' && *content <= '9' || *content == '.') {
		int length = 1, flag_point_found = 0;
		while(*(content + length) >= '0' && *(content + length) <= '9' || *(content + length) == '.') {
			if(*(content + length) == '.') {
				if(flag_point_found) break;
				flag_point_found = 1;
			}
			length++;
		}
		char * value = (char *) malloc((length + 1) * sizeof(char));
		for(int i = 0; i < length; i++) value[i] = content[i];
		value[length] = '\0';
		add(tokens, CONSTANT, value, (long)length, token_number);
		return length;
	}

	int size = 0;
	if(*content == '"') {
		size++;
		while(content[size] != '\0' && (content[size] != '"' || (content[size - 1] == '\\' && content[size - 2] != '\\'))) size++;
		size++;
	}
	if(*content == '\'') {
		size++;
		while(content[size] != '\0' && (content[size] != '\'' || (content[size - 1] == '\\' && content[size - 2] != '\\'))) size++;
		size++;
	}
	if(size) {
		size -= 2;
		char * value = (char *) malloc((size + 1) * sizeof(char));
		int tot = 0;
		for(int i = 1; i <= size; i++) {
			if(*(content + i) != '\\') {
				value[tot] = *(content + i);
				tot++;
				continue;
			}
			i++;
			switch(*(content + i)) {
				case 'n' : value[tot] = '\n'; break;
				case 't' : value[tot] = '\t'; break;
				case '\\' : value[tot] = '\\'; break;
				default : value[tot] = *(content + i); break;
			}
			tot++;
		}
		value[tot] = '\0';
		value = (char *) realloc(value, (tot + 1) * sizeof(char));
		add(tokens, CONSTANT, value, tot, token_number);
		size += 2;
	}
	return (int) size;
}

int is_identifier (char * content, token * tokens, long * token_number) {
	// printf("Checking ident\n");
	int length = 0;
	int valid = is_valid_for_ident(*(content + length));
	while(valid) {
		length += 1;
		valid = is_valid_for_ident(*(content + length));
	}
	// printf("Ident, valid length is %d\n", length);
	char * value = (char *) malloc((length + 1) * sizeof(char));
	for(int i = 0; i < length; i++)	value[i] = content[i];
	value[length] = '\0';
	if(length) add(tokens, IDENTIFIER, value, (long)length, token_number);
	else {
		length = 0;
		free(value);
	}
	return length;
}

long is_comment (char * content) {
	if(*content == '#') {
		long size = 1L;
		while(*(content + size) != '\n') size++;
		size++;
		return size;
	}
	if(cmp("\"\"\"", content)) {
		long size = 3L;
		while(*(content + size) != '\0' && !cmp("\"\"\"", content + size)) size++;
		size += 3;
		return size;
	}
	return 0L;
}

token * tokenize (char * content, long length, long * token_number) {
	*token_number = 0L;
	long i = 0L, capacity = 1L;
	token * tokens = (token *) malloc(capacity * sizeof(token));

	add(tokens, LEFT_BRACE, NULL, 0L, token_number);

	while(i < length) {
		if(*token_number == capacity) {
			// printf("Doubling\n");
			capacity *= 2;
			tokens = (token *) realloc(tokens, capacity * sizeof(token));
		}
		// printf("At char %c, ascii %d\n", *(content + i), *(content + i));
		if(ignore(content + i) || single_char(content + i, tokens, token_number)) {
			i++;
			continue;
		}
		// printf("Not ign or schar, wil check comm\n");
		long comment_length = is_comment(content + i);
		if(comment_length) {
			i += comment_length;
			continue;
		}
		// printf("Will cjeck key\n");
		int keyword_length = is_keyword(content + i, tokens, token_number);
		if(keyword_length) {
			i += keyword_length;
			continue;
		}
		// printf("will for const\n");
		int constant = is_constant(content + i, tokens, token_number);
		if(constant) {
			i += constant;
			continue;
		}
		// printf("Will check for ident\n");
		int identifier = is_identifier(content + i, tokens, token_number);
		if(identifier) {
			i += identifier;
			continue;
		}
		i++;
		printf("Not caught\n");
	}

	if(*token_number == capacity) tokens = (token *) realloc(tokens, (capacity + 1) * sizeof(token));
	add(tokens, RIGHT_BRACE, NULL, 0L, token_number);
	tokens = (token *) realloc(tokens, *token_number * sizeof(token));

	return tokens;
}