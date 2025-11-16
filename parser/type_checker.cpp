// type_checker.cpp
#define PARSER_REUSE_LEXER
#include "../regex/regex_code.cpp"
#include "parser.cpp"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

using namespace std;

// ---------------- Type Error Enum ------------------
enum class TypeChkError
{
    ErroneousVarDecl,
    FnCallParamCount,
    FnCallParamType,
    ErroneousReturnType,
    ExpressionTypeMismatch,
    ExpectedBooleanExpression,
    ErroneousBreak,
    NonBooleanCondStmt,
    EmptyExpression,
    AttemptedBoolOpOnNonBools,
    AttemptedBitOpOnNonNumeric,
    AttemptedShiftOnNonInt,
    AttemptedAddOpOnNonNumeric,
    AttemptedExponentiationOfNonNumeric,
    ReturnStmtNotFound,
};

string errorToString(TypeChkError e)
{
    switch (e)
    {
    case TypeChkError::ErroneousVarDecl:
        return "ErroneousVarDecl";
    case TypeChkError::FnCallParamCount:
        return "FnCallParamCount";
    case TypeChkError::FnCallParamType:
        return "FnCallParamType";
    case TypeChkError::ErroneousReturnType:
        return "ErroneousReturnType";
    case TypeChkError::ExpressionTypeMismatch:
        return "ExpressionTypeMismatch";
    case TypeChkError::ExpectedBooleanExpression:
        return "ExpectedBooleanExpression";
    case TypeChkError::ErroneousBreak:
        return "ErroneousBreak";
    case TypeChkError::NonBooleanCondStmt:
        return "NonBooleanCondStmt";
    case TypeChkError::EmptyExpression:
        return "EmptyExpression";
    case TypeChkError::AttemptedBoolOpOnNonBools:
        return "AttemptedBoolOpOnNonBools";
    case TypeChkError::AttemptedBitOpOnNonNumeric:
        return "AttemptedBitOpOnNonNumeric";
    case TypeChkError::AttemptedShiftOnNonInt:
        return "AttemptedShiftOnNonInt";
    case TypeChkError::AttemptedAddOpOnNonNumeric:
        return "AttemptedAddOpOnNonNumeric";
    case TypeChkError::AttemptedExponentiationOfNonNumeric:
        return "AttemptedExponentiationOfNonNumeric";
    case TypeChkError::ReturnStmtNotFound:
        return "ReturnStmtNotFound";
    default:
        return "UnknownTypeError";
    }
}

struct TypeError
{
    TypeChkError type;
    int line, col;
    string detail;
    TypeError(TypeChkError t, int l, int c, string d)
        : type(t), line(l), col(c), detail(d) {}
};

struct TypeScopeSymbol
{
    TokenType typeTok = TokenType::T_UNKNOWN;
    bool isFunc = false;
    vector<TokenType> paramTypes;
};

class TypeChecker
{
public:
    vector<TypeError> errors;
    vector<unordered_map<string, TypeScopeSymbol>> stack;
    int functionDepth = 0;
    int loopDepth = 0;

    void check(const Program &prog)
    {
        errors.clear();
        stack.clear();
        functionDepth = 0;
        loopDepth = 0;
        pushScope();

        // Add all global vars and functions first
        for (const auto &item : prog.items)
        {
            if (auto fn = dynamic_pointer_cast<FnDecl>(item))
            {
                declareFunction(fn);
            }
            else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item))
            {
                declareVariable(var);
            }
        }

        // Now type-check all items (full walk)
        for (const auto &item : prog.items)
        {
            if (auto fn = dynamic_pointer_cast<FnDecl>(item))
            {
                checkFunction(fn);
            }
            else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item))
            {
                checkVarDecl(var, true);
            }
            else if (auto stmt = dynamic_pointer_cast<Stmt>(item))
            {
                checkStmt(stmt, TokenType::T_UNKNOWN);
            }
        }

        popScope();
    }

private:
    void pushScope() { stack.push_back({}); }
    void popScope()
    {
        if (!stack.empty())
            stack.pop_back();
    }

    void declareVariable(shared_ptr<VarDeclStmt> var)
    {
        auto &current = stack.back();
        if (current.count(var->name))
        {
            errors.emplace_back(TypeChkError::ErroneousVarDecl, var->line, var->col, "Global variable '" + var->name + "' redefined");
        }
        else
        {
            current[var->name] = TypeScopeSymbol{var->typeTok, false, {}};
        }
    }

    void declareFunction(shared_ptr<FnDecl> fn)
    {
        auto &current = stack.back();
        if (current.count(fn->name))
        {
            auto &sym = current[fn->name];
            if (!sym.isFunc || sym.paramTypes.size() != fn->params.size())
                errors.emplace_back(TypeChkError::ErroneousVarDecl, fn->line, fn->col, "Function '" + fn->name + "' redefined with different signature");
        }
        else
        {
            vector<TokenType> pts;
            for (auto &pr : fn->params)
                pts.push_back(pr.first);
            current[fn->name] = TypeScopeSymbol{fn->returnType, true, pts};
        }
    }

    TypeScopeSymbol *lookup(const string &name)
    {
        for (int i = stack.size() - 1; i >= 0; --i)
            if (stack[i].count(name))
                return &stack[i][name];
        return nullptr;
    }

    void checkVarDecl(shared_ptr<VarDeclStmt> var, bool global)
    {
        if (!isTypeValid(var->typeTok))
        {
            errors.emplace_back(TypeChkError::ErroneousVarDecl, var->line, var->col, "Invalid type for variable '" + var->name + "'");
        }
        if (!global && stack.back().count(var->name))
        {
            errors.emplace_back(TypeChkError::ErroneousVarDecl, var->line, var->col, "Variable '" + var->name + "' redefined in local scope");
        }
        else if (!global)
        {
            stack.back()[var->name] = TypeScopeSymbol{var->typeTok, false, {}};
        }
        if (var->init)
        {
            auto exprType = getExprType(var->init);
            if (exprType != var->typeTok)
            {
                errors.emplace_back(TypeChkError::ExpressionTypeMismatch, var->line, var->col, "Initializer for '" + var->name + "' type mismatch");
            }
        }
    }

    // Recursively finds a ReturnStmt inside any statement/block
    bool containsReturn(const shared_ptr<Stmt> &stmt)
    {
        if (!stmt)
            return false;
        if (dynamic_pointer_cast<ReturnStmt>(stmt))
        {
            cout << "CONTAINS RETURN at line " << stmt->line << endl;
            return true;
        }
        if (auto block = dynamic_pointer_cast<BlockStmt>(stmt))
            for (auto &s : block->stmts)
                if (containsReturn(s))
                    return true;
        if (auto ifStmt = dynamic_pointer_cast<IfStmt>(stmt))
            return containsReturn(ifStmt->thenStmt) || (ifStmt->elseStmt && containsReturn(ifStmt->elseStmt));
        if (auto whileStmt = dynamic_pointer_cast<WhileStmt>(stmt))
            return containsReturn(whileStmt->body);
        if (auto doWhile = dynamic_pointer_cast<DoWhileStmt>(stmt))
            return containsReturn(doWhile->body);
        if (auto forStmt = dynamic_pointer_cast<ForStmt>(stmt))
            return containsReturn(forStmt->body);
        return false;
    }

    void checkFunction(shared_ptr<FnDecl> fn)
    {
        functionDepth++;
        pushScope();

        unordered_map<string, bool> paramNames;
        for (auto &pr : fn->params)
        {
            if (stack.back().count(pr.second))
                errors.emplace_back(TypeChkError::ErroneousVarDecl, fn->line, fn->col, "Parameter '" + pr.second + "' redefined in function '" + fn->name + "'");
            else
                stack.back()[pr.second] = TypeScopeSymbol{pr.first, false, {}};
            if (paramNames.count(pr.second))
                errors.emplace_back(TypeChkError::FnCallParamType, fn->line, fn->col, "Duplicate function parameter name: " + pr.second);
            paramNames[pr.second] = true;
        }

        // SIMPLIFIED: Just check if ANY statement in the function body contains a return
        bool foundReturn = false;
        for (auto &stmt : fn->body)
        {
            if (containsReturn(stmt))
            {
                foundReturn = true;
                break; // No need to check further once we find one
            }
        }

        // Also check the function body directly for return statements
        // This handles the case where return is at the top level of the function
        for (auto &stmt : fn->body)
        {
            if (dynamic_pointer_cast<ReturnStmt>(stmt))
            {
                foundReturn = true;
                break;
            }
        }

        if (fn->returnType != TokenType::T_UNKNOWN && !foundReturn)
        {
            // Special case: main function might not need a return (depending on your language spec)
            if (fn->name == "main" && fn->returnType == TokenType::T_INT)
            {
                // Many languages allow main to implicitly return 0
                // If you want to be strict, keep the error; if lenient, comment this out
                // errors.emplace_back(TypeChkError::ReturnStmtNotFound, fn->line, fn->col, "Function 'main' missing return statement");
            }
            else
            {
                errors.emplace_back(TypeChkError::ReturnStmtNotFound, fn->line, fn->col, "Function '" + fn->name + "' missing return statement");
            }
        }

        // Still need to type-check all statements
        for (auto &stmt : fn->body)
        {
            checkStmt(stmt, fn->returnType);
        }

        popScope();
        functionDepth--;
    }

    // Helper for nested blocks (ensure returns inside blocks in function body get detected)
    bool blockContainsReturn(const shared_ptr<BlockStmt> &block)
    {
        for (const auto &stmt : block->stmts)
        {
            if (dynamic_pointer_cast<ReturnStmt>(stmt))
                return true;
            if (auto subBlock = dynamic_pointer_cast<BlockStmt>(stmt))
                if (blockContainsReturn(subBlock))
                    return true;
            // Also check for returns in if/while/for/else
            if (auto ifStmt = dynamic_pointer_cast<IfStmt>(stmt))
            {
                if (blockContainsReturn(dynamic_pointer_cast<BlockStmt>(ifStmt->thenStmt)))
                    return true;
                if (ifStmt->elseStmt && blockContainsReturn(dynamic_pointer_cast<BlockStmt>(ifStmt->elseStmt)))
                    return true;
            }
            if (auto whileStmt = dynamic_pointer_cast<WhileStmt>(stmt))
            {
                if (blockContainsReturn(dynamic_pointer_cast<BlockStmt>(whileStmt->body)))
                    return true;
            }
            if (auto doWhile = dynamic_pointer_cast<DoWhileStmt>(stmt))
            {
                if (blockContainsReturn(dynamic_pointer_cast<BlockStmt>(doWhile->body)))
                    return true;
            }
            if (auto forStmt = dynamic_pointer_cast<ForStmt>(stmt))
            {
                if (blockContainsReturn(dynamic_pointer_cast<BlockStmt>(forStmt->body)))
                    return true;
            }
        }
        return false;
    }

    void checkStmt(shared_ptr<Stmt> stmt, TokenType expectedReturnType)
    {
        if (!stmt)
            return;
        if (auto block = dynamic_pointer_cast<BlockStmt>(stmt))
        {
            pushScope();
            for (auto &sub : block->stmts)
                checkStmt(sub, expectedReturnType);
            popScope();
        }
        else if (auto varDecl = dynamic_pointer_cast<VarDeclStmt>(stmt))
        {
            checkVarDecl(varDecl, false);
        }
        else if (auto exprStmt = dynamic_pointer_cast<ExprStmt>(stmt))
        {
            if (exprStmt->expr)
                checkExpr(exprStmt->expr);
        }
        else if (auto ret = dynamic_pointer_cast<ReturnStmt>(stmt))
        {
            if (!ret->expr)
            {
                errors.emplace_back(TypeChkError::EmptyExpression, ret->line, ret->col, "Return statement missing expression");
            }
            else if (getExprType(ret->expr) != expectedReturnType)
            {
                errors.emplace_back(TypeChkError::ErroneousReturnType, ret->line, ret->col, "Return type mismatch");
            }
        }
        else if (auto ifStmt = dynamic_pointer_cast<IfStmt>(stmt))
        {
            auto condType = getExprType(ifStmt->cond);
            if (condType != TokenType::T_BOOL)
                errors.emplace_back(TypeChkError::ExpectedBooleanExpression, ifStmt->line, ifStmt->col, "Condition of if is not boolean");
            checkStmt(ifStmt->thenStmt, expectedReturnType);
            if (ifStmt->elseStmt)
                checkStmt(ifStmt->elseStmt, expectedReturnType);
        }
        else if (auto whileStmt = dynamic_pointer_cast<WhileStmt>(stmt))
        {
            loopDepth++;
            auto condType = getExprType(whileStmt->cond);
            if (condType != TokenType::T_BOOL)
                errors.emplace_back(TypeChkError::NonBooleanCondStmt, whileStmt->line, whileStmt->col, "While condition not boolean");
            checkStmt(whileStmt->body, expectedReturnType);
            loopDepth--;
        }
        else if (auto doWhile = dynamic_pointer_cast<DoWhileStmt>(stmt))
        {
            loopDepth++;
            checkStmt(doWhile->body, expectedReturnType);
            auto condType = getExprType(doWhile->cond);
            if (condType != TokenType::T_BOOL)
                errors.emplace_back(TypeChkError::NonBooleanCondStmt, doWhile->line, doWhile->col, "Do-while condition not boolean");
            loopDepth--;
        }
        else if (auto forStmt = dynamic_pointer_cast<ForStmt>(stmt))
        {
            loopDepth++;
            if (forStmt->init)
            {
                if (auto decl = dynamic_pointer_cast<VarDeclStmt>(forStmt->init))
                {
                    checkVarDecl(decl, false);
                }
                else
                {
                    checkExpr(forStmt->init);
                }
            }
            if (forStmt->cond)
            {
                auto condType = getExprType(forStmt->cond);
                if (condType != TokenType::T_BOOL)
                    errors.emplace_back(TypeChkError::NonBooleanCondStmt, forStmt->line, forStmt->col, "For condition not boolean");
            }
            if (forStmt->post)
                checkExpr(forStmt->post);
            checkStmt(forStmt->body, expectedReturnType);
            loopDepth--;
        }
        else if (auto brk = dynamic_pointer_cast<BreakStmt>(stmt))
        {
            if (loopDepth <= 0) // use your mechanism to track if we're inside a loop
                errors.emplace_back(TypeChkError::ErroneousBreak, stmt->line, stmt->col, "break outside of loop");
        }
    }

    void checkExpr(shared_ptr<Expr> expr)
    {
        auto type = getExprType(expr);
        if (type == TokenType::T_UNKNOWN && expr)
            errors.emplace_back(TypeChkError::EmptyExpression, expr->line, expr->col, "Expression could not be resolved");
    }

    TokenType getExprType(shared_ptr<Expr> expr)
    {
        if (!expr)
            return TokenType::T_UNKNOWN;
        if (auto var = dynamic_pointer_cast<IdentifierExpr>(expr))
        {
            auto sym = lookup(var->name);
            return sym ? sym->typeTok : TokenType::T_UNKNOWN;
        }
        else if (auto lit = dynamic_pointer_cast<IntLiteral>(expr))
        {
            return TokenType::T_INT;
        }
        else if (auto lit = dynamic_pointer_cast<BoolLiteral>(expr))
        {
            return TokenType::T_BOOL;
        }
        else if (auto bin = dynamic_pointer_cast<BinaryExpr>(expr))
        {
            auto lt = getExprType(bin->lhs), rt = getExprType(bin->rhs);
            string op = bin->op;

            if (op == "+" || op == "-")
            {
                if (!isNumericType(lt) || !isNumericType(rt))
                    errors.emplace_back(TypeChkError::AttemptedAddOpOnNonNumeric, bin->line, bin->col, "Add/Sub on non-numeric types");
                return lt == rt ? lt : TokenType::T_UNKNOWN;
            }
            if (op == "*" || op == "/")
            {
                if (!isNumericType(lt) || !isNumericType(rt))
                    errors.emplace_back(TypeChkError::AttemptedBitOpOnNonNumeric, bin->line, bin->col, "Mul/Div on non-numeric types");
                return lt == rt ? lt : TokenType::T_UNKNOWN;
            }
            if (op == "&&" || op == "||")
            {
                if (lt != TokenType::T_BOOL || rt != TokenType::T_BOOL)
                    errors.emplace_back(TypeChkError::AttemptedBoolOpOnNonBools, bin->line, bin->col, "Boolean operations on non-bool types");
                return TokenType::T_BOOL;
            }
            if (op == "&" || op == "|" || op == "^")
            {
                if (!isNumericType(lt) || !isNumericType(rt))
                    errors.emplace_back(TypeChkError::AttemptedBitOpOnNonNumeric, bin->line, bin->col, "Bitwise on non-numeric");
                return lt == rt ? lt : TokenType::T_UNKNOWN;
            }
            if (op == "<<" || op == ">>")
            {
                if (lt != TokenType::T_INT || rt != TokenType::T_INT)
                    errors.emplace_back(TypeChkError::AttemptedShiftOnNonInt, bin->line, bin->col, "Shift operator on non-int");
                return TokenType::T_INT;
            }
            if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=")
            {
                if (lt == TokenType::T_UNKNOWN || rt == TokenType::T_UNKNOWN)
                    errors.emplace_back(TypeChkError::ExpressionTypeMismatch, bin->line, bin->col, "Comparison between unknown types");
                return TokenType::T_BOOL;
            }
            if (op == "**")
            {
                if (!isNumericType(lt) || !isNumericType(rt))
                    errors.emplace_back(TypeChkError::AttemptedExponentiationOfNonNumeric, bin->line, bin->col, "Exponentiation on non-numeric types");
                return lt == rt ? lt : TokenType::T_UNKNOWN;
            }
            if (op == "=")
            {
                if (lt != rt)
                    errors.emplace_back(TypeChkError::ExpressionTypeMismatch, bin->line, bin->col, "Assignment of different types");
                return lt;
            }
            return TokenType::T_UNKNOWN;
        }
        else if (auto call = dynamic_pointer_cast<CallExpr>(expr))
        {
            auto sym = lookup(call->name);
            if (!sym || !sym->isFunc)
            {
                errors.emplace_back(TypeChkError::FnCallParamType, call->line, call->col, "Call to undefined function '" + call->name + "'");
                return TokenType::T_UNKNOWN;
            }
            if (call->args.size() != sym->paramTypes.size())
            {
                errors.emplace_back(TypeChkError::FnCallParamCount, call->line, call->col, "Function '" + call->name + "' parameter count mismatch");
            }
            else
            {
                for (size_t i = 0; i < call->args.size(); ++i)
                {
                    auto argType = getExprType(call->args[i]);
                    if (argType != sym->paramTypes[i])
                        errors.emplace_back(TypeChkError::FnCallParamType, call->line, call->col, "Function '" + call->name + "' param type mismatch for arg " + to_string(i));
                }
            }
            return sym->typeTok;
        }
        else if (auto unary = dynamic_pointer_cast<UnaryExpr>(expr))
        {
            auto subType = getExprType(unary->rhs);
            string op = unary->op;
            if (op == "!")
            {
                if (subType != TokenType::T_BOOL)
                    errors.emplace_back(TypeChkError::AttemptedBoolOpOnNonBools, unary->line, unary->col, "Logical NOT on non-bool type");
                return TokenType::T_BOOL;
            }
            if (op == "-")
            {
                if (!isNumericType(subType))
                    errors.emplace_back(TypeChkError::AttemptedAddOpOnNonNumeric, unary->line, unary->col, "Unary minus on non-numeric type");
                return subType;
            }
            return TokenType::T_UNKNOWN;
        }
        else
        {
            // For unrecognized expression types
            return TokenType::T_UNKNOWN;
        }
    }

    bool isNumericType(TokenType t)
    {
        return t == TokenType::T_INT || t == TokenType::T_FLOAT;
    }
    bool isTypeValid(TokenType t)
    {
        // Never allow unknown as a "user" type!
        return (t == TokenType::T_INT || t == TokenType::T_BOOL ||
                t == TokenType::T_FLOAT || t == TokenType::T_STRING);
    }
};

// -------------- File Reading Utility -----------------
string readFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Cannot open file: " + filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ------------------ DRIVER --------------------------
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const string inputFile = "sample.txt";
    cout << "================================================================================\n";
    cout << "                        COMPREHENSIVE TYPE CHECKER\n";
    cout << "================================================================================\n\n";
    try
    {
        string source = readFile(inputFile);
        cout << "Reading input from: " << inputFile << "\nSOURCE CODE:\n------------------------------------------------------------------------\n";
        cout << source << "\n------------------------------------------------------------------------\n\n";
        // Lexical Analysis
        cout << "PHASE 1: LEXICAL ANALYSIS\n========================================================================\n";
        RegexLexer lexer(source);
        auto tokens = lexer.tokenize();
        // Display tokens
        cout << "TOKENS (" << tokens.size() << " tokens):\n------------------------------------------------------------------------\n";
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            const auto &token = tokens[i];
            cout << "[" << i << "] " << tokenToDisplay(token)
                 << " at line " << token.line << ", col " << token.col << "\n";
        }
        cout << "------------------------------------------------------------------------\n\n";
        // Parsing
        cout << "PHASE 2: PARSING\n========================================================================\n";
        Parser parser(tokens);
        Program program = parser.parseProgram();
        cout << "ABSTRACT SYNTAX TREE (AST):\n------------------------------------------------------------------------\n";
        program.print(cout);
        cout << "------------------------------------------------------------------------\n\n";
        // Type checking
        cout << "PHASE 3: TYPE CHECK ANALYSIS\n========================================================================\n";
        TypeChecker checker;
        checker.check(program);
        // Display results
        cout << "TYPE CHECK ANALYSIS RESULTS:\n------------------------------------------------------------------------\n";
        if (checker.errors.empty())
        {
            cout << "No type errors found!\n";
        }
        else
        {
            for (const auto &err : checker.errors)
            {
                cout << errorToString(err.type)
                     << " at line " << err.line << ", col " << err.col << " : " << err.detail << "\n";
            }
        }
    }
    catch (const exception &e)
    {
        cout << "ERROR: " << e.what() << "\n";
        return 1;
    }
    cout << "\n"
         << string(80, '=') << "\n";
    return 0;
}
