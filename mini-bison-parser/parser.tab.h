/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    STRING_LITERAL = 259,          /* STRING_LITERAL  */
    CHAR_LITERAL = 260,            /* CHAR_LITERAL  */
    INTEGER = 261,                 /* INTEGER  */
    FLOAT = 262,                   /* FLOAT  */
    INT = 263,                     /* INT  */
    FLOAT_TYPE = 264,              /* FLOAT_TYPE  */
    DOUBLE = 265,                  /* DOUBLE  */
    CHAR = 266,                    /* CHAR  */
    BOOL = 267,                    /* BOOL  */
    VOID = 268,                    /* VOID  */
    STRING = 269,                  /* STRING  */
    IF = 270,                      /* IF  */
    ELSE = 271,                    /* ELSE  */
    WHILE = 272,                   /* WHILE  */
    FOR = 273,                     /* FOR  */
    RETURN = 274,                  /* RETURN  */
    BREAK = 275,                   /* BREAK  */
    CONTINUE = 276,                /* CONTINUE  */
    COUT = 277,                    /* COUT  */
    CIN = 278,                     /* CIN  */
    ENDL = 279,                    /* ENDL  */
    PLUS = 280,                    /* PLUS  */
    MINUS = 281,                   /* MINUS  */
    MULTIPLY = 282,                /* MULTIPLY  */
    DIVIDE = 283,                  /* DIVIDE  */
    MODULO = 284,                  /* MODULO  */
    ASSIGN = 285,                  /* ASSIGN  */
    EQ = 286,                      /* EQ  */
    NE = 287,                      /* NE  */
    LT = 288,                      /* LT  */
    LE = 289,                      /* LE  */
    GT = 290,                      /* GT  */
    GE = 291,                      /* GE  */
    AND = 292,                     /* AND  */
    OR = 293,                      /* OR  */
    NOT = 294,                     /* NOT  */
    INCREMENT = 295,               /* INCREMENT  */
    DECREMENT = 296,               /* DECREMENT  */
    LPAREN = 297,                  /* LPAREN  */
    RPAREN = 298,                  /* RPAREN  */
    LBRACE = 299,                  /* LBRACE  */
    RBRACE = 300,                  /* RBRACE  */
    LBRACKET = 301,                /* LBRACKET  */
    RBRACKET = 302,                /* RBRACKET  */
    SEMICOLON = 303,               /* SEMICOLON  */
    COMMA = 304,                   /* COMMA  */
    COLON = 305,                   /* COLON  */
    OUTPUT = 306,                  /* OUTPUT  */
    INPUT = 307,                   /* INPUT  */
    TRUE = 308,                    /* TRUE  */
    FALSE = 309,                   /* FALSE  */
    LOWER_THAN_ELSE = 310          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 69 "parser.y"

    char* string_val;
    int int_val;
    double double_val;
    void* node_val;

#line 126 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
