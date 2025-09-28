// parser/run_with_regex.cpp
// Build: g++ -std=c++17 parser/run_with_regex.cpp -o parser_runner
// This file includes your regex lexer and parser.
// With PARSER_REUSE_LEXER defined, parser.cpp won’t redefine Token/TokenType.

#define PARSER_REUSE_LEXER
#include "../regex/regex_code.cpp"   // RegexLexer + Token/TokenType
#include "parser.cpp"                // Parser + AST

int main() {
    // Sample test code
    string code = R"(
fn int my_fn(int x, float y) {
    // This is a line comment
    /*
       This is a block comment
    */
    string s = "Abdullah\nZahid";
    float a = 23.45;
    float b = 23.5;
    // invalid numbers below -- expected errors if uncommented
    // float c = 12.;
    // float d = .12;
    if (x >= 10 && y < 20.5) {
        return x + 1;
    } else {
        return x - 1;
    }
}
)";

    try {
        // --- Lexing ---
        RegexLexer lex(code);
        auto tokens = lex.tokenize();

        cout << "==================== TOKENS ====================\n[";
        bool first = true;
        for (auto &t : tokens) {
            if (t.type == TokenType::T_EOF) continue;
            if (!first) cout << ", ";
            cout << tokenToDisplay(t);
            first = false;
        }
        cout << "]\n\n";

        // --- Parsing ---
        Parser parser(tokens);
        Program prog = parser.parseProgram();

        cout << "===================== AST =====================\n";
        prog.print(cout);
        cout << "===============================================\n";

    } catch (const ParseError &e) {
        cerr << "ParseError: " << e.what() << "\n";
    } catch (const exception &ex) {
        cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}
