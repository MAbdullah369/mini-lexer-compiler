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
// ScopeChecker - Comprehensive C++ scope analysis
// ---------------------------------------------------------------------
class ScopeChecker {
public:
    enum class ScopeError {
        UndeclaredVariableAccessed,
        UndefinedFunctionCalled,
        VariableRedefinition,
        FunctionPrototypeRedefinition,
        ParameterRedefinition,
        FunctionRedeclarationWithDifferentSignature,
        LocalFunctionDefinition,
        BreakContinueOutsideLoop,
        ReturnOutsideFunction,
        InvalidScopeExit
    };

private:
    struct Symbol {
        TokenType type = TokenType::T_UNKNOWN;
        bool isFunc = false;
        vector<TokenType> paramTypes;
        int line = 0, col = 0;
        bool isUsed = false;
        
        Symbol() = default;
        Symbol(TokenType t, bool func, const vector<TokenType>& params, int ln, int c) 
            : type(t), isFunc(func), paramTypes(params), line(ln), col(c) {}
    };

    struct ScopeInfo {
        unordered_map<string, Symbol> symbols;
        bool isFunctionScope = false;
        bool isLoopScope = false;
        int depth = 0;
    };

    vector<ScopeInfo> stack; // spaghetti stack
    vector<pair<ScopeError, string>> errors;
    int functionDepth = 0;
    int loopDepth = 0;

    void pushScope(bool isFunction = false, bool isLoop = false) {
        ScopeInfo newScope;
        newScope.isFunctionScope = isFunction;
        newScope.isLoopScope = isLoop;
        newScope.depth = stack.size();
        stack.push_back(newScope);
        
        if (isFunction) functionDepth++;
        if (isLoop) loopDepth++;
    }
    
    void popScope() {
        if (stack.empty()) return;
        
        ScopeInfo& top = stack.back();
        
        // Check for unused variables in this scope
        for (const auto& [name, symbol] : top.symbols) {
            if (!symbol.isFunc && !symbol.isUsed) {
                // Don't warn about function parameters or intentionally unused variables
                if (!(top.isFunctionScope && symbol.line == 0)) { // Parameters have line 0
                    // Could add unused variable warning here if desired
                }
            }
        }
        
        if (top.isFunctionScope) functionDepth--;
        if (top.isLoopScope) loopDepth--;
        
        stack.pop_back();
    }

    Symbol* findSymbol(const string& name) {
        // Search from innermost to outermost scope
        for (int i = stack.size() - 1; i >= 0; --i) {
            auto it = stack[i].symbols.find(name);
            if (it != stack[i].symbols.end()) {
                // Mark as used
                const_cast<Symbol&>(it->second).isUsed = true;
                return &it->second;
            }
        }
        return nullptr;
    }

    bool symbolExistsInCurrentScope(const string& name) {
        if (stack.empty()) return false;
        return stack.back().symbols.count(name) > 0;
    }

    bool symbolExistsInAnyScope(const string& name) {
        return findSymbol(name) != nullptr;
    }

    void addSymbol(const string& name, const Symbol& sym) {
        if (stack.empty()) return;
        
        auto& currentScope = stack.back();
        if (currentScope.symbols.count(name)) {
            addError(ScopeError::VariableRedefinition, 
                    "Identifier '" + name + "' redefined in the same scope");
        } else {
            currentScope.symbols[name] = sym;
        }
    }

    void addError(ScopeError kind, const string& message) {
        errors.push_back({kind, message});
    }

    // Mark as const for use in const methods
    string errorToString(ScopeError err) const {
        switch (err) {
            case ScopeError::UndeclaredVariableAccessed: return "UndeclaredVariableAccessed";
            case ScopeError::UndefinedFunctionCalled: return "UndefinedFunctionCalled";
            case ScopeError::VariableRedefinition: return "VariableRedefinition";
            case ScopeError::FunctionPrototypeRedefinition: return "FunctionPrototypeRedefinition";
            case ScopeError::ParameterRedefinition: return "ParameterRedefinition";
            case ScopeError::FunctionRedeclarationWithDifferentSignature: return "FunctionRedeclarationWithDifferentSignature";
            case ScopeError::LocalFunctionDefinition: return "LocalFunctionDefinition";
            case ScopeError::BreakContinueOutsideLoop: return "BreakContinueOutsideLoop";
            case ScopeError::ReturnOutsideFunction: return "ReturnOutsideFunction";
            case ScopeError::InvalidScopeExit: return "InvalidScopeExit";
            default: return "UnknownError";
        }
    }

    void checkExpr(const Expr& e);
    void checkStmt(const Stmt& s);
    void checkVarDecl(const VarDeclStmt& v, bool global);
    void checkFnDecl(const FnDecl& f);
    void checkBreakContinue(const string& stmtType);

public:
    void analyse(const Program& prog) {
        errors.clear();
        stack.clear();
        functionDepth = 0;
        loopDepth = 0;

        // Global scope
        pushScope();

        // Pass 1: Collect global symbols (functions and variables)
        for (const auto& item : prog.items) {
            if (auto fn = dynamic_pointer_cast<FnDecl>(item)) {
                const string& name = fn->name;
                if (symbolExistsInCurrentScope(name)) {
                    auto existing = stack.back().symbols[name];
                    if (existing.isFunc) {
                        // Check if function signature matches
                        if (existing.paramTypes.size() != fn->params.size()) {
                            addError(ScopeError::FunctionRedeclarationWithDifferentSignature,
                                    "Function '" + name + "' redeclared with different signature");
                        } else {
                            bool signatureMatch = true;
                            for (size_t i = 0; i < fn->params.size(); ++i) {
                                if (existing.paramTypes[i] != fn->params[i].first) {
                                    signatureMatch = false;
                                    break;
                                }
                            }
                            if (!signatureMatch) {
                                addError(ScopeError::FunctionRedeclarationWithDifferentSignature,
                                        "Function '" + name + "' redeclared with different parameter types");
                            } else {
                                addError(ScopeError::FunctionPrototypeRedefinition,
                                        "Function '" + name + "' redefined");
                            }
                        }
                    } else {
                        addError(ScopeError::FunctionPrototypeRedefinition,
                                "Function '" + name + "' conflicts with variable");
                    }
                } else {
                    Symbol sym{fn->returnType, true, {}, 0, 0};
                    for (auto& param : fn->params) {
                        sym.paramTypes.push_back(param.first);
                    }
                    addSymbol(name, sym);
                }
            }
            else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item)) {
                const string& name = var->name;
                if (symbolExistsInCurrentScope(name)) {
                    addError(ScopeError::VariableRedefinition,
                            "Global variable '" + name + "' redefined");
                } else {
                    addSymbol(name, Symbol{var->typeTok, false, {}, 0, 0});
                }
            }
        }

        // Pass 2: Full semantic analysis with scope tracking
        for (const auto& item : prog.items) {
            try {
                if (auto fn = dynamic_pointer_cast<FnDecl>(item)) {
                    checkFnDecl(*fn);
                }
                else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item)) {
                    // Global variables are already handled in pass 1
                    if (var->init) {
                        checkExpr(*var->init);
                    }
                }
                else if (auto stmt = dynamic_pointer_cast<Stmt>(item)) {
                    checkStmt(*stmt);
                }
            } catch (const exception& e) {
                addError(ScopeError::UndeclaredVariableAccessed, 
                        "Analysis error: " + string(e.what()));
            }
        }

        popScope(); // Remove global scope
    }

    void printErrors(ostream& os) const {
        if (errors.empty()) {
            os << "No scope errors found\n";
            return;
        }
        
        os << "Scope errors:\n";
        for (const auto& error : errors) {
            os << "  " << errorToString(error.first) << ": " << error.second << "\n";
        }
    }
    
    bool hasErrors() const { return !errors.empty(); }
};

// ---------------------------------------------------------------------
// Expression checking
// ---------------------------------------------------------------------
void ScopeChecker::checkExpr(const Expr& e) {
    if (auto id = dynamic_cast<const IdentifierExpr*>(&e)) {
        if (!findSymbol(id->name)) {
            addError(ScopeError::UndeclaredVariableAccessed,
                    "Variable '" + id->name + "' used but not declared");
        }
    }
    else if (auto bin = dynamic_cast<const BinaryExpr*>(&e)) {
        checkExpr(*bin->lhs);
        checkExpr(*bin->rhs);
    }
    else if (auto un = dynamic_cast<const UnaryExpr*>(&e)) {
        checkExpr(*un->rhs);
    }
    else if (auto post = dynamic_cast<const PostfixExpr*>(&e)) {
        checkExpr(*post->expr);
    }
    else if (auto call = dynamic_cast<const CallExpr*>(&e)) {
        auto sym = findSymbol(call->name);
        if (!sym || !sym->isFunc) {
            addError(ScopeError::UndefinedFunctionCalled,
                    "Function '" + call->name + "' called but not defined");
        }
        for (auto& arg : call->args) {
            checkExpr(*arg);
        }
    }
    else if (auto idx = dynamic_cast<const IndexExpr*>(&e)) {
        checkExpr(*idx->base);
        checkExpr(*idx->index);
    }
    // Literals don't need scope checking
}

// ---------------------------------------------------------------------
// Statement checking
// ---------------------------------------------------------------------
void ScopeChecker::checkBreakContinue(const string& stmtType) {
    if (loopDepth == 0) {
        addError(ScopeError::BreakContinueOutsideLoop,
                stmtType + " statement outside of loop");
    }
}

void ScopeChecker::checkStmt(const Stmt& s) {
    if (auto block = dynamic_cast<const BlockStmt*>(&s)) {
        pushScope();
        for (auto& stmt : block->stmts) {
            checkStmt(*stmt);
        }
        popScope();
    }
    else if (auto varDecl = dynamic_cast<const VarDeclStmt*>(&s)) {
        checkVarDecl(*varDecl, false);
    }
    else if (auto exprStmt = dynamic_cast<const ExprStmt*>(&s)) {
        if (exprStmt->expr) checkExpr(*exprStmt->expr);
    }
    else if (auto ret = dynamic_cast<const ReturnStmt*>(&s)) {
        if (functionDepth == 0) {
            addError(ScopeError::ReturnOutsideFunction,
                    "return statement outside of function");
        }
        if (ret->expr) checkExpr(*ret->expr);
    }
    else if (auto ifStmt = dynamic_cast<const IfStmt*>(&s)) {
        checkExpr(*ifStmt->cond);
        checkStmt(*ifStmt->thenStmt);
        if (ifStmt->elseStmt) checkStmt(*ifStmt->elseStmt);
    }
    else if (auto whileStmt = dynamic_cast<const WhileStmt*>(&s)) {
        int oldLoopDepth = loopDepth;
        pushScope(false, true);
        checkExpr(*whileStmt->cond);
        checkStmt(*whileStmt->body);
        popScope();
        loopDepth = oldLoopDepth;
    }
    else if (auto doWhile = dynamic_cast<const DoWhileStmt*>(&s)) {
        int oldLoopDepth = loopDepth;
        pushScope(false, true);
        checkStmt(*doWhile->body);
        checkExpr(*doWhile->cond);
        popScope();
        loopDepth = oldLoopDepth;
    }
    else if (auto forStmt = dynamic_cast<const ForStmt*>(&s)) {
        int oldLoopDepth = loopDepth;
        pushScope(false, true);
        if (forStmt->init) {
            if (auto decl = dynamic_cast<VarDeclStmt*>(forStmt->init.get())) {
                checkVarDecl(*decl, false);
            } else {
                checkExpr(*forStmt->init);
            }
        }
        if (forStmt->cond) checkExpr(*forStmt->cond);
        if (forStmt->post) checkExpr(*forStmt->post);
        checkStmt(*forStmt->body);
        popScope();
        loopDepth = oldLoopDepth;
    }
    // Handle the case where we have multiple statements without explicit block
    // This happens when the parser creates separate BlockStmt for each statement
    else {
        // For other statement types, just process them in current scope
        // This handles the case where multiple statements are at the same level
    }
}

// ---------------------------------------------------------------------
// Variable declaration checking
// ---------------------------------------------------------------------
void ScopeChecker::checkVarDecl(const VarDeclStmt& v, bool global) {
    if (!global) {
        // Check if variable is already declared in current scope
        if (symbolExistsInCurrentScope(v.name)) {
            addError(ScopeError::VariableRedefinition,
                    "Variable '" + v.name + "' redefined in the same scope");
        } else {
            Symbol sym{v.typeTok, false, {}, 0, 0};
            addSymbol(v.name, sym);
        }
    }
    if (v.init) {
        checkExpr(*v.init);
    }
}

// ---------------------------------------------------------------------
// Function declaration checking
// ---------------------------------------------------------------------
void ScopeChecker::checkFnDecl(const FnDecl& f) {
    // Check for nested function definitions (not allowed in C++)
    if (functionDepth > 0) {
        addError(ScopeError::LocalFunctionDefinition,
                "Nested function definition '" + f.name + "' not allowed");
    }

    pushScope(true); // Function scope
    
    // Add parameters to function scope
    for (auto& param : f.params) {
        if (symbolExistsInCurrentScope(param.second)) {
            addError(ScopeError::ParameterRedefinition,
                    "Parameter '" + param.second + "' redefined in function '" + f.name + "'");
        } else {
            Symbol sym{param.first, false, {}, 0, 0};
            addSymbol(param.second, sym);
        }
    }
    
    // Check function body - handle both single statements and blocks
    for (auto& stmt : f.body) {
        checkStmt(*stmt);
    }
    
    popScope(); // Exit function scope
}

// ---------------------------------------------------------------------
// Function to read file content
// ---------------------------------------------------------------------
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ---------------------------------------------------------------------
// Function to display tokens
// ---------------------------------------------------------------------
void displayTokens(const vector<Token>& tokens) {
    cout << "TOKENS (" << tokens.size() << " tokens):\n";
    cout << "------------------------------------------------------------------------\n";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        cout << "[" << i << "] " << tokenToDisplay(token) 
             << " at line " << token.line << ", col " << token.col << "\n";
    }
    cout << "------------------------------------------------------------------------\n\n";
}

// ---------------------------------------------------------------------
// Test harness - reads from sample.txt
// ---------------------------------------------------------------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string inputFile = "sample.txt";
    
    cout << "================================================================================\n";
    cout << "                          COMPREHENSIVE SCOPE CHECKER\n";
    cout << "================================================================================\n\n";

    try {
        // Read input from file
        string source = readFile(inputFile);
        
        cout << "Reading input from: " << inputFile << "\n";
        cout << "SOURCE CODE:\n";
        cout << "------------------------------------------------------------------------\n";
        cout << source << "\n";
        cout << "------------------------------------------------------------------------\n\n";

        // Lexical analysis
        cout << "PHASE 1: LEXICAL ANALYSIS\n";
        cout << "========================================================================\n";
        RegexLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        // Display tokens
        displayTokens(tokens);

        // Parsing
        cout << "PHASE 2: PARSING\n";
        cout << "========================================================================\n";
        Parser parser(tokens);
        Program program = parser.parseProgram();

        cout << "ABSTRACT SYNTAX TREE (AST):\n";
        cout << "------------------------------------------------------------------------\n";
        program.print(cout);
        cout << "------------------------------------------------------------------------\n\n";

        // Scope analysis
        cout << "PHASE 3: SCOPE ANALYSIS\n";
        cout << "========================================================================\n";
        ScopeChecker checker;
        checker.analyse(program);

        // Display results
        cout << "SCOPE ANALYSIS RESULTS:\n";
        cout << "------------------------------------------------------------------------\n";
        checker.printErrors(cout);
        
    } catch (const exception& e) {
        cout << "ERROR: " << e.what() << "\n";
        return 1;
    }

    cout << "\n" << string(80, '=') << "\n";
    return 0;
}