#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
using namespace std;

struct Token {
    string type;
    string value;
};

vector<Token> tokenize(const string &code) {
    vector<Token> tokens;

    // Token regex patterns
    vector<pair<string, regex>> tokenPatterns = {
        {"KEYWORD", regex("\\b(fn|int|string|float|bool|return|if|else|for|while)\\b")},
        {"IDENTIFIER", regex("\\b[a-zA-Z_][a-zA-Z0-9_]*\\b")},
        {"NUMBER", regex("\\b[0-9]+\\b")},
        {"STRING", regex("\"(\\\\.|[^\"])*\"")}, // handles escaped quotes
        {"OPERATOR", regex("==|=|<=|>=|!=|&&|\\|\\||\\+|-|\\*|/")},
        {"SYMBOL", regex("[(){};,]")},
        {"WHITESPACE", regex("\\s+")},
        {"INVALID", regex(".")} // fallback: catches invalid characters
    };

    string remaining = code;

    while (!remaining.empty()) {
        bool matched = false;

        for (auto &tp : tokenPatterns) {
            string type = tp.first;
            regex pattern = tp.second;
            smatch match;

            if (regex_search(remaining, match, pattern, regex_constants::match_continuous)) {
                string value = match.str();

                if (type == "WHITESPACE") {
                    // ignore whitespace
                }
                else if (type == "INVALID") {
                    cerr << "Lexical Error: Invalid token '" << value << "'\n";
                }
                else {
                    tokens.push_back({type, value});
                }

                remaining = match.suffix().str(); // move forward
                matched = true;
                break;
            }
        }

        if (!matched) break;
    }

    return tokens;
}

int main() {
    ifstream infile("input.txt");
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
