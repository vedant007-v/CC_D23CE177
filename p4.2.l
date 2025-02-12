%{
#include <stdio.h>
%}

%%

charusat   { printf("university"); }  // Replace "charusat" with "university"
.          { printf("%s", yytext); }  // Print other characters as they are

%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}
