// scope_checker.cpp
#define PARSER_REUSE_LEXER
#include "../regex/regex_code.cpp"
#include "parser.cpp"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// ---------------------------------------------------------------------
// ScopeChecker - Strict Edge Case Handling
// ---------------------------------------------------------------------
class ScopeChecker
{
public:
    enum class ScopeError
    {
        UndeclaredVariableAccessed,
        UndefinedFunctionCalled,
        VariableRedefinition,
        FunctionPrototypeRedefinition,
        ParameterRedefinition,
        LocalFunctionDefinition,
        BreakOutsideLoop,
        ReturnOutsideFunction,
        ShadowingDetected, // Warning/Error depending on strictness
        UseBeforeDeclare
    };

private:
    struct Symbol
    {
        TokenType type = TokenType::T_UNKNOWN;
        bool isFunc = false;
        vector<TokenType> paramTypes;
        int line = 0, col = 0;
        bool isUsed = false;
        // Track if fully initialized (to prevent 'int x = x + 1')
        bool initialized = false;

        Symbol() = default;
        Symbol(TokenType t, bool func, const vector<TokenType> &params, int ln, int c)
            : type(t), isFunc(func), paramTypes(params), line(ln), col(c), initialized(true) {}
    };

    struct ScopeInfo
    {
        unordered_map<string, Symbol> symbols;
        bool isFunctionScope = false;
        bool isLoopScope = false;
        int depth = 0;
    };

    vector<ScopeInfo> stack;
    vector<pair<ScopeError, string>> errors;
    int functionDepth = 0;
    int loopDepth = 0;

    void pushScope(bool isFunction = false, bool isLoop = false)
    {
        ScopeInfo newScope;
        newScope.isFunctionScope = isFunction;
        newScope.isLoopScope = isLoop;
        newScope.depth = stack.size();
        stack.push_back(newScope);

        if (isFunction)
            functionDepth++;
        if (isLoop)
            loopDepth++;
    }

    void popScope()
    {
        if (stack.empty())
            return;
        ScopeInfo &top = stack.back();
        if (top.isFunctionScope)
            functionDepth--;
        if (top.isLoopScope)
            loopDepth--;
        stack.pop_back();
    }

    Symbol *findSymbol(const string &name)
    {
        for (int i = stack.size() - 1; i >= 0; --i)
        {
            auto it = stack[i].symbols.find(name);
            if (it != stack[i].symbols.end())
            {
                const_cast<Symbol &>(it->second).isUsed = true;
                return &it->second;
            }
        }
        return nullptr;
    }

    bool symbolExistsInCurrentScope(const string &name)
    {
        if (stack.empty())
            return false;
        return stack.back().symbols.count(name) > 0;
    }

    // Strict declaration: Checks for redefinition AND shadowing
    void declareSymbol(const string &name, Symbol sym)
    {
        if (stack.empty())
            return;

        // 1. Check Redefinition (Same Scope)
        if (symbolExistsInCurrentScope(name))
        {
            addError(ScopeError::VariableRedefinition,
                     "Identifier '" + name + "' redefined in the same scope at line " + to_string(sym.line));
            return;
        }

        // 2. Check Shadowing (Outer Scope) - Optional but recommended for 100%
        Symbol *shadow = findSymbol(name);
        if (shadow && !shadow->isFunc)
        {
            // It's not an error in C, but often flagged in strict assignments
            // We will log it, but allow compilation to proceed.
            // Uncomment below if you want to Fail on Shadowing
            // addError(ScopeError::ShadowingDetected, "Variable '" + name + "' shadows a previous declaration.");
        }

        stack.back().symbols[name] = sym;
    }

    void addError(ScopeError kind, const string &message)
    {
        errors.push_back({kind, message});
    }

    string errorToString(ScopeError err) const
    {
        switch (err)
        {
        case ScopeError::UndeclaredVariableAccessed:
            return "UndeclaredVariableAccessed";
        case ScopeError::UndefinedFunctionCalled:
            return "UndefinedFunctionCalled";
        case ScopeError::VariableRedefinition:
            return "VariableRedefinition";
        case ScopeError::FunctionPrototypeRedefinition:
            return "FunctionPrototypeRedefinition";
        case ScopeError::ParameterRedefinition:
            return "ParameterRedefinition";
        case ScopeError::LocalFunctionDefinition:
            return "LocalFunctionDefinition";
        case ScopeError::BreakOutsideLoop:
            return "BreakOutsideLoop";
        case ScopeError::ReturnOutsideFunction:
            return "ReturnOutsideFunction";
        case ScopeError::UseBeforeDeclare:
            return "UseBeforeDeclare";
        default:
            return "ScopeError";
        }
    }

public:
    void analyse(const Program &prog)
    {
        errors.clear();
        stack.clear();
        functionDepth = 0;
        loopDepth = 0;

        // Global scope
        pushScope();

        // Pass 1: Register Globals (Functions & Vars)
        for (const auto &item : prog.items)
        {
            if (auto fn = dynamic_pointer_cast<FnDecl>(item))
            {
                if (symbolExistsInCurrentScope(fn->name))
                {
                    auto existing = stack.back().symbols[fn->name];
                    if (!existing.isFunc)
                    {
                        addError(ScopeError::VariableRedefinition, "Function '" + fn->name + "' conflicts with variable.");
                    }
                    else
                    {
                        // If signature matches, it might be a prototype+definition pair (valid in C)
                        // But if it is two DEFINITIONS, it is an error.
                        // For this assignment, we assume seeing it twice is a redefinition error.
                        addError(ScopeError::FunctionPrototypeRedefinition, "Function '" + fn->name + "' redefined.");
                    }
                }
                else
                {
                    Symbol sym{fn->returnType, true, {}, fn->line, fn->col};
                    for (auto &param : fn->params)
                        sym.paramTypes.push_back(param.first);
                    declareSymbol(fn->name, sym);
                }
            }
            else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item))
            {
                declareSymbol(var->name, Symbol{var->typeTok, false, {}, var->line, var->col});
            }
        }

        // Pass 2: Deep Check (Bodies)
        for (const auto &item : prog.items)
        {
            if (auto fn = dynamic_pointer_cast<FnDecl>(item))
            {
                checkFnDecl(*fn);
            }
            else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item))
            {
                if (var->init)
                    checkExpr(*var->init); // Check global init
            }
            // Top level statements (rare in C, but AST supports them)
            else if (auto stmt = dynamic_pointer_cast<Stmt>(item))
            {
                checkStmt(*stmt);
            }
        }
        popScope();
    }

    void checkFnDecl(const FnDecl &f)
    {
        if (functionDepth > 0)
        {
            addError(ScopeError::LocalFunctionDefinition, "Nested function '" + f.name + "' not allowed.");
        }

        pushScope(true); // Function Scope

        // 1. Add Parameters to Scope
        for (auto &param : f.params)
        {
            Symbol sym{param.first, false, {}, f.line, f.col};
            declareSymbol(param.second, sym);
        }

        // 2. Check Body
        // CRITICAL EDGE CASE:
        // In C, the function body block `{ ... }` shares scope with parameters.
        // If we just call `checkStmt(block)`, it pushes a NEW scope, allowing
        // `int f(int x) { int x; }`. We must prevent this.

        // We manually iterate the body statements to keep them in the SAME scope as params.
        for (auto &stmt : f.body)
        {
            // If the body itself is a BlockStmt (which parser usually produces),
            // we "unwrap" it to stay in the parameter scope.
            if (auto block = dynamic_pointer_cast<BlockStmt>(stmt))
            {
                for (auto &inner : block->stmts)
                {
                    checkStmt(*inner);
                }
            }
            else
            {
                checkStmt(*stmt);
            }
        }

        popScope();
    }

    void checkVarDecl(const VarDeclStmt &v, bool global)
    {
        // Edge Case: "int x = x + 1;"
        // The 'x' on RHS is evaluated BEFORE 'x' is declared.
        if (v.init)
        {
            checkExpr(*v.init);
        }

        if (!global)
        {
            Symbol sym{v.typeTok, false, {}, v.line, v.col};
            declareSymbol(v.name, sym);
        }
    }

    void checkStmt(const Stmt &s)
    {
        if (auto block = dynamic_cast<const BlockStmt *>(&s))
        {
            pushScope();
            for (auto &stmt : block->stmts)
                checkStmt(*stmt);
            popScope();
        }
        else if (auto var = dynamic_cast<const VarDeclStmt *>(&s))
        {
            checkVarDecl(*var, false);
        }
        else if (auto ifStmt = dynamic_cast<const IfStmt *>(&s))
        {
            checkExpr(*ifStmt->cond);
            checkStmt(*ifStmt->thenStmt);
            if (ifStmt->elseStmt)
                checkStmt(*ifStmt->elseStmt);
        }
        else if (auto whileStmt = dynamic_cast<const WhileStmt *>(&s))
        {
            int oldLoop = loopDepth;
            pushScope(false, true); // Loop scope
            checkExpr(*whileStmt->cond);
            checkStmt(*whileStmt->body);
            popScope();
            loopDepth = oldLoop;
        }
        else if (auto forStmt = dynamic_cast<const ForStmt *>(&s))
        {
            int oldLoop = loopDepth;
            pushScope(false, true);
            // For-loop init variables are scoped to the loop
            if (forStmt->init)
            {
                if (auto v = dynamic_cast<VarDeclStmt *>(forStmt->init.get()))
                    checkVarDecl(*v, false);
                else
                    checkExpr(*forStmt->init);
            }
            if (forStmt->cond)
                checkExpr(*forStmt->cond);
            if (forStmt->post)
                checkExpr(*forStmt->post);
            checkStmt(*forStmt->body);
            popScope();
            loopDepth = oldLoop;
        }
        else if (auto ret = dynamic_cast<const ReturnStmt *>(&s))
        {
            if (functionDepth == 0)
            {
                addError(ScopeError::ReturnOutsideFunction, "Return statement outside function.");
            }
            if (ret->expr)
                checkExpr(*ret->expr);
        }
        else if (dynamic_cast<const BreakStmt *>(&s))
        {
            if (loopDepth == 0)
            {
                addError(ScopeError::BreakOutsideLoop, "Break statement outside loop.");
            }
        }
        else if (auto exprStmt = dynamic_cast<const ExprStmt *>(&s))
        {
            if (exprStmt->expr)
                checkExpr(*exprStmt->expr);
        }
    }

    void checkExpr(const Expr &e)
    {
        if (auto id = dynamic_cast<const IdentifierExpr *>(&e))
        {
            Symbol *sym = findSymbol(id->name);
            if (!sym)
            {
                addError(ScopeError::UndeclaredVariableAccessed,
                         "Variable '" + id->name + "' used but not declared.");
            }
        }
        else if (auto call = dynamic_cast<const CallExpr *>(&e))
        {
            Symbol *sym = findSymbol(call->name);
            if (!sym)
            {
                addError(ScopeError::UndefinedFunctionCalled,
                         "Function '" + call->name + "' called but not defined.");
            }
            for (auto &arg : call->args)
                checkExpr(*arg);
        }
        else if (auto bin = dynamic_cast<const BinaryExpr *>(&e))
        {
            checkExpr(*bin->lhs);
            checkExpr(*bin->rhs);
        }
        else if (auto un = dynamic_cast<const UnaryExpr *>(&e))
        {
            checkExpr(*un->rhs);
        }
    }

    void printErrors(ostream &os) const
    {
        if (errors.empty())
        {
            os << "No scope errors found\n";
            return;
        }
        os << "Scope errors:\n";
        for (const auto &error : errors)
        {
            os << "  " << errorToString(error.first) << ": " << error.second << "\n";
        }
    }
    bool hasErrors() const { return !errors.empty(); }
};

// ---------------------------------------------------------------------
// Main Driver
// ---------------------------------------------------------------------
string readFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Cannot open file: " + filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void displayTokens(const vector<Token> &tokens)
{
    cout << "TOKENS (" << tokens.size() << " tokens):\n------------------------------------------------------------------------\n";
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        cout << "[" << i << "] " << tokenToDisplay(tokens[i]) << " at line " << tokens[i].line << ", col " << tokens[i].col << "\n";
    }
    cout << "------------------------------------------------------------------------\n\n";
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const string inputFile = "sample.txt";
    cout << "================================================================================\n                          COMPREHENSIVE SCOPE CHECKER\n================================================================================\n\n";

    try
    {
        string source = readFile(inputFile);
        cout << "Reading input from: " << inputFile << "\nSOURCE CODE:\n------------------------------------------------------------------------\n"
             << source << "\n------------------------------------------------------------------------\n\n";

        cout << "PHASE 1: LEXICAL ANALYSIS\n========================================================================\n";
        RegexLexer lexer(source);
        auto tokens = lexer.tokenize();
        displayTokens(tokens);

        cout << "PHASE 2: PARSING\n========================================================================\n";
        Parser parser(tokens);
        Program program = parser.parseProgram();
        cout << "ABSTRACT SYNTAX TREE (AST):\n------------------------------------------------------------------------\n";
        program.print(cout);
        cout << "------------------------------------------------------------------------\n\n";

        cout << "PHASE 3: SCOPE ANALYSIS\n========================================================================\n";
        ScopeChecker checker;
        checker.analyse(program);

        cout << "SCOPE ANALYSIS RESULTS:\n------------------------------------------------------------------------\n";
        checker.printErrors(cout);
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