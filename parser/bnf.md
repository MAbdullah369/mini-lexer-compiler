# Language BNF (subset used in parser)

<program> ::= { <top_decl> }

<top_decl> ::= <fn_decl> | <var_decl>

<fn_decl> ::= "fn" <type> <identifier> "(" <param_list_opt> ")" "{" { <stmt> } "}"

<param_list_opt> ::= <param> { "," <param> } | ε
<param> ::= <type> <identifier>

<var_decl> ::= <type> <identifier> [ "=" <expr> ] "." 

<type> ::= "int" | "float" | "string" | "bool"

<stmt> ::= <var_decl>
         | "return" [ <expr> ] "."
         | <expr> "."

<expr> ::= <logic_or>
<logic_or> ::= <logic_and> { "||" <logic_and> }
<logic_and> ::= <equality> { "&&" <equality> }
<equality> ::= <comparison> { ("==" | "!=") <comparison> }
<comparison> ::= <term> { ("<" | ">" | "<=" | ">=") <term> }
<term> ::= <factor> { ("+" | "-") <factor> }
<factor> ::= <unary> { ("*" | "/") <unary> }
<unary> ::= ("-" | "!") <unary> | <primary>
<primary> ::= <identifier> [ "(" <arg_list_opt> ")" ] | <int_lit> | <float_lit> | <string_lit> | "(" <expr> ")"

<arg_list_opt> ::= <expr> { "," <expr> } | ε
