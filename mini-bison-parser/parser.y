%{
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class ASTNode {
public:
    std::string type;
    std::string value;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    ASTNode(const std::string& t, const std::string& v = "") : type(t), value(v) {}
    
    void addChild(std::unique_ptr<ASTNode> child) {
        children.push_back(std::move(child));
    }
    
    void print(int indent = 0) const {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << type;
        if (!value.empty()) std::cout << " [" << value << "]";
        std::cout << std::endl;
        
        for (const auto& child : children) {
            child->print(indent + 1);
        }
    }
};

class SymbolTable {
private:
    std::vector<std::string> symbols;
    
public:
    bool isDeclared(const std::string& identifier) {
        for (const auto& var : symbols) {
            if (var == identifier) return true;
        }
        return false;
    }
    
    void declareVariable(const std::string& identifier) {
        if (!isDeclared(identifier)) {
            symbols.push_back(identifier);
        }
    }
    
    void addBuiltins() {
        symbols.insert(symbols.end(), {
            "cout", "cin", "endl", "main", "printf", "scanf", 
            "malloc", "free", "true", "false", "NULL", "nullptr",
            "add", "factorial", "printMessage", "calculate"
        });
    }
};

extern int yylex();
extern void yyerror(const char* s);
extern FILE* yyin;

static std::unique_ptr<ASTNode> root;
static SymbolTable symbolTable;
%}

%union {
    char* string_val;
    int int_val;
    double double_val;
    void* node_val;
}

%token <string_val> IDENTIFIER STRING_LITERAL CHAR_LITERAL
%token <int_val> INTEGER 
%token <double_val> FLOAT
%token INT FLOAT_TYPE DOUBLE CHAR BOOL VOID STRING
%token IF ELSE WHILE FOR RETURN BREAK CONTINUE
%token COUT CIN ENDL
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token ASSIGN EQ NE LT LE GT GE
%token AND OR NOT
%token INCREMENT DECREMENT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COMMA COLON
%token OUTPUT INPUT
%token TRUE FALSE

%type <node_val> program translation_unit external_declaration
%type <node_val> function_definition declaration
%type <node_val> init_declarator_list init_declarator
%type <node_val> type parameter_list parameter_declaration
%type <node_val> compound_statement statement_list statement
%type <node_val> selection_statement iteration_statement jump_statement
%type <node_val> expression assignment_expression logical_or_expression
%type <node_val> logical_and_expression equality_expression relational_expression
%type <node_val> additive_expression multiplicative_expression
%type <node_val> unary_expression postfix_expression primary_expression
%type <node_val> argument_expression_list

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
%right INCREMENT DECREMENT
%right ASSIGN
%left OUTPUT INPUT

%start program

%%

program:
    translation_unit
    {
        root = std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1));
    }
    ;

translation_unit:
    external_declaration
    {
        $$ = $1;
    }
    | translation_unit external_declaration
    {
        ASTNode* left = static_cast<ASTNode*>($1);
        ASTNode* right = static_cast<ASTNode*>($2);
        left->addChild(std::unique_ptr<ASTNode>(right));
        $$ = $1;
    }
    ;

external_declaration:
    function_definition
    {
        $$ = $1;
    }
    | declaration
    {
        $$ = $1;
    }
    ;

function_definition:
    type IDENTIFIER LPAREN parameter_list RPAREN compound_statement
    {
        auto node = new ASTNode("Function", $2);
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($4)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($6)));
        symbolTable.declareVariable($2);
        free($2);
        $$ = node;
    }
    | type IDENTIFIER LPAREN RPAREN compound_statement
    {
        auto node = new ASTNode("Function", $2);
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($5)));
        symbolTable.declareVariable($2);
        free($2);
        $$ = node;
    }
    ;

parameter_list:
    parameter_declaration
    {
        auto node = new ASTNode("ParameterList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        $$ = node;
    }
    | parameter_list COMMA parameter_declaration
    {
        ASTNode* list = static_cast<ASTNode*>($1);
        ASTNode* param = static_cast<ASTNode*>($3);
        list->addChild(std::unique_ptr<ASTNode>(param));
        $$ = $1;
    }
    ;

parameter_declaration:
    type IDENTIFIER
    {
        auto node = new ASTNode("Parameter", $2);
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        symbolTable.declareVariable($2);
        free($2);
        $$ = node;
    }
    ;

declaration:
    type init_declarator_list SEMICOLON
    {
        auto node = new ASTNode("Declaration");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($2)));
        $$ = node;
    }
    ;

init_declarator_list:
    init_declarator
    {
        auto node = new ASTNode("InitList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        $$ = node;
    }
    | init_declarator_list COMMA init_declarator
    {
        ASTNode* list = static_cast<ASTNode*>($1);
        ASTNode* init = static_cast<ASTNode*>($3);
        list->addChild(std::unique_ptr<ASTNode>(init));
        $$ = $1;
    }
    ;

init_declarator:
    IDENTIFIER
    {
        auto node = new ASTNode("Variable", $1);
        symbolTable.declareVariable($1);
        free($1);
        $$ = node;
    }
    | IDENTIFIER ASSIGN expression
    {
        auto node = new ASTNode("InitializedVar", $1);
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        symbolTable.declareVariable($1);
        free($1);
        $$ = node;
    }
    ;

type:
    INT { $$ = new ASTNode("Type", "int"); }
    | FLOAT_TYPE { $$ = new ASTNode("Type", "float"); }
    | DOUBLE { $$ = new ASTNode("Type", "double"); }
    | CHAR { $$ = new ASTNode("Type", "char"); }
    | BOOL { $$ = new ASTNode("Type", "bool"); }
    | VOID { $$ = new ASTNode("Type", "void"); }
    | STRING { $$ = new ASTNode("Type", "string"); }
    ;

compound_statement:
    LBRACE statement_list RBRACE
    {
        auto node = new ASTNode("Block");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($2)));
        $$ = node;
    }
    | LBRACE RBRACE
    {
        $$ = new ASTNode("EmptyBlock");
    }
    ;

statement_list:
    statement
    {
        auto node = new ASTNode("StatementList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        $$ = node;
    }
    | statement_list statement
    {
        ASTNode* list = static_cast<ASTNode*>($1);
        ASTNode* stmt = static_cast<ASTNode*>($2);
        list->addChild(std::unique_ptr<ASTNode>(stmt));
        $$ = $1;
    }
    ;

statement:
    expression SEMICOLON
    {
        $$ = $1;
    }
    | compound_statement
    {
        $$ = $1;
    }
    | selection_statement
    {
        $$ = $1;
    }
    | iteration_statement
    {
        $$ = $1;
    }
    | jump_statement
    {
        $$ = $1;
    }
    | declaration
    {
        $$ = $1;
    }
    | COUT OUTPUT expression SEMICOLON
    {
        auto node = new ASTNode("Output");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | COUT OUTPUT expression OUTPUT expression SEMICOLON
    {
        auto node = new ASTNode("OutputChain");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($5)));
        $$ = node;
    }
    | SEMICOLON
    {
        $$ = new ASTNode("EmptyStatement");
    }
    ;

selection_statement:
    IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
    {
        auto node = new ASTNode("IfStatement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($5)));
        $$ = node;
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    {
        auto node = new ASTNode("IfElseStatement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($5)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($7)));
        $$ = node;
    }
    ;

iteration_statement:
    WHILE LPAREN expression RPAREN statement
    {
        auto node = new ASTNode("WhileLoop");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($5)));
        $$ = node;
    }
    | FOR LPAREN expression SEMICOLON expression SEMICOLON expression RPAREN statement
    {
        auto node = new ASTNode("ForLoop");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($5)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($7)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($9)));
        $$ = node;
    }
    ;

jump_statement:
    RETURN expression SEMICOLON
    {
        auto node = new ASTNode("Return");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($2)));
        $$ = node;
    }
    | RETURN SEMICOLON
    {
        $$ = new ASTNode("ReturnVoid");
    }
    | BREAK SEMICOLON
    {
        $$ = new ASTNode("Break");
    }
    | CONTINUE SEMICOLON
    {
        $$ = new ASTNode("Continue");
    }
    ;

expression:
    assignment_expression
    {
        $$ = $1;
    }
    ;

assignment_expression:
    logical_or_expression
    {
        $$ = $1;
    }
    | IDENTIFIER ASSIGN assignment_expression
    {
        if (!symbolTable.isDeclared($1)) {
            yyerror("Undeclared variable");
            YYERROR;
        }
        auto node = new ASTNode("Assignment", $1);
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        free($1);
        $$ = node;
    }
    ;

logical_or_expression:
    logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression
    {
        auto node = new ASTNode("LogicalOR");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    ;

logical_and_expression:
    equality_expression { $$ = $1; }
    | logical_and_expression AND equality_expression
    {
        auto node = new ASTNode("LogicalAND");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    ;

equality_expression:
    relational_expression { $$ = $1; }
    | equality_expression EQ relational_expression
    {
        auto node = new ASTNode("Equal");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | equality_expression NE relational_expression
    {
        auto node = new ASTNode("NotEqual");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    ;

relational_expression:
    additive_expression { $$ = $1; }
    | relational_expression LT additive_expression
    {
        auto node = new ASTNode("LessThan");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | relational_expression LE additive_expression
    {
        auto node = new ASTNode("LessEqual");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | relational_expression GT additive_expression
    {
        auto node = new ASTNode("GreaterThan");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | relational_expression GE additive_expression
    {
        auto node = new ASTNode("GreaterEqual");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    ;

additive_expression:
    multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression
    {
        auto node = new ASTNode("Add");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | additive_expression MINUS multiplicative_expression
    {
        auto node = new ASTNode("Subtract");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    ;

multiplicative_expression:
    unary_expression { $$ = $1; }
    | multiplicative_expression MULTIPLY unary_expression
    {
        auto node = new ASTNode("Multiply");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | multiplicative_expression DIVIDE unary_expression
    {
        auto node = new ASTNode("Divide");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    | multiplicative_expression MODULO unary_expression
    {
        auto node = new ASTNode("Modulo");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        $$ = node;
    }
    ;

unary_expression:
    postfix_expression { $$ = $1; }
    | PLUS unary_expression
    {
        auto node = new ASTNode("UnaryPlus");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($2)));
        $$ = node;
    }
    | MINUS unary_expression
    {
        auto node = new ASTNode("UnaryMinus");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($2)));
        $$ = node;
    }
    | NOT unary_expression
    {
        auto node = new ASTNode("LogicalNOT");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($2)));
        $$ = node;
    }
    ;

postfix_expression:
    primary_expression { $$ = $1; }
    | postfix_expression INCREMENT
    {
        auto node = new ASTNode("PostfixIncrement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        $$ = node;
    }
    | postfix_expression DECREMENT
    {
        auto node = new ASTNode("PostfixDecrement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        $$ = node;
    }
    | IDENTIFIER LPAREN argument_expression_list RPAREN
    {
        auto node = new ASTNode("FunctionCall", $1);
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($3)));
        symbolTable.declareVariable($1);
        free($1);
        $$ = node;
    }
    | IDENTIFIER LPAREN RPAREN
    {
        auto node = new ASTNode("FunctionCall", $1);
        symbolTable.declareVariable($1);
        free($1);
        $$ = node;
    }
    ;

primary_expression:
    IDENTIFIER
    {
        if (!symbolTable.isDeclared($1)) {
            yyerror("Undeclared variable");
            YYERROR;
        }
        auto node = new ASTNode("Variable", $1);
        free($1);
        $$ = node;
    }
    | INTEGER
    {
        auto node = new ASTNode("Integer", std::to_string($1));
        $$ = node;
    }
    | FLOAT
    {
        auto node = new ASTNode("Float", std::to_string($1));
        $$ = node;
    }
    | STRING_LITERAL
    {
        auto node = new ASTNode("String", $1);
        free($1);
        $$ = node;
    }
    | CHAR_LITERAL
    {
        auto node = new ASTNode("Char", $1);
        free($1);
        $$ = node;
    }
    | TRUE
    {
        $$ = new ASTNode("Boolean", "true");
    }
    | FALSE
    {
        $$ = new ASTNode("Boolean", "false");
    }
    | LPAREN expression RPAREN
    {
        auto node = new ASTNode("Parenthesized");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($2)));
        $$ = node;
    }
    ;

argument_expression_list:
    assignment_expression
    {
        auto node = new ASTNode("ArgumentList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>($1)));
        $$ = node;
    }
    | argument_expression_list COMMA assignment_expression
    {
        ASTNode* list = static_cast<ASTNode*>($1);
        ASTNode* arg = static_cast<ASTNode*>($3);
        list->addChild(std::unique_ptr<ASTNode>(arg));
        $$ = $1;
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parser Error: %s\n", s);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Cannot open file: %s\n", argv[1]);
            return 1;
        }
    }
    
    std::cout << "C++ Parser with AST Generation" << std::endl;
    symbolTable.addBuiltins();
    
    int result = yyparse();
    
    if (yyin != stdin) {
        fclose(yyin);
    }
    
    if (result == 0 && root) {
        std::cout << "\n=== ABSTRACT SYNTAX TREE ===" << std::endl;
        root->print();
        std::cout << "============================\n" << std::endl;
    }
    
    return result;
}