#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
using namespace std;

struct Token {
    string type;
    string value;
};

bool isKeyword(const string &word) {
    string keywords[] = {"int", "return", "if", "else", "while", "for", "float", "string"};
    for (string kw : keywords) {
        if (word == kw) return true;
    }
    return false;
}

vector<Token> tokenize(const string &code) {
    vector<Token> tokens;
    string current = "";
    enum State { START, IN_ID, IN_NUM, IN_STRING } state = START;

    for (size_t i = 0; i < code.size(); i++) {
        char c = code[i];

        switch (state) {
            case START:
                if (isspace(c)) {
                    continue;
                }
                else if (isalpha(c) || c == '_') {
                    current = c;
                    state = IN_ID;
                }
                else if (isdigit(c)) {
                    current = c;
                    state = IN_NUM;
                }
                else if (c == '"') {
                    current = "\"";
                    state = IN_STRING;
                }
                else {
                    string sym(1, c);
                    tokens.push_back({"SYMBOL", sym});
                }
                break;

            case IN_ID:
                if (isalnum(c) || c == '_') {
                    current += c;
                } else {
                    if (isKeyword(current))
                        tokens.push_back({"KEYWORD", current});
                    else
                        tokens.push_back({"IDENTIFIER", current});
                    current = "";
                    state = START;
                    i--; // reprocess this char
                }
                break;

            case IN_NUM:
                if (isdigit(c)) {
                    current += c;
                } else {
                    tokens.push_back({"NUMBER", current});
                    current = "";
                    state = START;
                    i--; // reprocess
                }
                break;

            case IN_STRING:
                current += c;
                if (c == '"') {
                    tokens.push_back({"STRING", current});
                    current = "";
                    state = START;
                }
                break;
        }
    }

    // Flush remaining
    if (!current.empty()) {
        if (state == IN_ID) {
            if (isKeyword(current))
                tokens.push_back({"KEYWORD", current});
            else
                tokens.push_back({"IDENTIFIER", current});
        } else if (state == IN_NUM) {
            tokens.push_back({"NUMBER", current});
        }
    }

    return tokens;
}

int main() {
    ifstream infile("sample_code.txt");
    if (!infile) {
        cerr << "Error: Could not open sample_code.txt\n";
        return 1;
    }

    stringstream buffer;
    buffer << infile.rdbuf();
    string code = buffer.str();

    vector<Token> tokens = tokenize(code);

    cout << "Tokens:\n";
    for (auto &tok : tokens) {
        cout << "<" << tok.type << ", " << tok.value << ">\n";
    }

    return 0;
}
