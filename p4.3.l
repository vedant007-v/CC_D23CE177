%{
#include <stdio.h>
int char_count = 0, word_count = 0, line_count = 0;
%}

%%

.       { char_count += yyleng; }   // Count characters (including spaces)
\\n     { line_count++; char_count++; }  // Count lines
[[:alpha:]]+ { word_count++; }   // Count words

%%

int main() {
    yylex();
    printf("Characters: %d\nWords: %d\nLines: %d\n", char_count, word_count, line_count);
    return 0;
}

int yywrap() {
    return 1;
}
