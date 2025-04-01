#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>

using namespace std;

// Structure to represent a production rule
struct Production {
    char nonTerminal;
    vector<string> derivations;
};

// Check if a character is a terminal
bool isTerminal(char symbol) {
    return !isupper(symbol) && symbol != 'ε';
}

// Check if a character is a non-terminal
bool isNonTerminal(char symbol) {
    return isupper(symbol);
}

// Function to compute First sets for all non-terminals
map<char, set<char>> computeFirstSets(const vector<Production>& grammar) {
    map<char, set<char>> firstSets;

    // Initialize First sets for all non-terminals
    for (const auto& production : grammar) {
        firstSets[production.nonTerminal] = set<char>();
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& production : grammar) {
            char nonTerminal = production.nonTerminal;

            for (const string& derivation : production.derivations) {
                // Case 1: If X -> ε is a production, then add ε to First(X)
                if (derivation == "ε") {
                    if (firstSets[nonTerminal].insert('ε').second) {
                        changed = true;
                    }
                    continue;
                }

                // Case 2: If X -> Y1Y2...Yn is a production
                bool allDeriveEpsilon = true;
                for (size_t i = 0; i < derivation.length(); i++) {
                    char symbol = derivation[i];

                    // If the symbol is a terminal, add it to First(X) and break
                    if (isTerminal(symbol)) {
                        if (firstSets[nonTerminal].insert(symbol).second) {
                            changed = true;
                        }
                        allDeriveEpsilon = false;
                        break;
                    }

                    // If the symbol is a non-terminal
                    if (isNonTerminal(symbol)) {
                        bool derivesEpsilon = false;

                        // Add all non-epsilon terminals from First(Y) to First(X)
                        for (char terminal : firstSets[symbol]) {
                            if (terminal != 'ε') {
                                if (firstSets[nonTerminal].insert(terminal).second) {
                                    changed = true;
                                }
                            } else {
                                derivesEpsilon = true;
                            }
                        }

                        // If Y does not derive ε, then stop here
                        if (!derivesEpsilon) {
                            allDeriveEpsilon = false;
                            break;
                        }

                        // If it's the last symbol and it derives ε, then add ε to First(X)
                        if (i == derivation.length() - 1 && derivesEpsilon) {
                            if (firstSets[nonTerminal].insert('ε').second) {
                                changed = true;
                            }
                        }
                    }
                }

                // If all symbols in the derivation can derive ε, add ε to First(X)
                if (allDeriveEpsilon && derivation.length() > 0) {
                    if (firstSets[nonTerminal].insert('ε').second) {
                        changed = true;
                    }
                }
            }
        }
    }

    return firstSets;
}

// Function to compute Follow sets for all non-terminals
map<char, set<char>> computeFollowSets(const vector<Production>& grammar,
                                       const map<char, set<char>>& firstSets) {
    map<char, set<char>> followSets;

    // Initialize Follow sets for all non-terminals
    for (const auto& production : grammar) {
        followSets[production.nonTerminal] = set<char>();
    }

    // Add $ to Follow(S), where S is the start symbol
    followSets[grammar[0].nonTerminal].insert('$');

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& production : grammar) {
            char nonTerminal = production.nonTerminal;

            for (const string& derivation : production.derivations) {
                if (derivation == "ε") continue;

                for (size_t i = 0; i < derivation.length(); i++) {
                    char symbol = derivation[i];

                    // If the symbol is a non-terminal
                    if (isNonTerminal(symbol)) {
                        // Case 1: If A -> αBβ, add First(β) - {ε} to Follow(B)
                        if (i < derivation.length() - 1) {
                            bool allDerivesEpsilon = true;

                            for (size_t j = i + 1; j < derivation.length(); j++) {
                                char nextSymbol = derivation[j];

                                if (isTerminal(nextSymbol)) {
                                    if (followSets[symbol].insert(nextSymbol).second) {
                                        changed = true;
                                    }
                                    allDerivesEpsilon = false;
                                    break;
                                } else {
                                    // Add First(nextSymbol) - {ε} to Follow(symbol)
                                    bool derivesEpsilon = false;
                                    for (char terminal : firstSets.at(nextSymbol)) {
                                        if (terminal != 'ε') {
                                            if (followSets[symbol].insert(terminal).second) {
                                                changed = true;
                                            }
                                        } else {
                                            derivesEpsilon = true;
                                        }
                                    }

                                    // If nextSymbol does not derive ε, stop here
                                    if (!derivesEpsilon) {
                                        allDerivesEpsilon = false;
                                        break;
                                    }
                                }
                            }

                            // Case 2: If A -> αBβ and β can derive ε, add Follow(A) to Follow(B)
                            if (allDerivesEpsilon) {
                                for (char terminal : followSets[nonTerminal]) {
                                    if (followSets[symbol].insert(terminal).second) {
                                        changed = true;
                                    }
                                }
                            }
                        }
                        // Case 3: If A -> αB, add Follow(A) to Follow(B)
                        else {
                            for (char terminal : followSets[nonTerminal]) {
                                if (followSets[symbol].insert(terminal).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return followSets;
}

// Print the First and Follow sets
void printSets(const map<char, set<char>>& firstSets,
              const map<char, set<char>>& followSets) {
    // Print First sets
    for (const auto& pair : firstSets) {
        cout << "First(" << pair.first << ") = {";
        bool first = true;
        for (char symbol : pair.second) {
            if (!first) cout << ", ";
            cout << symbol;
            first = false;
        }
        cout << "}" << endl;
    }

    cout << endl;

    // Print Follow sets
    for (const auto& pair : followSets) {
        cout << "Follow(" << pair.first << ") = {";
        bool first = true;
        for (char symbol : pair.second) {
            if (!first) cout << ", ";
            cout << symbol;
            first = false;
        }
        cout << "}" << endl;
    }
}

int main() {
    // Define the grammar
    vector<Production> grammar = {
        {'S', {"ABC", "D"}},
        {'A', {"a", "ε"}},
        {'B', {"b", "ε"}},
        {'C', {"(S)", "c"}},
        {'D', {"AC"}}
    };

    // Compute First and Follow sets
    map<char, set<char>> firstSets = computeFirstSets(grammar);
    map<char, set<char>> followSets = computeFollowSets(grammar, firstSets);

    // Print the results
    printSets(firstSets, followSets);

    return 0;
}
