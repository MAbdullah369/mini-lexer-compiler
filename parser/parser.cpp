#include <bits/stdc++.h> // or whatever your header includes are // if you include the lexer this way
#include "debug.hpp"

using namespace std;

// =======================================================
// Operator Precedence & Associativity Table
// =======================================================

enum Assoc
{
    LEFT,
    RIGHT,
    NONE
};

struct OperatorInfo
{
    int precedence;
    Assoc assoc;
    std::string symbol;
};

// Table-driven precedence map
static const std::unordered_map<TokenType, OperatorInfo> OP_TABLE = {
    {TokenType::T_ASSIGNOP, {1, RIGHT, "="}},
    {TokenType::T_PLUS_EQ, {1, RIGHT, "+="}},
    {TokenType::T_MINUS_EQ, {1, RIGHT, "-="}},
    {TokenType::T_OR, {2, LEFT, "||"}},
    {TokenType::T_AND, {3, LEFT, "&&"}},
    {TokenType::T_EQUALSOP, {4, LEFT, "=="}},
    {TokenType::T_NOTEQUAL, {4, LEFT, "!="}},
    {TokenType::T_LESS, {5, LEFT, "<"}},
    {TokenType::T_LESSEQ, {5, LEFT, "<="}},
    {TokenType::T_GREATER, {5, LEFT, ">"}},
    {TokenType::T_GREATEREQ, {5, LEFT, ">="}},
    {TokenType::T_BITOR, {6, LEFT, "|"}},
    {TokenType::T_BITXOR, {7, LEFT, "^"}},
    {TokenType::T_BITAND, {8, LEFT, "&"}},
    {TokenType::T_LSHIFT, {9, LEFT, "<<"}},
    {TokenType::T_RSHIFT, {9, LEFT, ">>"}},
    {TokenType::T_PLUS, {10, LEFT, "+"}},
    {TokenType::T_MINUS, {10, LEFT, "-"}},
    {TokenType::T_MULTIPLY, {11, LEFT, "*"}},
    {TokenType::T_DIVIDE, {11, LEFT, "/"}},
    {TokenType::T_POWER, {12, RIGHT, "**"}},
    {TokenType::T_NOT, {13, RIGHT, "!"}},
    {TokenType::T_INC, {14, RIGHT, "++"}},
    {TokenType::T_DEC, {14, RIGHT, "--"}}}; // <-- THIS closes the table!

// Helper to convert type tokens to readable names
static const char *typeKeywordToString(TokenType t)
{
    switch (t)
    {
    case TokenType::T_INT:
        return "int";
    case TokenType::T_FLOAT:
        return "float";
    case TokenType::T_STRING:
        return "string";
    case TokenType::T_BOOL:
        return "bool";
    case TokenType::T_CHAR:
        return "char";
    default:
        return "<unknown-type>";
    }
}

#ifndef PARSER_REUSE_LEXER
// Fallback token definitions (used only when PARSER_REUSE_LEXER is NOT defined).
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
    T_IDENTIFIER,
    T_INTLIT,
    T_FLOATLIT,
    T_STRINGLIT,
    T_QUOTES,
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
    T_EOF,
    T_BITAND, // &
    T_BITOR,  // |
    T_BITXOR, // ^
    T_POWER,  // **
    T_UNKNOWN
};

struct Token
{
    TokenType type{};
    string lexeme;
    int line = 1;
    int col = 1;
};
static string tokenToDisplay(const Token &t)
{
    if (t.type == TokenType::T_IDENTIFIER)
        return "T_IDENTIFIER(\"" + t.lexeme + "\")";
    if (t.type == TokenType::T_INTLIT)
        return "T_INTLIT(" + t.lexeme + ")";
    if (t.type == TokenType::T_FLOATLIT)
        return "T_FLOATLIT(" + t.lexeme + ")";
    if (t.type == TokenType::T_STRINGLIT)
        return "T_STRINGLIT(\"" + t.lexeme + "\")";
    // fallback for others:
    return "<TOKEN>";
}
#endif

// ---------- Parse error ----------
enum class ParseErrorKind
{
    UnexpectedEOF,
    FailedToFindToken,
    ExpectedTypeToken,
    ExpectedIdentifier,
    UnexpectedToken,
    ExpectedFloatLit,
    ExpectedIntLit,
    ExpectedStringLit,
    ExpectedBoolLit,
    ExpectedExpr,
};

struct ParseError : public std::exception
{
    ParseErrorKind kind;
    Token token;
    string msg;
    ParseError(ParseErrorKind k, Token tk = {}, string m = "") : kind(k), token(tk), msg(m)
    {
        if (msg.empty())
        {
            // default messages
            switch (kind)
            {
            case ParseErrorKind::UnexpectedEOF:
                msg = "Unexpected end of input";
                break;
            case ParseErrorKind::FailedToFindToken:
                msg = "Failed to find token";
                break;
            case ParseErrorKind::ExpectedTypeToken:
                msg = "Expected type token";
                break;
            case ParseErrorKind::ExpectedIdentifier:
                msg = "Expected identifier";
                break;
            case ParseErrorKind::UnexpectedToken:
                msg = "Unexpected token";
                break;
            case ParseErrorKind::ExpectedFloatLit:
                msg = "Expected float literal";
                break;
            case ParseErrorKind::ExpectedIntLit:
                msg = "Expected int literal";
                break;
            case ParseErrorKind::ExpectedStringLit:
                msg = "Expected string literal";
                break;
            case ParseErrorKind::ExpectedBoolLit:
                msg = "Expected bool literal";
                break;
            case ParseErrorKind::ExpectedExpr:
                msg = "Expected expression";
                break;
            }
        }
        // Append token info if available
        if (token.line || token.col || !token.lexeme.empty())
        {
            msg += " (at line " + to_string(token.line) + ", col " + to_string(token.col) + ")";
            if (!token.lexeme.empty())
                msg += " token=" + tokenToDisplay(token);
        }
    }
    const char *what() const noexcept override { return msg.c_str(); }
};

// ---------- AST nodes ----------
static void indent(ostream &os, int n)
{
    for (int i = 0; i < n; i++)
        os << "  ";
}

struct ASTNode
{
    int line, col;
    ASTNode(int l = 0, int c = 0) : line(l), col(c) {}
    virtual ~ASTNode() = default;
    virtual void print(ostream &os, int indent = 0) const = 0;
};

struct Expr : ASTNode
{
    Expr(int l = 0, int c = 0) : ASTNode(l, c) {}
    virtual ~Expr() = default;
    // no extra methods unless you want
};
using ExprPtr = shared_ptr<Expr>;

inline std::string indentStr(int indent)
{
    return std::string(indent, ' ');
}

struct IntLiteral : Expr
{
    string val;
    IntLiteral(string v, int l = 0, int c = 0) : val(move(v))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Int(" << val << ") [l:" << line << " c:" << col << "]\n";
    }
};
struct FloatLiteral : Expr
{
    string val;
    FloatLiteral(string v, int l = 0, int c = 0) : val(move(v))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Float(" << val << ") [l:" << line << " c:" << col << "]\n";
    }
};
struct StringLiteral : Expr
{
    string val;
    StringLiteral(string v, int l = 0, int c = 0) : val(move(v))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "String(\"" << val << "\") [l:" << line << " c:" << col << "]\n";
    }
};
struct CharLiteral : Expr
{
    string val;
    explicit CharLiteral(string v, int l = 0, int c = 0) : val(std::move(v))
    {
        line = l;
        col = c;
    }
    void print(std::ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        string shown = val;
        if (val == "\n")
            shown = "\\n";
        else if (val == "\t")
            shown = "\\t";
        else if (val == "\r")
            shown = "\\r";
        else if (val == "\\")
            shown = "\\\\";
        else if (val == "'")
            shown = "\\'";
        os << "Char('" << shown << "') [l:" << line << " c:" << col << "]\n";
    }
};
struct BoolLiteral : Expr
{
    bool val;
    explicit BoolLiteral(bool v, int l = 0, int c = 0) : val(v)
    {
        line = l;
        col = c;
    }
    void print(std::ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Bool(" << (val ? "true" : "false") << ") [l:" << line << " c:" << col << "]\n";
    }
};

struct IdentifierExpr : Expr
{
    string name;
    IdentifierExpr(string n, int l = 0, int c = 0) : name(move(n))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Ident(" << name << ") [l:" << line << " c:" << col << "]\n";
    }
};
struct UnaryExpr : Expr
{
    string op;
    ExprPtr rhs;
    UnaryExpr(string o, ExprPtr r, int l = 0, int c = 0) : op(move(o)), rhs(r)
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Unary(" << op << ") [l:" << line << " c:" << col << "]\n";
        rhs->print(os, ind + 1);
    }
};
struct PostfixExpr : Expr
{
    string op;
    ExprPtr expr;
    PostfixExpr(string o, ExprPtr e, int l = 0, int c = 0) : op(std::move(o)), expr(std::move(e))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Postfix(" << op << ") [l:" << line << " c:" << col << "]\n";
        expr->print(os, ind + 1);
    }
};
struct BinaryExpr : Expr
{
    string op;
    ExprPtr lhs, rhs;
    BinaryExpr(string o, ExprPtr l, ExprPtr r, int ln = 0, int c = 0) : op(move(o)), lhs(l), rhs(r)
    {
        line = ln;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Binary(" << op << ") [l:" << line << " c:" << col << "]\n";
        lhs->print(os, ind + 1);
        rhs->print(os, ind + 1);
    }
};
struct CallExpr : Expr
{
    string name;
    vector<ExprPtr> args;
    CallExpr(string n, vector<ExprPtr> a, int l = 0, int c = 0) : name(move(n)), args(move(a))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Call(" << name << ") [l:" << line << " c:" << col << "]\n";
        for (auto &a : args)
            a->print(os, ind + 1);
    }
};
struct IndexExpr : Expr
{
    ExprPtr base;
    ExprPtr index;
    IndexExpr(ExprPtr b, ExprPtr i, int l = 0, int c = 0) : base(std::move(b)), index(std::move(i))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "IndexExpr [l:" << line << " c:" << col << "]\n";
        base->print(os, ind + 1);
        index->print(os, ind + 1);
    }
};

// ------ Statements / Declarations -----
struct Stmt : ASTNode
{
    Stmt(int l = 0, int c = 0) : ASTNode(l, c) {}
    virtual ~Stmt() = default;
};

using StmtPtr = shared_ptr<Stmt>;

struct BreakStmt : Stmt
{
    BreakStmt(int l, int c) : Stmt(l, c) {}
    void print(ostream &os, int ind = 0) const override
    {
        os << indentStr(ind) << "Break [l:" << line << " c:" << col << "]\n";
    }
};

struct EmptyStmt : Stmt
{
    EmptyStmt(int l = 0, int c = 0)
    {
        line = l;
        col = c;
    }
    void print(std::ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "EmptyStmt [l:" << line << " c:" << col << "]\n";
    }
};
struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e, int l = 0, int c = 0) : expr(e)
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "ExprStmt [l:" << line << " c:" << col << "]\n";
        if (expr)
            expr->print(os, ind + 1);
    }
};
struct ReturnStmt : Stmt
{
    ExprPtr expr;
    ReturnStmt(ExprPtr e, int l = 0, int c = 0) : expr(e)
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Return [l:" << line << " c:" << col << "]\n";
        if (expr)
            expr->print(os, ind + 1);
    }
};
struct VarDeclStmt : Stmt
{
    TokenType typeTok;
    string name;
    ExprPtr init;
    VarDeclStmt(TokenType t, string n, ExprPtr i, int l = 0, int c = 0) : typeTok(t), name(move(n)), init(i)
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "VarDecl(type=" << typeKeywordToString(typeTok) << " name=" << name << ") [l:" << line << " c:" << col << "]\n";
        if (init)
            init->print(os, ind + 1);
    }
};
struct BlockStmt : Stmt
{
    vector<StmtPtr> stmts;
    BlockStmt(int l = 0, int c = 0)
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Block [l:" << line << " c:" << col << "]\n";
        for (auto &s : stmts)
            s->print(os, ind + 1);
    }
};
struct IfStmt : Stmt
{
    ExprPtr cond;
    StmtPtr thenStmt;
    StmtPtr elseStmt;
    IfStmt(ExprPtr c, StmtPtr t, StmtPtr e = nullptr, int l = 0, int col_ = 0) : cond(c), thenStmt(t), elseStmt(e)
    {
        line = l;
        col = col_;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "If [l:" << line << " c:" << col << "]\n";
        cond->print(os, ind + 1);
        indent(os, ind + 1);
        os << "Then:\n";
        thenStmt->print(os, ind + 2);
        if (elseStmt)
        {
            indent(os, ind + 1);
            os << "Else:\n";
            elseStmt->print(os, ind + 2);
        }
    }
};
struct WhileStmt : Stmt
{
    ExprPtr cond;
    StmtPtr body;
    WhileStmt(ExprPtr c, StmtPtr b, int l = 0, int col_ = 0) : cond(c), body(b)
    {
        line = l;
        col = col_;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "While [l:" << line << " c:" << col << "]\n";
        cond->print(os, ind + 1);
        body->print(os, ind + 1);
    }
};
struct DoWhileStmt : Stmt
{
    StmtPtr body;
    ExprPtr cond;
    DoWhileStmt(StmtPtr b, ExprPtr c, int l = 0, int col_ = 0) : body(std::move(b)), cond(std::move(c))
    {
        line = l;
        col = col_;
    }
    void print(std::ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "DoWhile [l:" << line << " c:" << col << "]\n";
        indent(os, ind + 1);
        os << "Body:\n";
        body->print(os, ind + 2);
        indent(os, ind + 1);
        os << "Cond:\n";
        cond->print(os, ind + 2);
    }
};
struct ForStmt : Stmt
{
    ExprPtr init;
    ExprPtr cond;
    ExprPtr post;
    StmtPtr body;
    ForStmt(ExprPtr i, ExprPtr c, ExprPtr p, StmtPtr b, int l = 0, int c_ = 0) : init(i), cond(c), post(p), body(b)
    {
        line = l;
        col = c_;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "For [l:" << line << " c:" << col << "]\n";
        if (init)
        {
            indent(os, ind + 1);
            os << "Init:\n";
            init->print(os, ind + 2);
        }
        if (cond)
        {
            indent(os, ind + 1);
            os << "Cond:\n";
            cond->print(os, ind + 2);
        }
        if (post)
        {
            indent(os, ind + 1);
            os << "Post:\n";
            post->print(os, ind + 2);
        }
        indent(os, ind + 1);
        os << "Body:\n";
        body->print(os, ind + 2);
    }
};

struct FnDecl : ASTNode
{
    TokenType returnType;
    string name;
    vector<pair<TokenType, string>> params;
    vector<StmtPtr> body;
    FnDecl(TokenType rt, string n, int l = 0, int c = 0) : returnType(rt), name(move(n))
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "FnDecl(name=" << name << " type=" << typeKeywordToString(returnType)
           << ") [l:" << line << " c:" << col << "]\n";
        indent(os, ind + 1);
        os << "Params:\n";
        for (auto &p : params)
        {
            indent(os, ind + 2);
            os << "(type=" << typeKeywordToString(p.first) << " name=" << p.second << ")\n";
        }
        indent(os, ind + 1);
        os << "Body:\n";
        for (auto &s : body)
            s->print(os, ind + 2);
    }
};

struct Program : ASTNode
{
    vector<shared_ptr<ASTNode>> items;
    Program(int l = 0, int c = 0)
    {
        line = l;
        col = c;
    }
    void print(ostream &os, int ind = 0) const override
    {
        indent(os, ind);
        os << "Program [l:" << line << " c:" << col << "]\n";
        for (auto &it : items)
            it->print(os, ind + 1);
    }
};

// ---------- TokenStream (skips trivia: comments and quotes) ----------
struct TokenStream
{
    vector<Token> tokens;
    size_t i = 0;
    TokenStream() = default;
    TokenStream(vector<Token> t) : tokens(move(t)), i(0) {}

    static bool isTrivia(TokenType tt)
    {
        return tt == TokenType::T_LINECOMMENT || tt == TokenType::T_BLOCKCOMMENT;
    }
    size_t skipTriviaIndex(size_t idx) const
    {
        while (idx < tokens.size() && isTrivia(tokens[idx].type))
            idx++;
        return idx;
    }
    Token peek() const
    {
        size_t idx = skipTriviaIndex(i);
        if (idx < tokens.size())
            return tokens[idx];
        return Token{TokenType::T_EOF, "", 0, 0};
    }
    Token advance()
    {
        size_t idx = skipTriviaIndex(i);
        DBG("[DBG] TokenStream::advance() - from index " << i << " to " << idx);
        if (idx >= tokens.size())
        {
            i = tokens.size();
            return Token{TokenType::T_EOF, "", 0, 0};
        }
        Token t = tokens[idx];
        i = idx + 1;
        DBG("[DBG] TokenStream::advance() - advanced to: " << tokenToDisplay(t));
        return t;
    }
    bool match(TokenType t)
    {
        if (peek().type == t)
        {
            advance();
            return true;
        }
        return false;
    }
    bool eof() const { return skipTriviaIndex(i) >= tokens.size() || tokens[skipTriviaIndex(i)].type == TokenType::T_EOF; }
};

// ---------- Pratt precedence and helpers ----------
enum Prec
{
    PREC_NONE = 0,
    PREC_ASSIGN = 1,     // =
    PREC_OR = 2,         // ||
    PREC_AND = 3,        // &&
    PREC_EQUALITY = 4,   // == !=
    PREC_COMPARISON = 5, // < <= > >=
    PREC_SHIFT = 6,      // << >>
    PREC_TERM = 7,       // + -
    PREC_FACTOR = 8,     // * /
    PREC_UNARY = 9,      // - ! + (prefix)
    PREC_CALL = 10
};

// static string tokToOp(const Token &t)
// {
//     switch (t.type)
//     {
//     case TokenType::T_PLUS:
//         return "+";
//     case TokenType::T_MINUS:
//         return "-";
//     case TokenType::T_MULTIPLY:
//         return "*";
//     case TokenType::T_DIVIDE:
//         return "/";
//     case TokenType::T_NOT:
//         return "!";
//     case TokenType::T_EQUALSOP:
//         return "==";
//     case TokenType::T_NOTEQUAL:
//         return "!=";
//     case TokenType::T_LESS:
//         return "<";
//     case TokenType::T_GREATER:
//         return ">";
//     case TokenType::T_LESSEQ:
//         return "<=";
//     case TokenType::T_GREATEREQ:
//         return ">=";
//     case TokenType::T_AND:
//         return "&&";
//     case TokenType::T_OR:
//         return "||";
//     case TokenType::T_ASSIGNOP:
//         return "=";
//         // ...
//     case TokenType::T_LSHIFT:
//         return "<<"; // NEW
//     case TokenType::T_RSHIFT:
//         return ">>"; // <-- NEW
//         // ...
//     case TokenType::T_PLUS_EQ:
//         return "+=";
//     case TokenType::T_MINUS_EQ:
//         return "-=";
//     case TokenType::T_INC:
//         return "++";
//     case TokenType::T_DEC:
//         return "--";

//     default:
//         return "?";
//     }
// }
static std::string tokToOp(const Token &tok)
{
    auto it = OP_TABLE.find(tok.type);
    if (it != OP_TABLE.end())
        return it->second.symbol;
    return tok.lexeme;
}
struct Parser
{
    TokenStream ts;
    int fnDepth = 0; // 0 = top-level, >0 = inside a function body

    Parser() = default;
    Parser(vector<Token> tokens) : ts(move(tokens)) {}
    // Skip tokens until we reach a statement boundary.
    // If consumeBracer==true (top-level), swallow a stray '}' so we make progress.
    // If consumeBracer==false (inside a block), stop at '}' and let the caller handle it.
    void sync(bool consumeBracer = true)
    {
        bool advanced = false;
        while (!ts.eof())
        {
            Token t = ts.peek();
            if (t.type == TokenType::T_SEMICOLON)
            {
                ts.advance();
                return;
            }
            if (t.type == TokenType::T_BRACER)
            {
                if (consumeBracer)
                    ts.advance(); // <-- key difference
                return;
            }
            ts.advance();
            advanced = true;
        }
        (void)advanced; // silence unused variable warning if any
    }

    // Recover from a bad statement *inside a block*.
    // Skips tokens until we hit:
    //   - a top-level ';' (depth==0)  → consume it and stop
    //   - a top-level '}' (depth==0)  → stop *before* it (caller will handle)
    // Tracks nested { } so we skip whole sub-blocks safely.
    void syncInBlock()
    {
        int braceDepth = 0;
        while (!ts.eof())
        {
            Token t = ts.peek();

            if (t.type == TokenType::T_SEMICOLON && braceDepth == 0)
            {
                ts.advance(); // eat ';' to make progress
                return;
            }
            if (t.type == TokenType::T_BRACEL)
            {
                ts.advance(); // enter nested block
                braceDepth++;
                continue;
            }
            if (t.type == TokenType::T_BRACER)
            {
                if (braceDepth == 0)
                {
                    // end of *this* block: don't consume it; let caller handle
                    return;
                }
                ts.advance(); // close a nested block we entered
                braceDepth--;
                continue;
            }

            // Otherwise, just advance.
            ts.advance();
        }
    }

    void syncTopLevel()
    {
        while (!ts.eof())
        {
            Token t = ts.peek();

            // If we hit a ; or } or start of a new top-level decl, stop.
            if (t.type == TokenType::T_SEMICOLON || t.type == TokenType::T_BRACER)
            {
                ts.advance();
                return;
            }

            if (t.type == TokenType::T_FUNCTION || t.type == TokenType::T_INT ||
                t.type == TokenType::T_FLOAT || t.type == TokenType::T_STRING ||
                t.type == TokenType::T_BOOL || t.type == TokenType::T_CHAR)
            {
                // Looks like a new top-level declaration — stop recovery.
                return;
            }

            ts.advance(); // otherwise skip one token
        }
    }
    bool isAssignableLHS(const ExprPtr &e)
    {
        // identifier like:   x = ...
        if (std::dynamic_pointer_cast<IdentifierExpr>(e))
            return true;
        // array element like: arr[i] = ...
        if (std::dynamic_pointer_cast<IndexExpr>(e))
            return true;
        // (later you can add member access here)
        return false;
    }

    // Top-level
    Program parseProgram()
    {
        Program prog;

        // Local recovery helper for TOP-LEVEL only.
        // Skips junk until we either (a) consume a boundary ; or }, or
        // (b) see the start of a new top-level decl (fn / type).
        auto recoverTop = [this]()
        {
            while (!ts.eof())
            {
                Token t = ts.peek();

                // Make definite forward progress on common boundaries
                if (t.type == TokenType::T_SEMICOLON || t.type == TokenType::T_BRACER)
                {
                    ts.advance(); // eat it so we don't loop on the same token
                    return;
                }

                // Stop when we seem to be at the start of a new top-level decl
                if (t.type == TokenType::T_FUNCTION ||
                    t.type == TokenType::T_INT || t.type == TokenType::T_FLOAT ||
                    t.type == TokenType::T_STRING || t.type == TokenType::T_BOOL ||
                    t.type == TokenType::T_CHAR)
                {
                    return; // let the outer loop handle parsing it
                }

                // Otherwise skip one token and keep scanning
                ts.advance();
            }
        };

        while (!ts.eof())
        {
            Token p = ts.peek();

            if (p.type == TokenType::T_FUNCTION ||
                p.type == TokenType::T_INT || p.type == TokenType::T_FLOAT ||
                p.type == TokenType::T_STRING || p.type == TokenType::T_BOOL ||
                p.type == TokenType::T_CHAR)
            {
                try
                {
                    auto decl = parseTopLevelDecl();
                    if (decl)
                        prog.items.push_back(decl);
                }
                catch (const ParseError &e)
                {
                    DBG("Top-level parse error (recovering): " << e.what());
                    recoverTop(); // robust top-level sync; always makes progress
                }
            }
            else if (p.type == TokenType::T_EOF)
            {
                break;
            }
            else
            {
                DBG("Unexpected token at top-level (recovering): " << tokenToDisplay(p));
                recoverTop(); // robust top-level sync; always makes progress
            }
        }

        return prog;
    }

    shared_ptr<ASTNode> parseTopLevelDecl()
    {
        Token first = ts.peek();
        // Helper counter for unnamed parameters (e.g., "int foo(int)")
        static int dummyCounter = 0;

        // ----------- Case 1: 'fn' keyword (e.g. "fn int main()") -----------
        if (first.type == TokenType::T_FUNCTION)
        {
            ts.advance(); // consume 'fn'

            Token rt = ts.peek();
            // Verify return type
            if (!(rt.type == TokenType::T_INT || rt.type == TokenType::T_FLOAT ||
                  rt.type == TokenType::T_STRING || rt.type == TokenType::T_BOOL ||
                  rt.type == TokenType::T_CHAR))
            {
                // Check for specific common error: missing return type
                size_t afterRtIdx = ts.skipTriviaIndex(ts.i + 1);
                Token afterRt = (afterRtIdx < ts.tokens.size()) ? ts.tokens[afterRtIdx] : Token{TokenType::T_EOF, "", 0, 0};
                if (rt.type == TokenType::T_IDENTIFIER && afterRt.type == TokenType::T_PARENL)
                {
                    throw ParseError(ParseErrorKind::ExpectedTypeToken, rt, "Missing return type after 'fn'");
                }
                throw ParseError(ParseErrorKind::ExpectedTypeToken, rt, "Expected return type after 'fn'");
            }
            ts.advance(); // consume return type

            Token id = ts.peek();
            if (id.type != TokenType::T_IDENTIFIER)
                throw ParseError(ParseErrorKind::ExpectedIdentifier, id, "Expected function name");
            ts.advance(); // consume identifier

            if (!ts.match(TokenType::T_PARENL))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '('");

            auto fn = make_shared<FnDecl>(rt.type, id.lexeme, first.line, first.col);

            // Parse Parameters
            if (!ts.match(TokenType::T_PARENR))
            {
                while (true)
                {
                    Token ptype = ts.peek();
                    if (!(ptype.type == TokenType::T_INT || ptype.type == TokenType::T_FLOAT ||
                          ptype.type == TokenType::T_STRING || ptype.type == TokenType::T_BOOL ||
                          ptype.type == TokenType::T_CHAR))
                    {
                        throw ParseError(ParseErrorKind::ExpectedTypeToken, ptype, "Expected parameter type");
                    }
                    ts.advance(); // consume type

                    // FIX 1: Handle Unnamed Parameters
                    string paramName;
                    Token pname = ts.peek();
                    if (pname.type == TokenType::T_IDENTIFIER)
                    {
                        paramName = pname.lexeme;
                        ts.advance();
                    }
                    else
                    {
                        // Generate dummy name if missing so AST is valid
                        paramName = "_arg_" + to_string(dummyCounter++);
                    }
                    fn->params.push_back({ptype.type, paramName});

                    if (ts.match(TokenType::T_COMMA))
                        continue;
                    if (ts.match(TokenType::T_PARENR))
                        break;
                    throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ',' or ')'");
                }
            }

            // FIX 2: Handle Prototype (semicolon)
            if (ts.match(TokenType::T_SEMICOLON))
            {
                return fn;
            }

            if (!ts.match(TokenType::T_BRACEL))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '{' or ';'");

            // Parse Body
            fnDepth++;
            auto bodyBlock = make_shared<BlockStmt>(ts.peek().line, ts.peek().col);
            while (!ts.eof())
            {
                if (ts.peek().type == TokenType::T_BRACER)
                    break;
                try
                {
                    StmtPtr stmt = parseStmt();
                    if (stmt)
                        bodyBlock->stmts.push_back(stmt);
                }
                catch (const ParseError &e)
                {
                    syncInBlock();
                }
            }
            fn->body.push_back(bodyBlock);

            if (!ts.match(TokenType::T_BRACER))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '}'");

            fnDepth--;
            return fn;
        }

        // ----------- Case 2: C-Style Declaration (e.g. "int foo(...)") -----------
        Token typeTok = ts.peek();
        if (!(typeTok.type == TokenType::T_INT || typeTok.type == TokenType::T_FLOAT ||
              typeTok.type == TokenType::T_STRING || typeTok.type == TokenType::T_BOOL ||
              typeTok.type == TokenType::T_CHAR))
        {
            throw ParseError(ParseErrorKind::ExpectedTypeToken, typeTok, "Expected type token");
        }
        ts.advance(); // consume type

        Token id = ts.peek();
        if (id.type != TokenType::T_IDENTIFIER)
            throw ParseError(ParseErrorKind::ExpectedIdentifier, id, "Expected identifier");
        ts.advance(); // consume identifier

        // Check if it is a Function: has '('
        if (ts.match(TokenType::T_PARENL))
        {
            auto fn = make_shared<FnDecl>(typeTok.type, id.lexeme, typeTok.line, typeTok.col);

            // Parse Parameters
            if (!ts.match(TokenType::T_PARENR))
            {
                while (true)
                {
                    Token ptype = ts.peek();
                    if (!(ptype.type == TokenType::T_INT || ptype.type == TokenType::T_FLOAT ||
                          ptype.type == TokenType::T_STRING || ptype.type == TokenType::T_BOOL ||
                          ptype.type == TokenType::T_CHAR))
                    {
                        throw ParseError(ParseErrorKind::ExpectedTypeToken, ptype, "Expected param type");
                    }
                    ts.advance();

                    // FIX 1: Handle Unnamed Parameters
                    string paramName;
                    Token pname = ts.peek();
                    if (pname.type == TokenType::T_IDENTIFIER)
                    {
                        paramName = pname.lexeme;
                        ts.advance();
                    }
                    else
                    {
                        paramName = "_arg_" + to_string(dummyCounter++);
                    }
                    fn->params.push_back({ptype.type, paramName});

                    if (ts.match(TokenType::T_COMMA))
                        continue;
                    if (ts.match(TokenType::T_PARENR))
                        break;
                    throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ',' or ')'");
                }
            }

            // FIX 2: Handle Prototype (semicolon)
            if (ts.match(TokenType::T_SEMICOLON))
            {
                return fn;
            }

            if (!ts.match(TokenType::T_BRACEL))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '{' or ';'");

            fnDepth++;
            auto bodyBlock = make_shared<BlockStmt>(ts.peek().line, ts.peek().col);
            while (!ts.eof() && ts.peek().type != TokenType::T_BRACER)
            {
                try
                {
                    StmtPtr stmt = parseStmt();
                    if (stmt)
                        bodyBlock->stmts.push_back(stmt);
                }
                catch (const ParseError &e)
                {
                    syncInBlock();
                }
            }
            fn->body.push_back(bodyBlock);

            if (!ts.match(TokenType::T_BRACER))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '}'");
            fnDepth--;
            return fn;
        }
        else
        {
            // It is a Variable Declaration
            ExprPtr init = nullptr;
            if (ts.match(TokenType::T_ASSIGNOP))
            {
                init = parseExpression();
            }
            if (!ts.match(TokenType::T_SEMICOLON))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';'");

            return make_shared<VarDeclStmt>(typeTok.type, id.lexeme, init, typeTok.line, typeTok.col);
        }
    }

    // Statements
    StmtPtr parseStmt()
    {
        Token t = ts.peek();
        DBG("[DBG] parseStmt() START - type=" << (int)t.type << " token=" << tokenToDisplay(t)
                                              << " line=" << t.line << " col=" << t.col);

        // Empty statement: just a semicolon
        if (t.type == TokenType::T_SEMICOLON)
        {
            ts.advance();
            return make_shared<EmptyStmt>(t.line, t.col);
        }
        // 🚫 No nested function definitions
        if (t.type == TokenType::T_FUNCTION)
        {
            throw ParseError(ParseErrorKind::UnexpectedToken, t, "Nested function definitions are not allowed");
        }

        // Variable declarations (single or comma-list)
        if (t.type == TokenType::T_INT || t.type == TokenType::T_FLOAT ||
            t.type == TokenType::T_STRING || t.type == TokenType::T_BOOL ||
            t.type == TokenType::T_CHAR)
        {
            Token typeTok = ts.advance(); // type keyword

            // First identifier
            Token name = ts.peek();
            if (name.type != TokenType::T_IDENTIFIER)
                throw ParseError(ParseErrorKind::ExpectedIdentifier, name, "Expected identifier in variable declaration");
            ts.advance();

            // Optional initializer
            ExprPtr init = nullptr;
            if (ts.match(TokenType::T_ASSIGNOP))
            {
                init = parseExpression();
            }

            if (ts.match(TokenType::T_COMMA))
            {
                // Multiple declarations - block containing individual VarDeclStmts
                auto block = make_shared<BlockStmt>(typeTok.line, typeTok.col);
                block->stmts.push_back(
                    make_shared<VarDeclStmt>(typeTok.type, name.lexeme, init, name.line, name.col));
                do
                {
                    Token n2 = ts.peek();
                    if (n2.type != TokenType::T_IDENTIFIER)
                        throw ParseError(ParseErrorKind::ExpectedIdentifier, n2, "Expected identifier after ',' in declaration");
                    ts.advance();
                    ExprPtr i2 = nullptr;
                    if (ts.match(TokenType::T_ASSIGNOP))
                    {
                        i2 = parseExpression();
                    }
                    block->stmts.push_back(
                        make_shared<VarDeclStmt>(typeTok.type, n2.lexeme, i2, n2.line, n2.col));
                } while (ts.match(TokenType::T_COMMA));

                if (!ts.match(TokenType::T_SEMICOLON))
                    throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after variable declaration");

                return block;
            }
            else
            {
                if (!ts.match(TokenType::T_SEMICOLON))
                    throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after variable declaration");

                return make_shared<VarDeclStmt>(typeTok.type, name.lexeme, init, name.line, name.col);
            }
        }

        // Return statement
        if (t.type == TokenType::T_RETURN)
        {
            if (fnDepth == 0)
            {
                throw ParseError(ParseErrorKind::UnexpectedToken, t, "return outside of function");
            }
            ts.advance();
            ExprPtr e = nullptr;
            if (ts.peek().type != TokenType::T_SEMICOLON)
            {
                e = parseExpression();
            }
            if (!ts.match(TokenType::T_SEMICOLON))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after return");
            return make_shared<ReturnStmt>(e, t.line, t.col);
        }

        if (t.type == TokenType::T_IF)
        {
            return parseIf();
        }

        if (t.type == TokenType::T_WHILE)
        {
            Token whileTok = ts.peek();
            ts.advance(); // consume 'while'

            if (!ts.match(TokenType::T_PARENL))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after 'while'");

            ExprPtr cond = parseExpression();
            if (!cond)
                throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(), "Expected condition expression after '('");

            if (!ts.match(TokenType::T_PARENR))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ')' after while condition");

            StmtPtr body = parseStmtOrBlock();
            if (!body)
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected statement or block after while");

            return make_shared<WhileStmt>(cond, body, whileTok.line, whileTok.col);
        }

        if (t.type == TokenType::T_DO)
        {
            Token doTok = ts.peek();
            ts.advance(); // consume 'do'

            StmtPtr body = parseStmtOrBlock();

            if (ts.peek().type != TokenType::T_WHILE)
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected 'while' after 'do' body");
            ts.advance(); // consume 'while'

            if (!ts.match(TokenType::T_PARENL))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after 'while'");

            ExprPtr cond = parseExpression();
            if (!ts.match(TokenType::T_PARENR))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ')' after condition");
            if (!ts.match(TokenType::T_SEMICOLON))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after do-while");

            return make_shared<DoWhileStmt>(body, cond, doTok.line, doTok.col);
        }

        if (t.type == TokenType::T_FOR)
        {
            Token forTok = ts.peek();
            ts.advance();
            if (!ts.match(TokenType::T_PARENL))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after 'for'");
            ExprPtr init = nullptr, cond = nullptr, post = nullptr;
            if (ts.peek().type != TokenType::T_SEMICOLON)
            {
                if (ts.peek().type == TokenType::T_INT || ts.peek().type == TokenType::T_FLOAT || ts.peek().type == TokenType::T_STRING || ts.peek().type == TokenType::T_BOOL)
                {
                    Token typeTok = ts.advance();
                    Token name = ts.peek();
                    if (name.type != TokenType::T_IDENTIFIER)
                        throw ParseError(ParseErrorKind::ExpectedIdentifier, name, "Expected identifier in for-init");
                    ts.advance();
                    if (ts.match(TokenType::T_ASSIGNOP))
                        init = parseExpression();
                    else
                        init = nullptr;
                }
                else
                {
                    init = parseExpression();
                }
            }
            if (!ts.match(TokenType::T_SEMICOLON))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' in for loop after init");
            if (ts.peek().type != TokenType::T_SEMICOLON)
                cond = parseExpression();
            if (!ts.match(TokenType::T_SEMICOLON))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' in for loop after cond");
            if (ts.peek().type != TokenType::T_PARENR)
                post = parseExpression();
            if (!ts.match(TokenType::T_PARENR))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ')' to close for loop header");
            StmtPtr body = parseStmtOrBlock();
            return make_shared<ForStmt>(init, cond, post, body, forTok.line, forTok.col);
        }

        if (t.type == TokenType::T_BREAK)
        {
            ts.advance();
            if (!ts.match(TokenType::T_SEMICOLON))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after break");
            return make_shared<BreakStmt>(t.line, t.col);
        }

        if (t.type == TokenType::T_BRACEL)
        {
            Token blockTok = ts.peek();
            ts.advance(); // consume '{'
            auto block = make_shared<BlockStmt>(blockTok.line, blockTok.col);
            while (true)
            {
                if (ts.eof())
                    throw ParseError(ParseErrorKind::UnexpectedEOF, ts.peek(), "Unterminated block");

                Token next = ts.peek();
                if (next.type == TokenType::T_BRACER)
                {
                    ts.advance(); // consume '}'
                    break;
                }

                StmtPtr s = parseStmt();
                if (s)
                    block->stmts.push_back(s);
            }
            return block;
        }

        // Expression statement
        ExprPtr e = parseExpression();
        if (!e)
        {
            throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(), "Expected expression");
        }
        if (!ts.match(TokenType::T_SEMICOLON))
            throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after expression");
        return make_shared<ExprStmt>(e, t.line, t.col);
    }

    StmtPtr parseStmtOrBlock()
    {
        Token p = ts.peek();
        if (p.type == TokenType::T_BRACEL)
            return parseStmt(); // parseStmt handles block
        return parseStmt();
    }

    StmtPtr parseIf()
    {
        Token ifTok = ts.peek();
        ts.advance(); // consume 'if'
        if (!ts.match(TokenType::T_PARENL))
            throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after 'if'");
        ExprPtr cond = parseExpression();
        if (!ts.match(TokenType::T_PARENR))
            throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ')' after if condition");

        StmtPtr thenStmt = parseStmtOrBlock();

        StmtPtr elseStmt = nullptr;
        if (ts.match(TokenType::T_ELSE))
        {
            if (ts.peek().type == TokenType::T_IF)
            {
                elseStmt = parseIf();
            }
            else
            {
                elseStmt = parseStmtOrBlock();
            }
        }

        return make_shared<IfStmt>(cond, thenStmt, elseStmt, ifTok.line, ifTok.col);
    }

    // ---------- Pratt parser ----------
    ExprPtr parseExpression(int minPrec = 0)
    {
        ExprPtr left = parsePrefix();
        if (!left)
            throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(), "Expected expression");

        while (true)
        {
            Token op = ts.peek();
            int prec = getPrecedence(op.type);

            if (prec == 0 || prec < minPrec)
                break;

            Token opTok = ts.advance();
            int nextMin = prec + (isRightAssoc(opTok.type) ? 0 : 1);
            ExprPtr right = parseExpression(nextMin);

            // Handle compound assignment desugaring: x += y → x = x + y
            if (opTok.type == TokenType::T_PLUS_EQ || opTok.type == TokenType::T_MINUS_EQ)
            {
                const std::string bop = (opTok.type == TokenType::T_PLUS_EQ) ? "+" : "-";
                auto combined = std::make_shared<BinaryExpr>(bop, left, right, opTok.line, opTok.col);
                left = std::make_shared<BinaryExpr>("=", left, combined, opTok.line, opTok.col);
                continue;
            }

            left = std::make_shared<BinaryExpr>(tokToOp(opTok), left, right, opTok.line, opTok.col);
        }

        return left;
    }

    ExprPtr parsePostfixTrail(ExprPtr left)
    {
        while (true)
        {
            Token t = ts.peek();

            // function call: foo(...), also supports zero args foo()
            if (t.type == TokenType::T_PARENL)
            {
                int callLine = t.line, callCol = t.col;
                ts.advance(); // '('
                vector<ExprPtr> args;
                if (!ts.match(TokenType::T_PARENR))
                {
                    while (true)
                    {
                        ExprPtr arg = parseExpression();
                        args.push_back(arg);
                        if (ts.match(TokenType::T_COMMA))
                            continue;
                        if (ts.match(TokenType::T_PARENR))
                            break;
                        throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(),
                                         "Expected ',' or ')' in argument list");
                    }
                }
                string fnName;
                if (auto id = dynamic_pointer_cast<IdentifierExpr>(left))
                    fnName = id->name;
                else
                    fnName = "<unknown_fn>";

                left = make_shared<CallExpr>(fnName, args, callLine, callCol);
                continue;
            }

            // array indexing: arr[expr]
            if (t.type == TokenType::T_BRACKETL)
            {
                int idxLine = t.line, idxCol = t.col;
                ts.advance(); // '['
                ExprPtr idx = parseExpression();
                if (!ts.match(TokenType::T_BRACKETR))
                    throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ']' after index expression");
                left = make_shared<IndexExpr>(left, idx, idxLine, idxCol);
                continue;
            }

            // postfix ++ / --
            if (t.type == TokenType::T_INC || t.type == TokenType::T_DEC)
            {
                int postLine = t.line, postCol = t.col;
                ts.advance();
                left = make_shared<PostfixExpr>(tokToOp(t), left, postLine, postCol);
                continue;
            }

            break;
        }
        return left;
    }

    ExprPtr parsePrefix()
    {
        Token t = ts.peek();
        DBG("[DBG] parsePrefix() - token: " << tokenToDisplay(t) << " type: " << (int)t.type);

        // 🔒 Prevent treating type keywords (int, float, string, bool, char) as expressions
        if (t.type == TokenType::T_INT || t.type == TokenType::T_FLOAT ||
            t.type == TokenType::T_STRING || t.type == TokenType::T_BOOL ||
            t.type == TokenType::T_CHAR) // <— add this
        {
            DBG("[DBG] parsePrefix() - rejecting type keyword");
            return nullptr; // let parseStmt handle declarations
        }

        // ---------- Literals ----------
        if (t.type == TokenType::T_INTLIT)
        {
            ts.advance();
            auto node = make_shared<IntLiteral>(t.lexeme, t.line, t.col);
            return parsePostfixTrail(node);
        }
        if (t.type == TokenType::T_FLOATLIT)
        {
            ts.advance();
            auto node = make_shared<FloatLiteral>(t.lexeme, t.line, t.col);
            return parsePostfixTrail(node);
        }
        if (t.type == TokenType::T_STRINGLIT)
        {
            ts.advance();
            auto node = make_shared<StringLiteral>(t.lexeme, t.line, t.col);
            return parsePostfixTrail(node);
        }
        if (t.type == TokenType::T_CHARLIT)
        {
            ts.advance();
            auto node = make_shared<CharLiteral>(t.lexeme, t.line, t.col);
            return parsePostfixTrail(node);
        }

        if (t.type == TokenType::T_BOOLLIT) // NEW: handle true/false
        {
            ts.advance();
            bool val = (t.lexeme == "true");
            auto node = make_shared<BoolLiteral>(val, t.line, t.col);
            return parsePostfixTrail(node);
        }

        // ---------- Identifiers and function calls ----------
        if (t.type == TokenType::T_IDENTIFIER)
        {
            ts.advance();
            auto id = make_shared<IdentifierExpr>(t.lexeme, t.line, t.col);

            // ✅ function call detection
            if (ts.match(TokenType::T_PARENL))
            {
                int callLine = t.line, callCol = t.col;
                vector<ExprPtr> args;
                if (!ts.match(TokenType::T_PARENR))
                {
                    while (true)
                    {
                        ExprPtr arg = parseExpression();
                        args.push_back(arg);
                        if (ts.match(TokenType::T_COMMA))
                            continue;
                        if (ts.match(TokenType::T_PARENR))
                            break;
                        throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ',' or ')'");
                    }
                }
                auto call = make_shared<CallExpr>(t.lexeme, args, callLine, callCol);
                return parsePostfixTrail(call);
            }
            return parsePostfixTrail(id);
        }

        // ---------- Parenthesized expressions ----------
        if (t.type == TokenType::T_PARENL)
        {
            int parenLine = t.line, parenCol = t.col;
            ts.advance();
            ExprPtr inside = parseExpression();
            if (!inside)
                throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(),
                                 "Expected expression inside parentheses");
            if (!ts.match(TokenType::T_PARENR))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(),
                                 "Expected ')'");
            // Parentheses don't need special AST node, just preserve child
            return parsePostfixTrail(inside);
        }

        // ---------- Unary operators ----------
        if (t.type == TokenType::T_MINUS || t.type == TokenType::T_NOT ||
            t.type == TokenType::T_INC || t.type == TokenType::T_DEC ||
            t.type == TokenType::T_PLUS)
        {
            int unLine = t.line, unCol = t.col;
            ts.advance();
            ExprPtr rhs = parseExpression(PREC_UNARY);
            if (!rhs)
                throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(),
                                 "Expected expression after unary operator");
            return make_shared<UnaryExpr>(tokToOp(t), rhs, unLine, unCol);
        }

        DBG("[DBG] parsePrefix() - not a valid prefix expression");
        return nullptr;
    }

    static int getPrecedence(TokenType t)
    {
        auto it = OP_TABLE.find(t);
        return (it != OP_TABLE.end()) ? it->second.precedence : 0;
    }

    static bool isRightAssoc(TokenType t)
    {
        auto it = OP_TABLE.find(t);
        return (it != OP_TABLE.end() && it->second.assoc == RIGHT);
    }
};