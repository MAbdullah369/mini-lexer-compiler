#ifndef TOKENS_H
#define TOKENS_H

#include <string>

enum TokenType {
    T_FUNCTION, T_INT, T_FLOAT, T_STRING, T_BOOL, T_RETURN,
    T_IDENTIFIER, T_INTLIT, T_STRINGLIT,
    T_ASSIGNOP, T_EQUALSOP, T_COMMA, T_SEMICOLON,
    T_PARENL, T_PARENR, T_BRACEL, T_BRACER,
    T_UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

#endif
