%{
#include <stdio.h>
#include <stdlib.h>
%}

%%
"#".*                       { printf("Preprocessor Directive: %s\n", yytext); }
"include"                   { printf("Preprocessor Keyword: %s\n", yytext); }
"<"[a-zA-Z0-9_.]+">"        { printf("Header File: %s\n", yytext); }

"int"|"float"|"char"|"return"   { printf("Keyword: %s\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*         { printf("Identifier: %s\n", yytext); }

[0-9]+                         { printf("Constant: %s\n", yytext); }
[0-9]+"."[0-9]+                { printf("Float Constant: %s\n", yytext); }

"\""[^"]*"\""                  { printf("String: %s\n", yytext); }

"="                             { printf("Operator: %s\n", yytext); }
"+"|"-"|"*"|"/"|"%"             { printf("Operator: %s\n", yytext); }

"("|")"|"{"|"}"|";"|","        { printf("Punctuation: %s\n", yytext); }

[ \t\n]                        { /* Ignore whitespace */ }
.                               { printf("Unknown: %s\n", yytext); }
%%

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Error opening file");
            return 1;
        }
    }
    yylex();
    fclose(yyin);
    return 0;
}

int yywrap() { return 1; }
