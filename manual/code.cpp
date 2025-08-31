#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

struct Token {
    string type;
    string value;
};

vector<Token> tokenize(const string &code) {
    vector<Token> tokens;
    int i = 0;

    while (i < (int)code.size()) {
        char c = code[i];

        // Skip whitespace
        if (isspace(c)) {
            i++;
            continue;
        }

        // Keywords or identifiers
        if (isalpha(c) || c == '_') {
            string word;
            while (i < (int)code.size() && (isalnum(code[i]) || code[i] == '_')) {
                word += code[i++];
            }
            if (word == "fn" || word == "int" || word == "string" || word == "float" ||
                word == "bool" || word == "return" || word == "if" || word == "else" ||
                word == "for" || word == "while" || word == "main") {
                tokens.push_back({"KEYWORD", word});
            } else {
                tokens.push_back({"IDENTIFIER", word});
            }
            continue;
        }

        // Numbers (with invalid identifier check)
        if (isdigit(c)) {
            string num;
            while (i < (int)code.size() && isdigit(code[i])) {
                num += code[i++];
            }
            // If a digit is followed by a letter/underscore → invalid identifier
            if (i < (int)code.size() && (isalpha(code[i]) || code[i] == '_')) {
                string invalid = num;
                while (i < (int)code.size() && (isalnum(code[i]) || code[i] == '_')) {
                    invalid += code[i++];
                }
                cerr << "Lexical Error: Invalid identifier '" << invalid << "'\n";
                continue;
            }
            tokens.push_back({"NUMBER", num});
            continue;
        }

        // Strings
        if (c == '"') {
            string str;
            str += c;
            i++;
            while (i < (int)code.size() && code[i] != '"') {
                str += code[i++];
            }
            if (i < (int)code.size()) {
                str += code[i++]; // closing quote
                tokens.push_back({"STRING", str});
            } else {
                cerr << "Lexical Error: Unterminated string literal\n";
            }
            continue;
        }

        // Operators
        if (c == '=' || c == '!' || c == '<' || c == '>' ||
            c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '&' || c == '|') {
            string op;
            op += c;
            i++;
            if ((c == '=' || c == '!' || c == '<' || c == '>') && i < (int)code.size() && code[i] == '=') {
                op += code[i++];
            } else if ((c == '&' || c == '|') && i < (int)code.size() && code[i] == c) {
                op += code[i++];
            }
            tokens.push_back({"OPERATOR", op});
            continue;
        }

        // Symbols
        if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == ',') {
            string sym(1, c);
            tokens.push_back({"SYMBOL", sym});
            i++;
            continue;
        }

        // Invalid character
        cerr << "Lexical Error: Invalid token '" << c << "'\n";
        i++;
    }

    return tokens;
}

int main() {
    ifstream infile("sample_code.txt");
    if (!infile) {
        cerr << "Error: input.txt not found!\n";
        return 1;
    }

    string code((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());
    vector<Token> tokens = tokenize(code);

    cout << "Tokens:\n";
    for (auto &t : tokens) {
        cout << "<" << t.type << ", " << t.value << ">\n";
    }

    return 0;
}
