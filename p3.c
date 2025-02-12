#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKENS 100
#define MAX_LENGTH 100

const char *keywords[] = {"int", "char", "return", "struct", "void", "long"};
const char *operators = "+-*/=<>!";
const char *punctuation = "(),;{}";

typedef enum { KEYWORD, IDENTIFIER, CONSTANT, OPERATOR, PUNCTUATION, INVALID } TokenType;

typedef struct {
    char value[MAX_LENGTH];
    TokenType type;
} Token;

Token tokens[MAX_TOKENS];
int tokenCount = 0;

bool isKeyword(char *str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) return true;
    }
    return false;
}

bool isOperator(char ch) {
    return strchr(operators, ch) != NULL;
}

bool isPunctuation(char ch) {
    return strchr(punctuation, ch) != NULL;
}

bool isValidIdentifier(char *str) {
    if (!isalpha(str[0]) && str[0] != '_') return false;
    for (int i = 1; str[i] != '\0'; i++) {
        if (!isalnum(str[i]) && str[i] != '_') return false;
    }
    return true;
}

void tokenize(char *str) {
    char buffer[MAX_LENGTH];
    int index = 0;
    
    while (*str) {
        if (isspace(*str)) {
            str++;
            continue;
        }
        
        if (isalpha(*str) || *str == '_') {
            index = 0;
            while (isalnum(*str) || *str == '_') {
                buffer[index++] = *str++;
            }
            buffer[index] = '\0';
            
            TokenType type = isKeyword(buffer) ? KEYWORD : IDENTIFIER;
            strcpy(tokens[tokenCount].value, buffer);
            tokens[tokenCount].type = type;
            tokenCount++;
        } 
        else if (isdigit(*str)) {
            index = 0;
            while (isdigit(*str)) {
                buffer[index++] = *str++;
            }
            buffer[index] = '\0';
            
            tokens[tokenCount].type = CONSTANT;
            strcpy(tokens[tokenCount].value, buffer);
            tokenCount++;
        } 
        else if (isOperator(*str)) {
            buffer[0] = *str;
            buffer[1] = '\0';
            tokens[tokenCount].type = OPERATOR;
            strcpy(tokens[tokenCount].value, buffer);
            tokenCount++;
            str++;
        } 
        else if (isPunctuation(*str)) {
            buffer[0] = *str;
            buffer[1] = '\0';
            tokens[tokenCount].type = PUNCTUATION;
            strcpy(tokens[tokenCount].value, buffer);
            tokenCount++;
            str++;
        } 
        else {
            buffer[0] = *str;
            buffer[1] = '\0';
            tokens[tokenCount].type = INVALID;
            strcpy(tokens[tokenCount].value, buffer);
            tokenCount++;
            str++;
        }
    }
}

void printTokens() {
    printf("\nTOKENS:\n");
    for (int i = 0; i < tokenCount; i++) {
        const char *typeStr[] = {"Keyword", "Identifier", "Constant", "Operator", "Punctuation", "Invalid"};
        printf("%s: %s\n", typeStr[tokens[i].type], tokens[i].value);
    }
}

int main() {
    char input[] = "int main() { int a = 5, 7H; char b = 'x'; return a + b; }";
    
    printf("Input Code:\n%s\n", input);
    tokenize(input);
    printTokens();

    return 0;
}
