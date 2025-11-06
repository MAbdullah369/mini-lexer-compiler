%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
%}

%%

"int"            return INT;
"float"          return FLOAT_TYPE;
"double"         return DOUBLE;
"char"           return CHAR;
"bool"           return BOOL;
"void"           return VOID;
"string"         return STRING;
"if"             return IF;
"else"           return ELSE;
"while"          return WHILE;
"for"            return FOR;
"return"         return RETURN;
"break"          return BREAK;
"continue"       return CONTINUE;
"cout"           return COUT;
"cin"            return CIN;
"endl"           return ENDL;
"true"           return TRUE;
"false"          return FALSE;

"+"              return PLUS;
"-"              return MINUS;
"*"              return MULTIPLY;
"/"              return DIVIDE;
"%"              return MODULO;

"="              return ASSIGN;
"=="             return EQ;
"!="             return NE;
"<"              return LT;
"<="             return LE;
">"              return GT;
">="             return GE;

"&&"             return AND;
"||"             return OR;
"!"              return NOT;

"++"             return INCREMENT;
"--"             return DECREMENT;

"<<"             return OUTPUT;
">>"             return INPUT;

"("              return LPAREN;
")"              return RPAREN;
"{"              return LBRACE;
"}"              return RBRACE;
"["              return LBRACKET;
"]"              return RBRACKET;
";"              return SEMICOLON;
","              return COMMA;
":"              return COLON;

[0-9]+           { yylval.int_val = atoi(yytext); return INTEGER; }
[0-9]+\.[0-9]*   { yylval.double_val = atof(yytext); return FLOAT; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.string_val = strdup(yytext); return IDENTIFIER; }
\"([^"\\]|\\["\\])*\" { yylval.string_val = strdup(yytext); return STRING_LITERAL; }
'[^']'          { yylval.string_val = strdup(yytext); return CHAR_LITERAL; }

[ \t\n]          ;
.                { return yytext[0]; }

%%

int yywrap(void) {
    return 1;
}