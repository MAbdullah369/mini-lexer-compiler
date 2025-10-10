// parser/run_with_regex.cpp
// Build: g++ -std=c++17 parser/run_with_regex.cpp -o parser_runner
// This file includes your regex lexer and parser.
// With PARSER_REUSE_LEXER defined, parser.cpp won’t redefine Token/TokenType.

#define PARSER_REUSE_LEXER
#include "../regex/regex_code.cpp" // RegexLexer + Token/TokenType
#include "parser.cpp"              // Parser + AST

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // -- Examples ---------------------------------------------------------
    // NOTE: no cout/<< in this language yet; use print(x) or comment them out.

    const string ex1 = R"(
int add(int x, int y) {
    return x + y;
}

int result = add(3, 6);
)";

    // Use a tiny helper instead of cout:
    const string ex2 = R"(
int print(int x) { return x; }

int main() {
    for (int i = 0; i < 5; i = i + 1) {
        cout << i;   // if you also want cout<<, see step 3 below
    }
}
)";

    const string ex3 = R"(
int print(int x) { return x; }

int main() {
    if (a < b) {
        print(a);
    } else {
        print(b);
    }
}
)";

    const string ex4 = R"(
int main()
{
    x = (a + (b * c)) << d;
}
)";

    const string ex5 = R"(
int main() {
    x = a / b * c;
}

)";

    const string ex6 = R"(
int main() {
    x += y;
    a -= 2;
}
int main() {
    ++x;
    --y;
}
int main() {
    x++;
    y--;
}
int main() {
    x += y += z;
    a = b++ + ++c;
}
int main() {
    x = a >> b;
}
int main() {
    x = -a * b;
    if (!flag) { x = 1; }
}
int main() { x = !a; }
int main() { x = !!flag; }
int main() { x = !a * -b; }
x = foo(bar(1, 2), baz(3 + qux(4)));
int main() {
    x = foo(bar(1, 2), baz(3 + qux(4)));
}
int main() {
    x = -a * b;
    if (!flag) { x = 1; }
    y = foo(bar(1, 2), baz(true));
}
int main() {
    char a = 'x';
    char b = '\n';   // newline (single decoded char)
    char c = '\\';   // backslash
    char d = '\'';   // single quote
}
int main(){ x = y = z = 1; }
int main(){ a = (b + c) * (d - e); }
int main(){ x = (a + b) / (c - d); }
int main(){ x = a + b << c < d; }
fn add(x, y) { return x + y; }
int ok() { return 1; }
int main() {
    ??? not valid !!!
}

fn int add(int x, int y) { return x + y; }
int main() {
    string s = "hello
world";         // raw newline inside string
    int x = 1;   // this must be reported with the correct line/col
}
fn int main() {
    fn int inner() { return 1; }
    return 0;
}
int main() {
    arr[i + 1];
}
int main() {
    print();
}




)";

    const string ex7 = R"(
int main() {
    int i = 0;
    do {
        i = i + 1;
    } while (i < 5);
}

int main() {
    foo();
    bar(a, b);
    arr[i + 1] = baz(x++) + qux(y--);
}
int main() {
    int a = 5, b = a + 1, c;
}
return 1;
fn int main(){ return 0; }
fn int main(){ return 1; }

int main() {
    if (x) a = 1;
    else if (y) a = 2;
    else a = 3;
}
int main() {
    x = a + b * c << d;
}


)";

    vector<string> examples = {ex1, ex2, ex3, ex4, ex5, ex6, ex7};
    // --------------------------------------------------------------------

    try
    {
        for (size_t idx = 0; idx < examples.size(); ++idx)
        {
            const string &code = examples[idx];

            // LEX (silent)
            RegexLexer lex(code);
            auto tokens = lex.tokenize();

            // PARSE
            Parser parser(tokens);
            Program prog = parser.parseProgram();

            // OUTPUT (only example + AST)
            cout << "==================== EXAMPLE " << (idx + 1) << " ====================\n";
            cout << code << "\n";
            cout << "====================== AST =====================\n";
            prog.print(cout);
            cout << "================================================\n\n";
        }
    }
    catch (const ParseError &e)
    {
        cerr << "ParseError: " << e.what() << "\n";
        return 1;
    }
    catch (const exception &ex)
    {
        cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
