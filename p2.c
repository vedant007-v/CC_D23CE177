#include <stdio.h>
#include <stdlib.h>

#define MAX_STATES 10
#define MAX_SYMBOLS 10

int transition[MAX_STATES][MAX_SYMBOLS];
int numStates, numSymbols, initialState, numAcceptStates, acceptStates[MAX_STATES];
char symbols[MAX_SYMBOLS];

int getSymbolIndex(char c) {
    for (int i = 0; i < numSymbols; i++) {
        if (symbols[i] == c) {
            return i;
        }
    }
    return -1;
}

int isAcceptState(int state) {
    for (int i = 0; i < numAcceptStates; i++) {
        if (acceptStates[i] == state) {
            return 1;
        }
    }
    return 0;
}

void processString(char *str) {
    int currentState = initialState;
    
    for (int i = 0; str[i] != '\0'; i++) {
        int symbolIndex = getSymbolIndex(str[i]);
        if (symbolIndex == -1) {
            printf("Invalid String\n");
            return;
        }
        currentState = transition[currentState][symbolIndex];
    }

    if (isAcceptState(currentState)) {
        printf("Valid String\n");
    } else {
        printf("Invalid String\n");
    }
}

int main() {
    printf("Number of input symbols: ");
    scanf("%d", &numSymbols);

    printf("Input symbols: ");
    for (int i = 0; i < numSymbols; i++) {
        scanf(" %c", &symbols[i]);
    }

    printf("Enter number of states: ");
    scanf("%d", &numStates);

    printf("Initial state: ");
    scanf("%d", &initialState);

    printf("Number of accepting states: ");
    scanf("%d", &numAcceptStates);

    printf("Accepting states: ");
    for (int i = 0; i < numAcceptStates; i++) {
        scanf("%d", &acceptStates[i]);
    }

    printf("Transition table:\n");
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numSymbols; j++) {
            printf("State %d to %c -> ", i, symbols[j]);
            scanf("%d", &transition[i][j]);
        }
    }

    char inputString[100];
    printf("Input string: ");
    scanf("%s", inputString);

    processString(inputString);

    return 0;
}
