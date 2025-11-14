// lexer_regex.cpp
#include <bits/stdc++.h>
using namespace std;

enum class TokenType
{
    T_FUNCTION,
    T_INT,
    T_FLOAT,
    T_STRING,
    T_BOOL,
    T_BOOLLIT, // ✅ for true/false literals

    T_RETURN,
    T_IF,
    T_ELSE,
    T_WHILE,
    T_FOR,
    T_DO, // <-- add this
    T_BREAK,
    T_IDENTIFIER,
    T_INTLIT,
    T_FLOATLIT,
    T_STRINGLIT,
    T_CHAR,    // <— NEW: type keyword 'char'
    T_CHARLIT, // ✅ add this
    T_LINECOMMENT,
    T_BLOCKCOMMENT,
    T_PARENL,
    T_PARENR,
    T_BRACEL,
    T_BRACER,
    T_BRACKETL,
    T_BRACKETR,
    T_COMMA,
    T_SEMICOLON,
    T_LSHIFT, // <<
    T_RSHIFT, // >>   // <-- add this
    T_ASSIGNOP,
    T_EQUALSOP,
    T_NOTEQUAL,
    T_LESS,
    T_LESSEQ,
    T_GREATER,
    T_GREATEREQ,
    T_PLUS,
    T_MINUS,
    T_MULTIPLY,
    T_DIVIDE,
    T_AND,
    T_OR,
    T_NOT,
    T_BITAND, // &
    T_BITOR,  // |
    T_BITXOR, // ^
    T_POWER,  // **
    T_EOF,
    T_PLUS_EQ,  // +=
    T_MINUS_EQ, // -=
    T_INC,      // ++
    T_DEC,      // --
    T_UNKNOWN

};

struct Token
{
    TokenType type{};
    string lexeme;
    int line = 1;
    int col = 1;
};

static string tokenTypeName(TokenType t)
{
    switch (t)
    {
    case TokenType::T_FUNCTION:
        return "T_FUNCTION";
    case TokenType::T_INT:
        return "T_INT";
    case TokenType::T_FLOAT:
        return "T_FLOAT";
    case TokenType::T_STRING:
        return "T_STRING";
    case TokenType::T_BOOL:
        return "T_BOOL";
    case TokenType::T_BOOLLIT:
        return "T_BOOLLIT";

    case TokenType::T_RETURN:
        return "T_RETURN";
    case TokenType::T_IF:
        return "T_IF";
    case TokenType::T_ELSE:
        return "T_ELSE";
    case TokenType::T_WHILE:
        return "T_WHILE";
    case TokenType::T_FOR:
        return "T_FOR";
    case TokenType::T_DO:
        return "T_DO";
    case TokenType::T_BREAK:
        return "T_BREAK";
    case TokenType::T_IDENTIFIER:
        return "T_IDENTIFIER";
    case TokenType::T_INTLIT:
        return "T_INTLIT";
    case TokenType::T_FLOATLIT:
        return "T_FLOATLIT";
    case TokenType::T_STRINGLIT:
        return "T_STRINGLIT";
    case TokenType::T_LINECOMMENT:
        return "T_LINECOMMENT";
    case TokenType::T_BLOCKCOMMENT:
        return "T_BLOCKCOMMENT";
    case TokenType::T_CHAR:
        return "T_CHAR";
    case TokenType::T_CHARLIT:
        return "T_CHARLIT";

    case TokenType::T_PARENL:
        return "T_PARENL";
    case TokenType::T_PARENR:
        return "T_PARENR";
    case TokenType::T_BRACEL:
        return "T_BRACEL";
    case TokenType::T_BRACER:
        return "T_BRACER";
    case TokenType::T_BRACKETL:
        return "T_BRACKETL";
    case TokenType::T_BRACKETR:
        return "T_BRACKETR";
    case TokenType::T_COMMA:
        return "T_COMMA";
    case TokenType::T_SEMICOLON:
        return "T_SEMICOLON";
    case TokenType::T_ASSIGNOP:
        return "T_ASSIGNOP";
    case TokenType::T_EQUALSOP:
        return "T_EQUALSOP";
    case TokenType::T_NOTEQUAL:
        return "T_NOTEQUAL";
    case TokenType::T_LESS:
        return "T_LESS";
    case TokenType::T_LESSEQ:
        return "T_LESSEQ";
    case TokenType::T_GREATER:
        return "T_GREATER";
    case TokenType::T_GREATEREQ:
        return "T_GREATEREQ";
    case TokenType::T_PLUS:
        return "T_PLUS";
    case TokenType::T_MINUS:
        return "T_MINUS";
    case TokenType::T_MULTIPLY:
        return "T_MULTIPLY";
    case TokenType::T_DIVIDE:
        return "T_DIVIDE";
    case TokenType::T_AND:
        return "T_AND";
    case TokenType::T_OR:
        return "T_OR";
    case TokenType::T_NOT:
        return "T_NOT";
    case TokenType::T_EOF:
        return "T_EOF";
    // in tokenTypeName(...) or tokenToDisplay(...)
    case TokenType::T_LSHIFT:
        return "T_LSHIFT";
    case TokenType::T_RSHIFT:
        return "T_RSHIFT"; // <-- add this
    case TokenType::T_PLUS_EQ:
        return "T_PLUS_EQ";
    case TokenType::T_MINUS_EQ:
        return "T_MINUS_EQ";
    case TokenType::T_INC:
        return "T_INC";
    case TokenType::T_DEC:
        return "T_DEC";
    case TokenType::T_BITAND:
        return "T_BITAND";
    case TokenType::T_BITOR:
        return "T_BITOR";
    case TokenType::T_BITXOR:
        return "T_BITXOR";
    case TokenType::T_POWER:
        return "T_POWER";

    default:
        return "T_UNKNOWN";
    }
}

static string tokenToDisplay(const Token &t)
{
    switch (t.type)
    {
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
    case TokenType::T_BOOLLIT:
        return "T_BOOLLIT(" + t.lexeme + ")";
    case TokenType::T_CHARLIT:
        return "T_CHARLIT('" + t.lexeme + "')";

    default:
        return tokenTypeName(t.type);
    }
}

class RegexLexer
{
public:
    explicit RegexLexer(string src) : source(move(src))
    {
        initializePatterns();
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;
        while (pos < source.length())
        {
            skipWhitespace();
            if (pos >= source.length())
                break;
            Token token = nextToken();
            if (token.type != TokenType::T_UNKNOWN)
            {
                tokens.push_back(token);
            }
            else
            {
                // Emit unknown and keep going (advance happens in nextToken’s logic)
                tokens.push_back(token);
                // optionally also log a warning here
            }
        }
        tokens.push_back(makeToken(TokenType::T_EOF));
        return tokens;
    }

private:
    string source;
    size_t pos = 0;
    int line = 1;
    int col = 1;

    regex whitespacePattern, lineCommentPattern, blockCommentPattern,
        floatPattern, badFloat1, badFloat2, intPattern, identifierPattern, stringPattern, charPattern, twoCharOpPattern;

    unordered_map<string, TokenType> keywords;

    void initializePatterns()
    {
        whitespacePattern = regex(R"(^[ \t\r\n]+)");
        lineCommentPattern = regex(R"(^//([^\r\n]*))");
        blockCommentPattern = regex(R"(^/\*([\s\S]*?)\*/)", regex_constants::ECMAScript);
        // Valid float: at least one digit before and after the dot
        floatPattern = regex(R"(^([0-9]+\.[0-9]+))");
        badFloat1 = regex(R"(^([0-9]+\.(?![0-9])))"); // invalid: 12.
        badFloat2 = regex(R"(^(\.[0-9]+))");          // invalid: .45
        intPattern = regex(R"(^(\d+))");
        identifierPattern = regex(R"(^([a-zA-Z_][a-zA-Z0-9_]*))");
        // ✅ Fixed string regex (handles escapes + requires closing quote)
        stringPattern = regex(R"delim(^"((?:[^"\\]|\\.)*)")delim", regex_constants::ECMAScript);
        charPattern = regex(R"(^'((?:[^'\\]|\\.)*)')", regex_constants::ECMAScript);

        // twoCharOpPattern = regex(R"(^(==|!=|<=|>=|&&|\|\|))");
        // change to include << :
        twoCharOpPattern = regex(R"(^(==|!=|<=|>=|&&|\|\|)|^(<<))");

        keywords = {
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
            {"do", TokenType::T_DO},
            {"break", TokenType::T_BREAK},
            {"char", TokenType::T_CHAR}, // <— NEW
            // ✅ ADD THESE:
            {"true", TokenType::T_BOOLLIT},
            {"false", TokenType::T_BOOLLIT},
        };
    }

    void skipWhitespace()
    {
        smatch match;
        string remaining = source.substr(pos);
        while (regex_search(remaining, match, whitespacePattern))
        {
            if (match.position() != 0)
                break;
            string matched = match.str();
            for (char c : matched)
            {
                if (c == '\n')
                {
                    line++;
                    col = 1;
                }
                else
                    col++;
            }
            pos += matched.length();
            remaining = source.substr(pos);
        }
    }

    Token makeToken(TokenType type, const string &lex = "")
    {
        return {type, lex, line, col};
    }

    string processEscapeSequences(const string &str)
    {
        string result;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (str[i] == '\\' && i + 1 < str.length())
            {
                char esc = str[i + 1];
                switch (esc)
                {
                case 'n':
                    result += '\n';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '"':
                    result += '"';
                    break;
                case '0':
                    result += '\0';
                    break;
                case '\'':
                    result += '\'';
                    break;

                default:
                    throw runtime_error("Invalid escape sequence \\" + string(1, esc) +
                                        " at line " + to_string(line));
                }
                i++; // skip escape
            }
            else
                result += str[i];
        }
        return result;
    }

    Token nextToken()
    {
        if (pos >= source.length())
            return makeToken(TokenType::T_EOF);
        string remaining = source.substr(pos);
        smatch match;
        int startCol = col;

        // --- block comment ---
        if (regex_search(remaining, match, blockCommentPattern) && match.position() == 0)
        {
            string content = match.str(1);
            pos += match.length();
            for (char c : match.str())
            {
                if (c == '\n')
                {
                    line++;
                    col = 1;
                }
                else
                    col++;
            }
            return makeToken(TokenType::T_BLOCKCOMMENT, content);
        }
        if (remaining.substr(0, 2) == "/*" && remaining.find("*/") == string::npos)
        {
            throw runtime_error("Unterminated block comment (line " + to_string(line) + ")");
        }

        // --- line comment ---
        if (regex_search(remaining, match, lineCommentPattern) && match.position() == 0)
        {
            string content = match.str(1);
            pos += match.length();
            col += match.length();
            return makeToken(TokenType::T_LINECOMMENT, content);
        }

        if (regex_search(remaining, match, charPattern) && match.position() == 0)
        {
            string inner = match.str(1);
            string decoded = processEscapeSequences(inner);
            if (decoded.size() != 1)
                throw runtime_error("Invalid character literal: must contain exactly one character at line " + to_string(line));
            pos += match.length();
            col += match.length();
            return makeToken(TokenType::T_CHARLIT, decoded);
        }
        if (remaining[0] == '\'')
        {
            throw runtime_error("Unterminated character literal (line " + to_string(line) + ")");
        }

        // --- string literal ---
        if (regex_search(remaining, match, stringPattern) && match.position() == 0)
        {
            string content = match.str(1);
            pos += match.length();
            col += match.length();
            return makeToken(TokenType::T_STRINGLIT, processEscapeSequences(content));
        }

        // If we see a quote but it doesn't match the pattern → check for newline
        if (remaining[0] == '"')
        {
            size_t newlinePos = remaining.find('\n');
            if (newlinePos != string::npos)
            {
                throw runtime_error("Unterminated string literal (line " + to_string(line) + ")");
            }
            else
            {
                throw runtime_error("Unterminated string literal (line " + to_string(line) + ")");
            }
        }

        // --- floats ---
        if (regex_search(remaining, match, badFloat1) && match.position() == 0)
        {
            throw runtime_error("Invalid float literal: missing digits after '.' at line " + to_string(line));
        }
        if (regex_search(remaining, match, badFloat2) && match.position() == 0)
        {
            throw runtime_error("Invalid float literal: missing digits before '.' at line " + to_string(line));
        }
        if (regex_search(remaining, match, floatPattern) && match.position() == 0)
        {
            string value = match.str(1);
            pos += match.length();
            col += match.length();
            return makeToken(TokenType::T_FLOATLIT, value);
        }

        // --- int ---
        if (regex_search(remaining, match, intPattern) && match.position() == 0)
        {
            string value = match.str(1);
            size_t nextPos = pos + match.length();
            if (nextPos < source.length() && (isalpha(source[nextPos]) || source[nextPos] == '_'))
            {
                throw runtime_error("Invalid identifier starting with digit at line " + to_string(line));
            }
            pos += match.length();
            col += match.length();
            return makeToken(TokenType::T_INTLIT, value);
        }

        // --- identifier / keyword ---
        if (regex_search(remaining, match, identifierPattern) && match.position() == 0)
        {
            string value = match.str(1);
            pos += match.length();
            col += match.length();
            auto it = keywords.find(value);
            TokenType type = (it != keywords.end()) ? it->second : TokenType::T_IDENTIFIER;
            return makeToken(type, value);
        }

        // --- two-char operators ---
        if (regex_search(remaining, match, twoCharOpPattern) && match.position() == 0)
        {
            string op = match.str(1).empty() ? match.str(0) : match.str(1); // covers both groups
            if (op == "==")
            {
                pos += 2;
                col += 2;
                return makeToken(TokenType::T_EQUALSOP, op);
            }
            if (op == "!=")
            {
                pos += 2;
                col += 2;
                return makeToken(TokenType::T_NOTEQUAL, op);
            }
            if (op == "<=")
            {
                pos += 2;
                col += 2;
                return makeToken(TokenType::T_LESSEQ, op);
            }
            if (op == ">=")
            {
                pos += 2;
                col += 2;
                return makeToken(TokenType::T_GREATEREQ, op);
            }
            if (op == "&&")
            {
                pos += 2;
                col += 2;
                return makeToken(TokenType::T_AND, op);
            }
            if (op == "||")
            {
                pos += 2;
                col += 2;
                return makeToken(TokenType::T_OR, op);
            }
            if (op == "<<")
            {
                pos += 2;
                col += 2;
                return makeToken(TokenType::T_LSHIFT, op);
            }
        }
        if (remaining.rfind("**", 0) == 0)
        {
            pos += 2;
            col += 2;
            return makeToken(TokenType::T_POWER, "**");
        }

        // Just before single-char switch:
        if (remaining.rfind("<<", 0) == 0)
        {
            pos += 2;
            col += 2;
            return makeToken(TokenType::T_LSHIFT, "<<");
        }
        if (remaining.rfind(">>", 0) == 0)
        { // <-- NEW
            pos += 2;
            col += 2;
            return makeToken(TokenType::T_RSHIFT, ">>");
        }
        // --- multi-char operators (order matters!) ---
        // --- multi-char operators (order matters!) ---
        if (remaining.rfind("++", 0) == 0)
        {
            pos += 2;
            col += 2;
            return makeToken(TokenType::T_INC, "++");
        }
        if (remaining.rfind("--", 0) == 0)
        {
            pos += 2;
            col += 2;
            return makeToken(TokenType::T_DEC, "--");
        }
        // (+=, -=, <<, >> already handled above if you added them)

        if (remaining.rfind("+=", 0) == 0)
        {
            pos += 2;
            col += 2;
            return makeToken(TokenType::T_PLUS_EQ, "+=");
        }
        if (remaining.rfind("-=", 0) == 0)
        {
            pos += 2;
            col += 2;
            return makeToken(TokenType::T_MINUS_EQ, "-=");
        }
        // (if you later add >>, put it here too, before single '>' handling)

        // --- single-char tokens ---
        char c = remaining[0];
        pos++;
        col++;
        switch (c)
        {
        case '&':
            return makeToken(TokenType::T_BITAND, "&");
        case '|':
            return makeToken(TokenType::T_BITOR, "|");
        case '^':
            return makeToken(TokenType::T_BITXOR, "^");

        case '=':
            return makeToken(TokenType::T_ASSIGNOP, "=");
        case '<':
            return makeToken(TokenType::T_LESS, "<");
        case '>':
            return makeToken(TokenType::T_GREATER, ">");
        case '!':
            return makeToken(TokenType::T_NOT, "!");
        case '+':
            return makeToken(TokenType::T_PLUS, "+");
        case '-':
            return makeToken(TokenType::T_MINUS, "-");
        case '*':
            return makeToken(TokenType::T_MULTIPLY, "*");
        case '/':
            return makeToken(TokenType::T_DIVIDE, "/");
        case '(':
            return makeToken(TokenType::T_PARENL, "(");
        case ')':
            return makeToken(TokenType::T_PARENR, ")");
        case '{':
            return makeToken(TokenType::T_BRACEL, "{");
        case '}':
            return makeToken(TokenType::T_BRACER, "}");
        case '[':
            return makeToken(TokenType::T_BRACKETL, "[");
        case ']':
            return makeToken(TokenType::T_BRACKETR, "]");
        case ',':
            return makeToken(TokenType::T_COMMA, ",");
        case ';':
            return makeToken(TokenType::T_SEMICOLON, ";");
        }
        return makeToken(TokenType::T_UNKNOWN, string(1, c));
    }
};

#ifdef LEXER_STANDALONE
int main()
{
    try
    {
        string code = R"(
fn int my_fn(int x, float y) {
    // This is a line comment
    /*
       This is a block comment
    */
    string s = "Abdullah\nZahid";
    float a = 23.45;
    float b = 23.5;
    if (x >= 10 && y < 20.5) {
        return x + 1;
    } else {
        return x - 1;
    }
}
)";
        RegexLexer lex(code);
        auto tokens = lex.tokenize();
        cout << "[";
        bool first = true;
        for (auto &t : tokens)
        {
            if (t.type == TokenType::T_EOF)
                continue;
            if (!first)
                cout << ", ";
            cout << tokenToDisplay(t);
            first = false;
        }
        cout << "]\n";
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
#endif
