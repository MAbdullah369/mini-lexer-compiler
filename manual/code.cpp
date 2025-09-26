// lexer_full.cpp

#include <bits/stdc++.h>
using namespace std;

enum class TokenType {
    T_FUNCTION, T_INT, T_FLOAT, T_STRING, T_BOOL, T_RETURN,
    T_IF, T_ELSE, T_WHILE, T_FOR,
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT,
    T_QUOTES,
    T_LINECOMMENT, T_BLOCKCOMMENT,
    T_PARENL, T_PARENR, T_BRACEL, T_BRACER, T_BRACKETL, T_BRACKETR, T_DOT, T_COMMA, T_SEMICOLON,
    T_ASSIGNOP, T_EQUALSOP, T_NOTEQUAL, T_LESS, T_LESSEQ, T_GREATER, T_GREATEREQ,
    T_PLUS, T_MINUS, T_MULTIPLY, T_DIVIDE,
    T_AND, T_OR, T_NOT,
    T_EOF, T_UNKNOWN
};

struct Token {
    TokenType type{};
    string lexeme;
    int line = 1;
    int col = 1;
};

static string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::T_FUNCTION: return "T_FUNCTION";
        case TokenType::T_INT: return "T_INT";
        case TokenType::T_FLOAT: return "T_FLOAT";
        case TokenType::T_STRING: return "T_STRING";
        case TokenType::T_BOOL: return "T_BOOL";
        case TokenType::T_RETURN: return "T_RETURN";
        case TokenType::T_IF: return "T_IF";
        case TokenType::T_ELSE: return "T_ELSE";
        case TokenType::T_WHILE: return "T_WHILE";
        case TokenType::T_FOR: return "T_FOR";
        case TokenType::T_IDENTIFIER: return "T_IDENTIFIER";
        case TokenType::T_INTLIT: return "T_INTLIT";
        case TokenType::T_FLOATLIT: return "T_FLOATLIT";
        case TokenType::T_STRINGLIT: return "T_STRINGLIT";
        case TokenType::T_QUOTES: return "T_QUOTES";
        case TokenType::T_LINECOMMENT: return "T_LINECOMMENT";
        case TokenType::T_BLOCKCOMMENT: return "T_BLOCKCOMMENT";
        case TokenType::T_PARENL: return "T_PARENL";
        case TokenType::T_PARENR: return "T_PARENR";
        case TokenType::T_BRACEL: return "T_BRACEL";
        case TokenType::T_BRACER: return "T_BRACER";
        case TokenType::T_BRACKETL: return "T_BRACKETL";
        case TokenType::T_BRACKETR: return "T_BRACKETR";
        case TokenType::T_DOT: return "T_DOT";
        case TokenType::T_COMMA: return "T_COMMA";
        case TokenType::T_SEMICOLON: return "T_SEMICOLON";
        case TokenType::T_ASSIGNOP: return "T_ASSIGNOP";
        case TokenType::T_EQUALSOP: return "T_EQUALSOP";
        case TokenType::T_NOTEQUAL: return "T_NOTEQUAL";
        case TokenType::T_LESS: return "T_LESS";
        case TokenType::T_LESSEQ: return "T_LESSEQ";
        case TokenType::T_GREATER: return "T_GREATER";
        case TokenType::T_GREATEREQ: return "T_GREATEREQ";
        case TokenType::T_PLUS: return "T_PLUS";
        case TokenType::T_MINUS: return "T_MINUS";
        case TokenType::T_MULTIPLY: return "T_MULTIPLY";
        case TokenType::T_DIVIDE: return "T_DIVIDE";
        case TokenType::T_AND: return "T_AND";
        case TokenType::T_OR: return "T_OR";
        case TokenType::T_NOT: return "T_NOT";
        case TokenType::T_EOF: return "T_EOF";
        default: return "T_UNKNOWN";
    }
}

static string tokenToDisplay(const Token &t) {
    switch (t.type) {
        case TokenType::T_IDENTIFIER:
            return "T_IDENTIFIER(\"" + t.lexeme + "\")";
        case TokenType::T_INTLIT:
            return "T_INTLIT(" + t.lexeme + ")";
        case TokenType::T_FLOATLIT:
            return "T_FLOATLIT(" + t.lexeme + ")";
        case TokenType::T_STRINGLIT:
            return "T_STRINGLIT(\"" + t.lexeme + "\")";
        case TokenType::T_LINECOMMENT:
            return "T_LINECOMMENT(\"" + t.lexeme + "\")";
        case TokenType::T_BLOCKCOMMENT:
            return "T_BLOCKCOMMENT(\"" + t.lexeme + "\")";
        default:
            return tokenTypeName(t.type);
    }
}

class Lexer {
public:
    explicit Lexer(string src) : source(move(src)) {}

    vector<Token> tokenize() {
        vector<Token> out;
        Token tk;
        do {
            tk = nextToken();
            out.push_back(tk);
        } while (tk.type != TokenType::T_EOF);
        return out;
    }

private:
    string source;
    size_t pos = 0;
    int line = 1;
    int col = 1;

    const unordered_map<string, TokenType> keywords = {
        {"fn", TokenType::T_FUNCTION},
        {"int", TokenType::T_INT},
        {"float", TokenType::T_FLOAT},
        {"string", TokenType::T_STRING},
        {"bool", TokenType::T_BOOL},
        {"return", TokenType::T_RETURN},
        {"if", TokenType::T_IF},
        {"else", TokenType::T_ELSE},
        {"while", TokenType::T_WHILE},
        {"for", TokenType::T_FOR},
    };

    deque<Token> pendingTokens;

    char peek() const { return pos < source.size() ? source[pos] : '\0'; }
    char peekNext() const { return (pos + 1) < source.size() ? source[pos + 1] : '\0'; }

    char advance() {
        char c = peek();
        if (c == '\0') return c;
        pos++;
        if (c == '\n') { line++; col = 1; }
        else col++;
        return c;
    }

    Token scanLineComment() {
        int startCol = col;
        string content;
        advance(); advance();
        while (peek() != '\n' && peek() != '\0') {
            content.push_back(advance());
        }
        return makeToken(TokenType::T_LINECOMMENT, content, startCol);
    }

    Token scanBlockComment() {
        int startCol = col;
        string content;
        advance(); advance();
        while (!(peek() == '*' && peekNext() == '/')) {
            if (peek() == '\0') throw runtime_error("Unterminated block comment (line " + to_string(line) + ")");
            content.push_back(advance());
        }
        advance(); advance();
        return makeToken(TokenType::T_BLOCKCOMMENT, content, startCol);
    }

    void skipWhitespaceOnly() {
        while (true) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                advance();
                continue;
            }
            break;
        }
    }

    static bool isIdentStart(unsigned char c) { return isalpha(c) || c == '_'; }
    static bool isIdentPart(unsigned char c) { return isalnum(c) || c == '_'; }

    Token makeToken(TokenType type, const string &lex = "", int startCol = -1) const {
        Token t;
        t.type = type;
        t.lexeme = lex;
        t.line = line;
        t.col = (startCol == -1 ? col : startCol);
        return t;
    }

    Token scanIdentifierOrKeyword() {
        int startCol = col;
        string lex;
        while (isIdentPart((unsigned char)peek())) lex.push_back(advance());
        auto it = keywords.find(lex);
        if (it != keywords.end()) return makeToken(it->second, lex, startCol);
        return makeToken(TokenType::T_IDENTIFIER, lex, startCol);
    }

    Token scanNumber() {
        int startCol = col;
        string digits;
        bool isFloat = false;
        bool dotSeen = false;

        // integer part (if any)
        while (isdigit((unsigned char)peek())) {
            digits.push_back(advance());
        }

        // fractional part
        if (peek() == '.') {
            dotSeen = true;
            digits.push_back(advance());
            isFloat = true;

            // allow trailing dot (12.)
            while (isdigit((unsigned char)peek())) {
                digits.push_back(advance());
            }
        }

        // ❌ if another dot appears => invalid float
        if (peek() == '.') {
            throw runtime_error("Lex error (line " + to_string(line) +
                ", col " + to_string(startCol) +
                "): multiple dots in number literal '" + digits + "'");
        }

        // check for identifiers starting with digit
        if (isIdentStart((unsigned char)peek())) {
            string tail;
            while (isIdentPart((unsigned char)peek())) tail.push_back(advance());
            throw runtime_error("Lex error (line " + to_string(line) +
                ", col " + to_string(startCol) +
                "): invalid identifier starting with digit: '" + digits + tail + "'");
        }

        return makeToken(isFloat ? TokenType::T_FLOATLIT : TokenType::T_INTLIT, digits, startCol);
    }


    void scanStringLiteral_emitting_quotes() {
    int startCol = col;
    advance(); // consume opening quote
    pendingTokens.push_back(makeToken(TokenType::T_QUOTES, "\"", startCol));

    string cleanValue;
    while (true) {
        char c = peek();
        if (c == '\0')
            throw runtime_error("Unterminated string literal (line " + to_string(line) + ")");
        if (c == '"') break;

        if (c == '\\') {
            advance(); // consume '\'
            char esc = advance(); // consume the escape character
            switch (esc) {
                case 'n': case 't': case 'r':
                case '\\': case '"': case '0':
                    // ✅ valid escape recognized
                    // but don't add anything to cleanValue
                    break;
                default:
                    throw runtime_error("Invalid escape sequence \\" + string(1, esc) +
                        " (line " + to_string(line) + ")");
            }
        } else {
            cleanValue.push_back(advance()); // keep normal characters
        }
    }

    int contentStartCol = startCol + 1;
    pendingTokens.push_back(makeToken(TokenType::T_STRINGLIT, cleanValue, contentStartCol));

    int rightQuoteCol = col;
    advance(); // consume closing quote
    pendingTokens.push_back(makeToken(TokenType::T_QUOTES, "\"", rightQuoteCol));
}


    Token scanOperatorOrPunct() {
        int startCol = col;
        char c = advance();
        char n = peek();
        switch (c) {
            case '.':
                if (n == '.') {
                // use startCol (already defined at top of function)
                throw runtime_error("Lex error (line " + to_string(line) +
                    ", col " + to_string(startCol) + "): invalid float literal starting with '..'");
            }
            return makeToken(TokenType::T_DOT, ".", startCol);                      

            case '=':
                if (n == '=') { advance(); return makeToken(TokenType::T_EQUALSOP, "==", startCol); }
                return makeToken(TokenType::T_ASSIGNOP, "=", startCol);
            case '!':
                if (n == '=') { advance(); return makeToken(TokenType::T_NOTEQUAL, "!=", startCol); }
                return makeToken(TokenType::T_NOT, "!", startCol);
            case '<':
                if (n == '=') { advance(); return makeToken(TokenType::T_LESSEQ, "<=", startCol); }
                return makeToken(TokenType::T_LESS, "<", startCol);
            case '>':
                if (n == '=') { advance(); return makeToken(TokenType::T_GREATEREQ, ">=", startCol); }
                return makeToken(TokenType::T_GREATER, ">", startCol);
            case '&':
                if (n == '&') { advance(); return makeToken(TokenType::T_AND, "&&", startCol); }
                return makeToken(TokenType::T_UNKNOWN, "&", startCol);
            case '|':
                if (n == '|') { advance(); return makeToken(TokenType::T_OR, "||", startCol); }
                return makeToken(TokenType::T_UNKNOWN, "|", startCol);
            case '(': return makeToken(TokenType::T_PARENL, "(", startCol);
            case ')': return makeToken(TokenType::T_PARENR, ")", startCol);
            case '{': return makeToken(TokenType::T_BRACEL, "{", startCol);
            case '}': return makeToken(TokenType::T_BRACER, "}", startCol);
            case '[': return makeToken(TokenType::T_BRACKETL, "[", startCol);
            case ']': return makeToken(TokenType::T_BRACKETR, "]", startCol);
            case ',': return makeToken(TokenType::T_COMMA, ",", startCol);
            case ';': return makeToken(TokenType::T_SEMICOLON, ";", startCol);
            case '"': return makeToken(TokenType::T_QUOTES, "\"", startCol);
            case '/': return makeToken(TokenType::T_DIVIDE, "/", startCol);
            case '+': return makeToken(TokenType::T_PLUS, "+", startCol);
            case '-': return makeToken(TokenType::T_MINUS, "-", startCol);
            case '*': return makeToken(TokenType::T_MULTIPLY, "*", startCol);
            default: return makeToken(TokenType::T_UNKNOWN, string(1, c), startCol);
        }
    }

    Token nextToken() {
        if (!pendingTokens.empty()) {
            Token t = pendingTokens.front();
            pendingTokens.pop_front();
            return t;
        }

        skipWhitespaceOnly();
        int startCol = col;
        char c = peek();
        if (c == '\0') return makeToken(TokenType::T_EOF, "", startCol);

        if (c == '/' && peekNext() == '/') return scanLineComment();
        if (c == '/' && peekNext() == '*') return scanBlockComment();

        if (isIdentStart((unsigned char)c)) return scanIdentifierOrKeyword();
        if (isdigit((unsigned char)c)) return scanNumber();
        if (c == '"') {
            scanStringLiteral_emitting_quotes();
            if (!pendingTokens.empty()) {
                Token t = pendingTokens.front();
                pendingTokens.pop_front();
                return t;
            }
        }
        return scanOperatorOrPunct();
    }
};

int main() {
    try {
        string code = R"(
        int main() {
            int a = ..12;      
            string name = "Abdullah\nzahid";  // escape preserved
            string bad = "abc\t"; // should throw invalid escape
        }
        )";

        Lexer lex(code);
        auto tokens = lex.tokenize();

        cout << "[";
        bool first = true;
        for (auto &t : tokens) {
            if (t.type == TokenType::T_EOF) continue;
            if (!first) cout << ", ";
            cout << tokenToDisplay(t);
            first = false;
        }
        cout << "]\n";
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
