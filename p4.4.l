%{
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int has_upper = 0, has_lower = 0, has_digit = 0, has_symbol = 0;
%}

%%

^([a-zA-Z0-9*;#$@]{9,15})$  {
    has_upper = has_lower = has_digit = has_symbol = 0;
    
    for (int i = 0; i < yyleng; i++) {
        if (isupper(yytext[i])) has_upper = 1;
        else if (islower(yytext[i])) has_lower = 1;
        else if (isdigit(yytext[i])) has_digit = 1;
        else if (strchr("*;#$@", yytext[i])) has_symbol = 1;
    }
    
    if (has_upper && has_lower && has_digit && has_symbol) {
        printf("Valid password\n");
    } else {
        printf("Invalid password\n");
    }
}

. { }  // Ignore other characters

%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}
