#ifndef SCANNER_H
#define SCANNER_H

typedef enum lexeme {
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_SQUARE, RIGHT_SQUARE, PIPE, COMMA, DOT,
	IDENTIFIER, CONSTANT, COLLECTION, TRUE, FALSE,
	SWITCH, CASE, WHILE,
	CONTINUE, BREAK, RETURN
} lexeme;

typedef struct token {
	lexeme lex;
	char * val;
	long size;
} token;

token * tokenize (char * content, long length, long * token_number);

#endif