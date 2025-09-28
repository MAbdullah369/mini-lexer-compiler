// parser/parser.cpp
// Recursive-descent + Pratt parser for your lexer tokens.
// Intended to be #included after your lexer file is included.
// If you compile this file standalone, keep PARSER_REUSE_LEXER undefined
// to produce self-contained Token/TokenType fallback definitions.

#include <bits/stdc++.h>
using namespace std;

#ifndef PARSER_REUSE_LEXER
// Fallback token definitions (used only when PARSER_REUSE_LEXER is NOT defined).
enum class TokenType {
    T_FUNCTION, T_INT, T_FLOAT, T_STRING, T_BOOL, T_RETURN,
    T_IF, T_ELSE, T_WHILE, T_FOR,
    T_IDENTIFIER, T_INTLIT, T_FLOATLIT, T_STRINGLIT,
    T_QUOTES,
    T_LINECOMMENT, T_BLOCKCOMMENT,
    T_PARENL, T_PARENR, T_BRACEL, T_BRACER, T_BRACKETL, T_BRACKETR, T_COMMA, T_SEMICOLON,
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
static string tokenToDisplay(const Token &t) {
    if (t.type == TokenType::T_IDENTIFIER) return "T_IDENTIFIER(\"" + t.lexeme + "\")";
    if (t.type == TokenType::T_INTLIT) return "T_INTLIT(" + t.lexeme + ")";
    if (t.type == TokenType::T_FLOATLIT) return "T_FLOATLIT(" + t.lexeme + ")";
    if (t.type == TokenType::T_STRINGLIT) return "T_STRINGLIT(\"" + t.lexeme + "\")";
    // fallback for others:
    return "<TOKEN>";
}
#endif

// ---------- Parse error ----------
enum class ParseErrorKind {
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

struct ParseError : public std::exception {
    ParseErrorKind kind;
    Token token;
    string msg;
    ParseError(ParseErrorKind k, Token tk = {}, string m = "") : kind(k), token(tk), msg(m) {
        if (msg.empty()) {
            // default messages
            switch (kind) {
                case ParseErrorKind::UnexpectedEOF: msg = "Unexpected end of input"; break;
                case ParseErrorKind::FailedToFindToken: msg = "Failed to find token"; break;
                case ParseErrorKind::ExpectedTypeToken: msg = "Expected type token"; break;
                case ParseErrorKind::ExpectedIdentifier: msg = "Expected identifier"; break;
                case ParseErrorKind::UnexpectedToken: msg = "Unexpected token"; break;
                case ParseErrorKind::ExpectedFloatLit: msg = "Expected float literal"; break;
                case ParseErrorKind::ExpectedIntLit: msg = "Expected int literal"; break;
                case ParseErrorKind::ExpectedStringLit: msg = "Expected string literal"; break;
                case ParseErrorKind::ExpectedBoolLit: msg = "Expected bool literal"; break;
                case ParseErrorKind::ExpectedExpr: msg = "Expected expression"; break;
            }
        }
        // Append token info if available
        if (token.line || token.col || !token.lexeme.empty()) {
            msg += " (at line " + to_string(token.line) + ", col " + to_string(token.col) + ")";
            if (!token.lexeme.empty()) msg += " token=" + tokenToDisplay(token);
        }
    }
    const char* what() const noexcept override { return msg.c_str(); }
};

// ---------- AST nodes ----------
static void indent(ostream &os, int n) { for (int i=0;i<n;i++) os << "  "; }

struct ASTNode { virtual ~ASTNode() = default; virtual void print(ostream &os,int indent_level=0) const = 0; };

struct Expr : ASTNode {};
using ExprPtr = shared_ptr<Expr>;

struct IntLiteral : Expr { string val; IntLiteral(string v):val(move(v)){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Int("<<val<<")\n"; } };
struct FloatLiteral : Expr { string val; FloatLiteral(string v):val(move(v)){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Float("<<val<<")\n"; } };
struct StringLiteral : Expr { string val; StringLiteral(string v):val(move(v)){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"String(\""<<val<<"\")\n"; } };
struct IdentifierExpr : Expr { string name; IdentifierExpr(string n):name(move(n)){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Ident("<<name<<")\n"; } };
struct UnaryExpr : Expr { string op; ExprPtr rhs; UnaryExpr(string o, ExprPtr r):op(move(o)),rhs(r){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Unary("<<op<<")\n"; rhs->print(os,ind+1); } };
struct BinaryExpr : Expr { string op; ExprPtr lhs, rhs; BinaryExpr(string o, ExprPtr l, ExprPtr r):op(move(o)),lhs(l),rhs(r){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Binary("<<op<<")\n"; lhs->print(os,ind+1); rhs->print(os,ind+1); } };
struct CallExpr : Expr { string name; vector<ExprPtr> args; CallExpr(string n, vector<ExprPtr> a):name(move(n)),args(move(a)){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Call("<<name<<")\n"; for(auto &a:args) a->print(os,ind+1); } };

// Statements / declarations
struct Stmt : ASTNode {};
using StmtPtr = shared_ptr<Stmt>;

struct ExprStmt : Stmt { ExprPtr expr; ExprStmt(ExprPtr e):expr(e){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"ExprStmt\n"; if(expr) expr->print(os,ind+1); } };
struct ReturnStmt : Stmt { ExprPtr expr; ReturnStmt(ExprPtr e):expr(e){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Return\n"; if(expr) expr->print(os,ind+1); } };
struct VarDeclStmt : Stmt { TokenType typeTok; string name; ExprPtr init; VarDeclStmt(TokenType t,string n,ExprPtr i):typeTok(t),name(move(n)),init(i){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"VarDecl(type="<<(int)typeTok<<" name="<<name<<")\n"; if(init) init->print(os,ind+1); } };
struct BlockStmt : Stmt { vector<StmtPtr> stmts; void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Block\n"; for(auto &s:stmts) s->print(os,ind+1); } };
struct IfStmt : Stmt { ExprPtr cond; StmtPtr thenStmt; StmtPtr elseStmt; IfStmt(ExprPtr c, StmtPtr t, StmtPtr e=nullptr):cond(c),thenStmt(t),elseStmt(e){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"If\n"; cond->print(os,ind+1); indent(os,ind+1); os<<"Then:\n"; thenStmt->print(os,ind+2); if(elseStmt){ indent(os,ind+1); os<<"Else:\n"; elseStmt->print(os,ind+2);} } };
struct WhileStmt : Stmt { ExprPtr cond; StmtPtr body; WhileStmt(ExprPtr c, StmtPtr b):cond(c),body(b){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"While\n"; cond->print(os,ind+1); body->print(os,ind+1); } };
struct ForStmt : Stmt { ExprPtr init; ExprPtr cond; ExprPtr post; StmtPtr body; ForStmt(ExprPtr i, ExprPtr c, ExprPtr p, StmtPtr b):init(i),cond(c),post(p),body(b){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"For\n"; if(init){ indent(os,ind+1); os<<"Init:\n"; init->print(os,ind+2);} if(cond){ indent(os,ind+1); os<<"Cond:\n"; cond->print(os,ind+2);} if(post){ indent(os,ind+1); os<<"Post:\n"; post->print(os,ind+2);} indent(os,ind+1); os<<"Body:\n"; body->print(os,ind+2); } };

struct FnDecl : ASTNode { TokenType returnType; string name; vector<pair<TokenType,string>> params; vector<StmtPtr> body; FnDecl(TokenType rt,string n):returnType(rt),name(move(n)){} void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"FnDecl(name="<<name<<" type="<<(int)returnType<<")\n"; indent(os,ind+1); os<<"Params:\n"; for(auto &p:params){ indent(os,ind+2); os<<"(type="<<(int)p.first<<" name="<<p.second<<")\n"; } indent(os,ind+1); os<<"Body:\n"; for(auto &s:body) s->print(os,ind+2); } };

struct Program : ASTNode { vector<shared_ptr<ASTNode>> items; void print(ostream &os,int ind=0) const override { indent(os,ind); os<<"Program\n"; for(auto &it:items) it->print(os,ind+1); } };

// ---------- TokenStream (skips trivia: comments and quotes) ----------
struct TokenStream {
    vector<Token> tokens;
    size_t i = 0;
    TokenStream() = default;
    TokenStream(vector<Token> t): tokens(move(t)), i(0) {}

    static bool isTrivia(TokenType tt) {
        return tt == TokenType::T_LINECOMMENT || tt == TokenType::T_BLOCKCOMMENT;
    }

    // return index of first non-trivia token at or after idx
    size_t skipTriviaIndex(size_t idx) const {
        while (idx < tokens.size() && isTrivia(tokens[idx].type)) idx++;
        return idx;
    }

    Token peek() const {
        size_t idx = skipTriviaIndex(i);
        if (idx < tokens.size()) return tokens[idx];
        return Token{TokenType::T_EOF, "", 0, 0};
    }

    Token advance() {
        size_t idx = skipTriviaIndex(i);
        if (idx >= tokens.size()) {
            i = tokens.size();
            return Token{TokenType::T_EOF, "", 0, 0};
        }
        Token t = tokens[idx];
        i = idx + 1;
        return t;
    }

    bool match(TokenType t) {
        if (peek().type == t) { advance(); return true; }
        return false;
    }

    bool eof() const { return skipTriviaIndex(i) >= tokens.size() || tokens[skipTriviaIndex(i)].type == TokenType::T_EOF; }
};

// ---------- Pratt precedence and helpers ----------
enum Prec { PREC_NONE=0, PREC_ASSIGN=1, PREC_OR=2, PREC_AND=3, PREC_EQUALITY=4, PREC_COMPARISON=5, PREC_TERM=6, PREC_FACTOR=7, PREC_UNARY=8, PREC_CALL=9 };

static string tokToOp(const Token &t) {
    switch (t.type) {
        case TokenType::T_PLUS: return "+";
        case TokenType::T_MINUS: return "-";
        case TokenType::T_MULTIPLY: return "*";
        case TokenType::T_DIVIDE: return "/";
        case TokenType::T_EQUALSOP: return "==";
        case TokenType::T_NOTEQUAL: return "!=";
        case TokenType::T_LESS: return "<";
        case TokenType::T_GREATER: return ">";
        case TokenType::T_LESSEQ: return "<=";
        case TokenType::T_GREATEREQ: return ">=";
        case TokenType::T_AND: return "&&";
        case TokenType::T_OR: return "||";
        case TokenType::T_ASSIGNOP: return "=";
        default: return "?";
    }
}

struct Parser {
    TokenStream ts;
    Parser() = default;
    Parser(vector<Token> tokens): ts(move(tokens)) {}

    // Top-level
    Program parseProgram() {
        Program prog;
        while (!ts.eof()) {
            Token p = ts.peek();
            // top-level may start with 'fn' or a type token
            if (p.type == TokenType::T_FUNCTION || p.type == TokenType::T_INT || p.type == TokenType::T_FLOAT || p.type == TokenType::T_STRING || p.type == TokenType::T_BOOL) {
                auto decl = parseTopLevelDecl();
                if (decl) prog.items.push_back(decl);
            } else {
                // if unexpected token at top-level, raise error
                if (p.type == TokenType::T_EOF) break;
                throw ParseError(ParseErrorKind::UnexpectedToken, p, "Unexpected token at top-level");
            }
        }
        return prog;
    }

    shared_ptr<ASTNode> parseTopLevelDecl() {
        Token first = ts.peek();
        if (first.type == TokenType::T_FUNCTION) {
            // fn <type> <ident> ( params ) { body }
            ts.advance(); // consume fn
            Token rt = ts.peek();
            if (!(rt.type==TokenType::T_INT || rt.type==TokenType::T_FLOAT || rt.type==TokenType::T_STRING || rt.type==TokenType::T_BOOL)) {
                throw ParseError(ParseErrorKind::ExpectedTypeToken, rt, "Expected return type after 'fn'");
            }
            ts.advance(); // consume return type
            Token id = ts.peek();
            if (id.type != TokenType::T_IDENTIFIER) throw ParseError(ParseErrorKind::ExpectedIdentifier, id, "Expected function name after return type");
            ts.advance(); // consume identifier
            if (!ts.match(TokenType::T_PARENL)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after function name");
            auto fn = make_shared<FnDecl>(rt.type, id.lexeme);
            // params
            if (!ts.match(TokenType::T_PARENR)) {
                while (true) {
                    Token ptype = ts.peek(); if (!(ptype.type==TokenType::T_INT || ptype.type==TokenType::T_FLOAT || ptype.type==TokenType::T_STRING || ptype.type==TokenType::T_BOOL))
                        throw ParseError(ParseErrorKind::ExpectedTypeToken, ptype, "Expected parameter type");
                    ts.advance();
                    Token pname = ts.peek(); if (pname.type != TokenType::T_IDENTIFIER) throw ParseError(ParseErrorKind::ExpectedIdentifier, pname, "Expected parameter name");
                    ts.advance();
                    fn->params.push_back({ptype.type, pname.lexeme});
                    if (ts.match(TokenType::T_COMMA)) continue;
                    if (ts.match(TokenType::T_PARENR)) break;
                    throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ',' or ')' in parameter list");
                }
            }
            // body
            if (!ts.match(TokenType::T_BRACEL)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '{' to start function body");
            while (!ts.match(TokenType::T_BRACER)) {
                fn->body.push_back(parseStmt());
            }
            return fn;
        }

        // Otherwise variable declaration or function without 'fn'
        // Expect type
        Token typeTok = ts.peek();
        if (!(typeTok.type==TokenType::T_INT || typeTok.type==TokenType::T_FLOAT|| typeTok.type==TokenType::T_STRING || typeTok.type==TokenType::T_BOOL)) {
            throw ParseError(ParseErrorKind::ExpectedTypeToken, typeTok, "Expected type token");
        }
        ts.advance(); // consume type
        Token id = ts.peek();
        if (id.type != TokenType::T_IDENTIFIER) throw ParseError(ParseErrorKind::ExpectedIdentifier, id, "Expected identifier");
        ts.advance(); // consume identifier

        // function-style if next is '('
        if (ts.match(TokenType::T_PARENL)) {
            auto fn = make_shared<FnDecl>(typeTok.type, id.lexeme);
            if (!ts.match(TokenType::T_PARENR)) {
                while (true) {
                    Token ptype = ts.peek(); if (!(ptype.type==TokenType::T_INT || ptype.type==TokenType::T_FLOAT || ptype.type==TokenType::T_STRING || ptype.type==TokenType::T_BOOL)) throw ParseError(ParseErrorKind::ExpectedTypeToken, ptype, "Expected param type");
                    ts.advance();
                    Token pname = ts.peek(); if (pname.type!=TokenType::T_IDENTIFIER) throw ParseError(ParseErrorKind::ExpectedIdentifier, pname, "Expected param name");
                    ts.advance();
                    fn->params.push_back({ptype.type, pname.lexeme});
                    if (ts.match(TokenType::T_COMMA)) continue;
                    if (ts.match(TokenType::T_PARENR)) break;
                    throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ',' or ')'");
                }
            }
            if (!ts.match(TokenType::T_BRACEL)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '{' for function body");
            while (!ts.match(TokenType::T_BRACER)) {
                fn->body.push_back(parseStmt());
            }
            return fn;
        } else {
            // var decl
            ExprPtr init = nullptr;
            if (ts.match(TokenType::T_ASSIGNOP)) {
                init = parseExpression();
            }
            if (!ts.match(TokenType::T_SEMICOLON)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after variable declaration");
            return make_shared<VarDeclStmt>(typeTok.type, id.lexeme, init);
        }
    }

    // Statements
    StmtPtr parseStmt() {
        Token t = ts.peek();
        cerr << "[DBG] parseStmt() peek -> type=" << (int)t.type << " token=" << tokenToDisplay(t)
        << " line=" << t.line << " col=" << t.col << "\n";
        if (t.type == TokenType::T_INT || t.type == TokenType::T_FLOAT ||
            t.type == TokenType::T_STRING || t.type == TokenType::T_BOOL) {
            Token typeTok = ts.advance();
            Token name = ts.peek();
            if (name.type != TokenType::T_IDENTIFIER)
            throw ParseError(ParseErrorKind::ExpectedIdentifier, name, "Expected identifier in variable declaration");
            ts.advance();
            ExprPtr init = nullptr;
            if (ts.match(TokenType::T_ASSIGNOP)) init = parseExpression();
            if (!ts.match(TokenType::T_SEMICOLON))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after variable declaration");
            return make_shared<VarDeclStmt>(typeTok.type, name.lexeme, init);
        }

        if (t.type == TokenType::T_RETURN) {
            ts.advance();
            ExprPtr e = nullptr;
            if (ts.peek().type != TokenType::T_SEMICOLON) {
                e = parseExpression();
            }
            if (!ts.match(TokenType::T_SEMICOLON)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after return");
            return make_shared<ReturnStmt>(e);
        }

        /*if (t.type == TokenType::T_INT || t.type == TokenType::T_FLOAT || 
            t.type == TokenType::T_STRING || t.type == TokenType::T_BOOL) {
            // local var decl
            Token typeTok = ts.advance();
            Token name = ts.peek();
            if (name.type != TokenType::T_IDENTIFIER) 
            throw ParseError(ParseErrorKind::ExpectedIdentifier, name, "Expected identifier in variable declaration");
            ts.advance();
            ExprPtr init = nullptr;
            if (ts.match(TokenType::T_ASSIGNOP)) init = parseExpression();
            if (!ts.match(TokenType::T_SEMICOLON)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after var declaration");
            return make_shared<VarDeclStmt>(typeTok.type, name.lexeme, init);
        }*/

        if (t.type == TokenType::T_IF) {
            ts.advance(); // consume if
            if (!ts.match(TokenType::T_PARENL)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after 'if'");
            ExprPtr cond = parseExpression();
            if (!ts.match(TokenType::T_PARENR)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ')' after if condition");
            StmtPtr thenStmt = parseStmtOrBlock();
            StmtPtr elseStmt = nullptr;
            if (ts.match(TokenType::T_ELSE)) {
                elseStmt = parseStmtOrBlock();
            }
            return make_shared<IfStmt>(cond, thenStmt, elseStmt);
        }

        if (t.type == TokenType::T_WHILE) {
            ts.advance();
            if (!ts.match(TokenType::T_PARENL)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after 'while'");
            ExprPtr cond = parseExpression();
            if (!ts.match(TokenType::T_PARENR)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ')' after while condition");
            StmtPtr body = parseStmtOrBlock();
            return make_shared<WhileStmt>(cond, body);
        }

        if (t.type == TokenType::T_FOR) {
            ts.advance();
            if (!ts.match(TokenType::T_PARENL)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected '(' after 'for'");
            // init (optional)
            ExprPtr init = nullptr, cond = nullptr, post = nullptr;
            if (ts.peek().type != TokenType::T_SEMICOLON) {
                // allow var decl or expression
                if (ts.peek().type==TokenType::T_INT || ts.peek().type==TokenType::T_FLOAT || ts.peek().type==TokenType::T_STRING || ts.peek().type==TokenType::T_BOOL) {
                    // parse var decl inside for - we will convert to expression by ignoring type
                    Token typeTok = ts.advance();
                    Token name = ts.peek(); if (name.type!=TokenType::T_IDENTIFIER) throw ParseError(ParseErrorKind::ExpectedIdentifier, name, "Expected identifier in for-init");
                    ts.advance();
                    if (ts.match(TokenType::T_ASSIGNOP)) init = parseExpression();
                    else init = nullptr;
                } else {
                    init = parseExpression();
                }
            }
            if (!ts.match(TokenType::T_SEMICOLON)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' in for loop after init");
            if (ts.peek().type != TokenType::T_SEMICOLON) cond = parseExpression();
            if (!ts.match(TokenType::T_SEMICOLON)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' in for loop after cond");
            if (ts.peek().type != TokenType::T_PARENR) post = parseExpression();
            if (!ts.match(TokenType::T_PARENR)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ')' to close for loop header");
            StmtPtr body = parseStmtOrBlock();
            return make_shared<ForStmt>(init, cond, post, body);
        }

        if (t.type == TokenType::T_BRACEL) {
            // block
            ts.advance(); // consume '{'
            auto block = make_shared<BlockStmt>();
            while (!ts.match(TokenType::T_BRACER)) {
                // handle possible EOF
                if (ts.eof()) throw ParseError(ParseErrorKind::UnexpectedEOF, ts.peek(), "Unterminated block");
                block->stmts.push_back(parseStmt());
            }
            return block;
        }

        // Expression statement
        ExprPtr e = parseExpression();
        if (!ts.match(TokenType::T_SEMICOLON)) throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(), "Expected ';' after expression");
        return make_shared<ExprStmt>(e);
    }

    StmtPtr parseStmtOrBlock() {
        Token p = ts.peek();
        if (p.type == TokenType::T_BRACEL) return parseStmt(); // parseStmt handles block
        return parseStmt();
    }

    // ---------- Pratt parser ----------
    ExprPtr parseExpression(int minPrec = 0) {
        ExprPtr left = parsePrefix();
        if (!left) throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(), "Expected expression");
        while (true) {
            Token op = ts.peek();
            int prec = getBinaryPrec(op.type);
            if (prec < minPrec) break;

            // consume operator
            Token opTok = ts.advance();
            // Determine associativity: assignment is right-assoc
            int nextMin = prec + (isRightAssoc(opTok.type) ? 0 : 1);
            ExprPtr right = parseExpression(nextMin);
            if (!right) throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(), "Expected right-hand expression after operator " + tokToOp(opTok));
            left = make_shared<BinaryExpr>(tokToOp(opTok), left, right);
        }
        return left;
    }

    ExprPtr parsePrefix() {
        Token t = ts.peek();

        // 🔒 Prevent treating type keywords (int, float, string, bool) as expressions
        if (t.type == TokenType::T_INT || t.type == TokenType::T_FLOAT ||
            t.type == TokenType::T_STRING || t.type == TokenType::T_BOOL) {
            return nullptr; // stop here, let parseStmt handle them as declarations
        }

        if (t.type == TokenType::T_INTLIT) {
            ts.advance();
            return make_shared<IntLiteral>(t.lexeme);
        }
        if (t.type == TokenType::T_FLOATLIT) {
            ts.advance();
            return make_shared<FloatLiteral>(t.lexeme);
        }
        if (t.type == TokenType::T_STRINGLIT) {
            ts.advance();
            return make_shared<StringLiteral>(t.lexeme);
        }

        if (t.type == TokenType::T_IDENTIFIER) {
            ts.advance();
            // function call?
            if (ts.match(TokenType::T_PARENL)) {
                vector<ExprPtr> args;
                if (!ts.match(TokenType::T_PARENR)) {
                    while (true) {
                        ExprPtr a = parseExpression();
                        args.push_back(a);
                        if (ts.match(TokenType::T_COMMA)) continue;
                        if (ts.match(TokenType::T_PARENR)) break;
                        throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(),
                                "Expected ',' or ')' in argument list");
                    }
                }
                return make_shared<CallExpr>(t.lexeme, args);
            }
            return make_shared<IdentifierExpr>(t.lexeme);
        }   

        if (t.type == TokenType::T_PARENL) {
            ts.advance();
            ExprPtr inside = parseExpression();
            if (!inside)
                throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(),
                            "Expected expression inside parentheses");
            if (!ts.match(TokenType::T_PARENR))
                throw ParseError(ParseErrorKind::UnexpectedToken, ts.peek(),
                            "Expected ')'");
            return inside;
        }

        if (t.type == TokenType::T_MINUS || t.type == TokenType::T_NOT) {
            ts.advance();
            ExprPtr rhs = parseExpression(PREC_UNARY);
            if (!rhs)
                throw ParseError(ParseErrorKind::ExpectedExpr, ts.peek(),
                            "Expected expression after unary operator");
            return make_shared<UnaryExpr>(tokToOp(t), rhs);
        }

        // 🔽 Handle string literals wrapped in quotes
        if (t.type == TokenType::T_STRINGLIT) {
            ts.advance();
            return make_shared<StringLiteral>(t.lexeme);
        }


        // Not a valid prefix expression
        return nullptr;
    }



    int getBinaryPrec(TokenType t) {
        switch (t) {
            case TokenType::T_ASSIGNOP: return PREC_ASSIGN; // '=' if you want assignment as expression
            case TokenType::T_OR: return PREC_OR;
            case TokenType::T_AND: return PREC_AND;
            case TokenType::T_EQUALSOP: case TokenType::T_NOTEQUAL: return PREC_EQUALITY;
            case TokenType::T_LESS: case TokenType::T_LESSEQ: case TokenType::T_GREATER: case TokenType::T_GREATEREQ: return PREC_COMPARISON;
            case TokenType::T_PLUS: case TokenType::T_MINUS: return PREC_TERM;
            case TokenType::T_MULTIPLY: case TokenType::T_DIVIDE: return PREC_FACTOR;
            default: return PREC_NONE;
        }
    }

    bool isRightAssoc(TokenType t) {
        return t == TokenType::T_ASSIGNOP; // assignment is right-associative
    }
};
