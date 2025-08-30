#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct Token {
    string value;
    string type;
};

vector<Token> tokenize(const string &code) {
    vector<Token> tokens;

    regex token_regex(
        "fn|int|float|string|bool|return"              // keywords
        "|[a-zA-Z_][a-zA-Z0-9_]*"                     // identifiers
        "|[0-9]+"                                     // numbers
        "|\"(\\\\.|[^\"])*\""                         // strings
        "|==|=|;|,|\\(|\\)|\\{|\\}"                   // operators & punctuation
    );

    sregex_iterator iter(code.begin(), code.end(), token_regex);
    sregex_iterator end;

    while (iter != end) {
        string token_value = iter->str();
        string type;

        if (token_value == "fn" || token_value == "int" || token_value == "float" ||
            token_value == "string" || token_value == "bool" || token_value == "return") {
            type = "KEYWORD";
        } else if (regex_match(token_value, regex("[0-9]+"))) {
            type = "NUMBER";
        } else if (regex_match(token_value, regex("\"(\\\\.|[^\"])*\""))) {
            type = "STRING";
        } else if (token_value == "=" || token_value == "==") {
            type = "OPERATOR";
        } else if (token_value == ";" || token_value == "," || token_value == "(" ||
                   token_value == ")" || token_value == "{" || token_value == "}") {
            type = "SYMBOL";
        } else {
            type = "IDENTIFIER";
        }

        tokens.push_back({token_value, type});
        ++iter;
    }

    return tokens;
}

int main() {
    // Read input file
    ifstream infile("input.txt");
    if (!infile) {
        cerr << "Error: Could not open input.txt\n";
        return 1;
    }

    stringstream buffer;
    buffer << infile.rdbuf();  // read entire file
    string code = buffer.str();

    vector<Token> tokens = tokenize(code);

    cout << "Tokens:\n";
    for (const auto &tok : tokens) {
        cout << "<" << tok.type << ", " << tok.value << ">\n";
    }

    return 0;
}
