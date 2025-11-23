// irGenerator.cpp - Single file IR Generator
#define PARSER_REUSE_LEXER
#include "../regex/regex_code.cpp"
#include "parser.cpp"
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <fstream>

using namespace std;

// IR Instruction Types
enum class IROp {
    ASSIGN, ADD, SUB, MUL, DIV, 
    EQ, NE, LT, LE, GT, GE,
    LAND, LOR, LNOT,
    JUMP, JUMP_TRUE, JUMP_FALSE,
    CALL, RET, PARAM,
    LABEL
};

struct IRInstruction {
    IROp op;
    string result;
    string arg1;
    string arg2;
    int line;
    
    IRInstruction(IROp o, const string& res, const string& a1 = "", 
                  const string& a2 = "", int ln = 0)
        : op(o), result(res), arg1(a1), arg2(a2), line(ln) {}
};

class IRGenerator {
private:
    vector<IRInstruction> instructions;
    int tempCounter = 0;
    int labelCounter = 0;
    vector<string> loopEndLabels;
    
    // Symbol table for variables
    unordered_map<string, string> symbolTable;

public:
    vector<IRInstruction> generateIR(const Program& ast) {
        instructions.clear();
        tempCounter = 0;
        labelCounter = 0;
        symbolTable.clear();
        loopEndLabels.clear();
        
        visitProgram(ast);
        return instructions;
    }
    
    void printIR(ostream& os) const {
        os << "=== THREE-ADDRESS CODE IR ===" << endl;
        for (const auto& instr : instructions) {
            os << formatInstruction(instr) << endl;
        }
    }

private:
    string newTemp() {
        return "t" + to_string(tempCounter++);
    }
    
    string newLabel() {
        return "L" + to_string(labelCounter++);
    }
    
    void emit(IROp op, const string& result = "", const string& arg1 = "", 
              const string& arg2 = "", int line = 0) {
        instructions.emplace_back(op, result, arg1, arg2, line);
    }
    
    string formatInstruction(const IRInstruction& instr) const {
        switch (instr.op) {
            case IROp::ASSIGN:
                return instr.result + " = " + instr.arg1;
            case IROp::ADD:
                return instr.result + " = " + instr.arg1 + " + " + instr.arg2;
            case IROp::SUB:
                return instr.result + " = " + instr.arg1 + " - " + instr.arg2;
            case IROp::MUL:
                return instr.result + " = " + instr.arg1 + " * " + instr.arg2;
            case IROp::DIV:
                return instr.result + " = " + instr.arg1 + " / " + instr.arg2;
            case IROp::EQ:
                return instr.result + " = " + instr.arg1 + " == " + instr.arg2;
            case IROp::NE:
                return instr.result + " = " + instr.arg1 + " != " + instr.arg2;
            case IROp::LT:
                return instr.result + " = " + instr.arg1 + " < " + instr.arg2;
            case IROp::LE:
                return instr.result + " = " + instr.arg1 + " <= " + instr.arg2;
            case IROp::GT:
                return instr.result + " = " + instr.arg1 + " > " + instr.arg2;
            case IROp::GE:
                return instr.result + " = " + instr.arg1 + " >= " + instr.arg2;
            case IROp::LAND:
                return instr.result + " = " + instr.arg1 + " && " + instr.arg2;
            case IROp::LOR:
                return instr.result + " = " + instr.arg1 + " || " + instr.arg2;
            case IROp::LNOT:
                return instr.result + " = !" + instr.arg1;
            case IROp::JUMP:
                return "goto " + instr.result;
            case IROp::JUMP_TRUE:
                return "if " + instr.arg1 + " goto " + instr.result;
            case IROp::JUMP_FALSE:
                return "ifFalse " + instr.arg1 + " goto " + instr.result;
            case IROp::CALL:
                return instr.result + " = call " + instr.arg1;
            case IROp::RET:
                if (instr.result.empty()) 
                    return "return";
                else
                    return "return " + instr.result;
            case IROp::PARAM:
                return "param " + instr.result;
            case IROp::LABEL:
                return instr.result + ":";
            default:
                return "unknown";
        }
    }
    
    // Visitor methods
    void visitProgram(const Program& prog) {
        // First pass: collect global symbols
        for (const auto& item : prog.items) {
            if (auto var = dynamic_pointer_cast<VarDeclStmt>(item)) {
                symbolTable[var->name] = typeKeywordToString(var->typeTok);
            }
        }
        
        // Second pass: generate IR
        for (const auto& item : prog.items) {
            if (auto fn = dynamic_pointer_cast<FnDecl>(item)) {
                visitFunction(*fn);
            } else if (auto var = dynamic_pointer_cast<VarDeclStmt>(item)) {
                visitGlobalVar(*var);
            }
        }
    }
    
    void visitFunction(const FnDecl& fn) {
        // Function header
        emit(IROp::LABEL, "func_" + fn.name, "", "", fn.line);
        
        // Parameters
        for (const auto& param : fn.params) {
            symbolTable[param.second] = typeKeywordToString(param.first);
            emit(IROp::PARAM, param.second, "", "", fn.line);
        }
        
        // Function body
        for (const auto& stmt : fn.body) {
            if (auto block = dynamic_pointer_cast<BlockStmt>(stmt)) {
                for (const auto& s : block->stmts) {
                    visitStatement(s);
                }
            } else {
                visitStatement(stmt);
            }
        }
        
        // Implicit return if needed
        if (fn.returnType != TokenType::T_UNKNOWN && instructions.back().op != IROp::RET) {
            string retVal = "0"; // default return value
            if (fn.returnType == TokenType::T_BOOL) retVal = "false";
            else if (fn.returnType == TokenType::T_FLOAT) retVal = "0.0";
            else if (fn.returnType == TokenType::T_STRING) retVal = "\"\"";
            emit(IROp::RET, retVal, "", "", fn.line);
        }
    }
    
    void visitGlobalVar(const VarDeclStmt& var) {
        if (var.init) {
            string initVal = visitExpression(var.init);
            emit(IROp::ASSIGN, var.name, initVal, "", var.line);
        }
    }
    
    void visitStatement(const shared_ptr<Stmt>& stmt) {
        if (!stmt) return;
        
        if (auto exprStmt = dynamic_pointer_cast<ExprStmt>(stmt)) {
            if (exprStmt->expr) {
                visitExpression(exprStmt->expr);
            }
        }
        else if (auto varDecl = dynamic_pointer_cast<VarDeclStmt>(stmt)) {
            visitLocalVar(*varDecl);
        }
        else if (auto retStmt = dynamic_pointer_cast<ReturnStmt>(stmt)) {
            string retVal = retStmt->expr ? visitExpression(retStmt->expr) : "";
            emit(IROp::RET, retVal, "", "", retStmt->line);
        }
        else if (auto ifStmt = dynamic_pointer_cast<IfStmt>(stmt)) {
            visitIfStatement(*ifStmt);
        }
        else if (auto whileStmt = dynamic_pointer_cast<WhileStmt>(stmt)) {
            visitWhileStatement(*whileStmt);
        }
        else if (auto forStmt = dynamic_pointer_cast<ForStmt>(stmt)) {
            visitForStatement(*forStmt);
        }
        else if (auto block = dynamic_pointer_cast<BlockStmt>(stmt)) {
            for (const auto& s : block->stmts) {
                visitStatement(s);
            }
        }
        else if (auto breakStmt = dynamic_pointer_cast<BreakStmt>(stmt)) {
            if (!loopEndLabels.empty()) {
                emit(IROp::JUMP, loopEndLabels.back(), "", "", breakStmt->line);
            }
        }
        else if (auto emptyStmt = dynamic_pointer_cast<EmptyStmt>(stmt)) {
            // Do nothing for empty statements
        }
    }
    
    void visitLocalVar(const VarDeclStmt& var) {
        symbolTable[var.name] = typeKeywordToString(var.typeTok);
        if (var.init) {
            string initVal = visitExpression(var.init);
            emit(IROp::ASSIGN, var.name, initVal, "", var.line);
        }
    }
    
    void visitIfStatement(const IfStmt& ifStmt) {
        string cond = visitExpression(ifStmt.cond);
        string trueLabel = newLabel();
        string falseLabel = newLabel();
        string endLabel = newLabel();
        
        emit(IROp::JUMP_FALSE, falseLabel, cond, "", ifStmt.line);
        emit(IROp::JUMP, trueLabel, "", "", ifStmt.line);
        
        // True branch
        emit(IROp::LABEL, trueLabel, "", "", ifStmt.line);
        visitStatement(ifStmt.thenStmt);
        
        if (ifStmt.elseStmt) {
            emit(IROp::JUMP, endLabel, "", "", ifStmt.line);
        }
        
        // False branch
        emit(IROp::LABEL, falseLabel, "", "", ifStmt.line);
        if (ifStmt.elseStmt) {
            visitStatement(ifStmt.elseStmt);
            emit(IROp::LABEL, endLabel, "", "", ifStmt.line);
        }
    }
    
    void visitWhileStatement(const WhileStmt& whileStmt) {
        string startLabel = newLabel();
        string bodyLabel = newLabel();
        string endLabel = newLabel();
        
        loopEndLabels.push_back(endLabel);
        
        emit(IROp::LABEL, startLabel, "", "", whileStmt.line);
        string cond = visitExpression(whileStmt.cond);
        emit(IROp::JUMP_FALSE, endLabel, cond, "", whileStmt.line);
        emit(IROp::JUMP, bodyLabel, "", "", whileStmt.line);
        
        emit(IROp::LABEL, bodyLabel, "", "", whileStmt.line);
        visitStatement(whileStmt.body);
        emit(IROp::JUMP, startLabel, "", "", whileStmt.line);
        
        emit(IROp::LABEL, endLabel, "", "", whileStmt.line);
        loopEndLabels.pop_back();
    }
    
    void visitForStatement(const ForStmt& forStmt) {
        string startLabel = newLabel();
        string bodyLabel = newLabel();
        string postLabel = newLabel();
        string endLabel = newLabel();
        
        loopEndLabels.push_back(endLabel);
        
        // Initialization
        if (forStmt.init) {
            if (auto varDecl = dynamic_pointer_cast<VarDeclStmt>(forStmt.init)) {
                visitLocalVar(*varDecl);
            } else {
                visitExpression(forStmt.init);
            }
        }
        
        emit(IROp::LABEL, startLabel, "", "", forStmt.line);
        
        // Condition
        if (forStmt.cond) {
            string cond = visitExpression(forStmt.cond);
            emit(IROp::JUMP_FALSE, endLabel, cond, "", forStmt.line);
        }
        
        emit(IROp::JUMP, bodyLabel, "", "", forStmt.line);
        
        // Body
        emit(IROp::LABEL, bodyLabel, "", "", forStmt.line);
        visitStatement(forStmt.body);
        
        // Post iteration
        emit(IROp::JUMP, postLabel, "", "", forStmt.line);
        emit(IROp::LABEL, postLabel, "", "", forStmt.line);
        if (forStmt.post) {
            visitExpression(forStmt.post);
        }
        
        emit(IROp::JUMP, startLabel, "", "", forStmt.line);
        emit(IROp::LABEL, endLabel, "", "", forStmt.line);
        loopEndLabels.pop_back();
    }
    
    string visitExpression(const shared_ptr<Expr>& expr) {
        if (!expr) return "";
        
        if (auto intLit = dynamic_pointer_cast<IntLiteral>(expr)) {
            string temp = newTemp();
            emit(IROp::ASSIGN, temp, intLit->val, "", expr->line);
            return temp;
        }
        else if (auto floatLit = dynamic_pointer_cast<FloatLiteral>(expr)) {
            string temp = newTemp();
            emit(IROp::ASSIGN, temp, floatLit->val, "", expr->line);
            return temp;
        }
        else if (auto stringLit = dynamic_pointer_cast<StringLiteral>(expr)) {
            string temp = newTemp();
            emit(IROp::ASSIGN, temp, "\"" + stringLit->val + "\"", "", expr->line);
            return temp;
        }
        else if (auto boolLit = dynamic_pointer_cast<BoolLiteral>(expr)) {
            string temp = newTemp();
            emit(IROp::ASSIGN, temp, boolLit->val ? "true" : "false", "", expr->line);
            return temp;
        }
        else if (auto charLit = dynamic_pointer_cast<CharLiteral>(expr)) {
            string temp = newTemp();
            emit(IROp::ASSIGN, temp, "'" + charLit->val + "'", "", expr->line);
            return temp;
        }
        else if (auto ident = dynamic_pointer_cast<IdentifierExpr>(expr)) {
            return ident->name; // Just use the variable name
        }
        else if (auto unary = dynamic_pointer_cast<UnaryExpr>(expr)) {
            string rhs = visitExpression(unary->rhs);
            string temp = newTemp();
            
            if (unary->op == "!") {
                emit(IROp::LNOT, temp, rhs, "", expr->line);
            } else if (unary->op == "-") {
                emit(IROp::SUB, temp, "0", rhs, expr->line);
            } else if (unary->op == "++") {
                // Prefix increment
                emit(IROp::ADD, temp, rhs, "1", expr->line);
                emit(IROp::ASSIGN, rhs, temp, "", expr->line);
            } else if (unary->op == "--") {
                // Prefix decrement
                emit(IROp::SUB, temp, rhs, "1", expr->line);
                emit(IROp::ASSIGN, rhs, temp, "", expr->line);
            } else {
                emit(IROp::ASSIGN, temp, rhs, "", expr->line);
            }
            return temp;
        }
        else if (auto postfix = dynamic_pointer_cast<PostfixExpr>(expr)) {
            string exprVal = visitExpression(postfix->expr);
            string temp = newTemp();
            
            // Store original value
            emit(IROp::ASSIGN, temp, exprVal, "", expr->line);
            
            if (postfix->op == "++") {
                string newVal = newTemp();
                emit(IROp::ADD, newVal, exprVal, "1", expr->line);
                emit(IROp::ASSIGN, exprVal, newVal, "", expr->line);
            } else if (postfix->op == "--") {
                string newVal = newTemp();
                emit(IROp::SUB, newVal, exprVal, "1", expr->line);
                emit(IROp::ASSIGN, exprVal, newVal, "", expr->line);
            }
            return temp;
        }
        else if (auto binary = dynamic_pointer_cast<BinaryExpr>(expr)) {
            string lhs = visitExpression(binary->lhs);
            string rhs = visitExpression(binary->rhs);
            string temp = newTemp();
            
            if (binary->op == "+") {
                emit(IROp::ADD, temp, lhs, rhs, expr->line);
            } else if (binary->op == "-") {
                emit(IROp::SUB, temp, lhs, rhs, expr->line);
            } else if (binary->op == "*") {
                emit(IROp::MUL, temp, lhs, rhs, expr->line);
            } else if (binary->op == "/") {
                emit(IROp::DIV, temp, lhs, rhs, expr->line);
            } else if (binary->op == "==") {
                emit(IROp::EQ, temp, lhs, rhs, expr->line);
            } else if (binary->op == "!=") {
                emit(IROp::NE, temp, lhs, rhs, expr->line);
            } else if (binary->op == "<") {
                emit(IROp::LT, temp, lhs, rhs, expr->line);
            } else if (binary->op == "<=") {
                emit(IROp::LE, temp, lhs, rhs, expr->line);
            } else if (binary->op == ">") {
                emit(IROp::GT, temp, lhs, rhs, expr->line);
            } else if (binary->op == ">=") {
                emit(IROp::GE, temp, lhs, rhs, expr->line);
            } else if (binary->op == "&&") {
                emit(IROp::LAND, temp, lhs, rhs, expr->line);
            } else if (binary->op == "||") {
                emit(IROp::LOR, temp, lhs, rhs, expr->line);
            } else if (binary->op == "=") {
                emit(IROp::ASSIGN, lhs, rhs, "", expr->line);
                return lhs; // Assignment returns the assigned value
            } else if (binary->op == "+=") {
                string sum = newTemp();
                emit(IROp::ADD, sum, lhs, rhs, expr->line);
                emit(IROp::ASSIGN, lhs, sum, "", expr->line);
                return lhs;
            } else if (binary->op == "-=") {
                string diff = newTemp();
                emit(IROp::SUB, diff, lhs, rhs, expr->line);
                emit(IROp::ASSIGN, lhs, diff, "", expr->line);
                return lhs;
            }
            return temp;
        }
        else if (auto call = dynamic_pointer_cast<CallExpr>(expr)) {
            // Push parameters
            for (const auto& arg : call->args) {
                string argVal = visitExpression(arg);
                emit(IROp::PARAM, argVal, "", "", expr->line);
            }
            
            string temp = newTemp();
            emit(IROp::CALL, temp, call->name, "", expr->line);
            return temp;
        }
        else if (auto index = dynamic_pointer_cast<IndexExpr>(expr)) {
            string base = visitExpression(index->base);
            string idx = visitExpression(index->index);
            string temp = newTemp();
            // For arrays: base[idx] - simplified as base + index for now
            emit(IROp::ADD, temp, base, idx, expr->line);
            return temp;
        }
        
        return "";
    }
};

// Main driver that integrates with your existing pipeline
int main() {
    const string inputFile = "sample.txt";
    
    try {
        // Read source file
        ifstream file(inputFile);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file: " + inputFile);
        }
        stringstream buffer;
        buffer << file.rdbuf();
        string source = buffer.str();

        cout << "=== SOURCE CODE ===" << endl;
        cout << source << endl << endl;

        // Lexical Analysis
        cout << "=== LEXICAL ANALYSIS ===" << endl;
        RegexLexer lexer(source);
        auto tokens = lexer.tokenize();
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            if (token.type == TokenType::T_EOF) break;
            cout << "[" << i << "] " << tokenToDisplay(token) 
                 << " at line " << token.line << ", col " << token.col << endl;
        }
        cout << endl;

        // Parsing
        cout << "=== PARSING ===" << endl;
        Parser parser(tokens);
        Program program = parser.parseProgram();
        program.print(cout);
        cout << endl;

        // IR Generation
        cout << "=== IR GENERATION ===" << endl;
        IRGenerator irGen;
        auto irCode = irGen.generateIR(program);
        irGen.printIR(cout);

    } catch (const exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}