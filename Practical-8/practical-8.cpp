#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <stack>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;

// Structure to represent a production rule
struct Production {
    char nonTerminal;
    vector<string> derivations;
};

// Structure to store parsing table cell info
struct TableEntry {
    string production;
    bool isValid;

    TableEntry() : production(""), isValid(true) {}
    TableEntry(string prod) : production(prod), isValid(true) {}
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

// Calculate the First set of a string of grammar symbols
set<char> calculateFirstOfString(const string& str, const map<char, set<char>>& firstSets) {
    set<char> result;

    if (str.empty() || str == "ε") {
        result.insert('ε');
        return result;
    }

    bool allDeriveEpsilon = true;

    for (size_t i = 0; i < str.length(); i++) {
        char symbol = str[i];

        if (isTerminal(symbol)) {
            result.insert(symbol);
            allDeriveEpsilon = false;
            break;
        }

        if (isNonTerminal(symbol)) {
            bool derivesEpsilon = false;

            for (char terminal : firstSets.at(symbol)) {
                if (terminal != 'ε') {
                    result.insert(terminal);
                } else {
                    derivesEpsilon = true;
                }
            }

            if (!derivesEpsilon) {
                allDeriveEpsilon = false;
                break;
            }

            if (i == str.length() - 1 && derivesEpsilon) {
                result.insert('ε');
            }
        }
    }

    if (allDeriveEpsilon && !str.empty()) {
        result.insert('ε');
    }

    return result;
}

// Construct the predictive parsing table
map<char, map<char, TableEntry>> constructParsingTable(
    const vector<Production>& grammar,
    const map<char, set<char>>& firstSets,
    const map<char, set<char>>& followSets,
    bool& isLL1,
    const set<char>& terminals) {

    map<char, map<char, TableEntry>> table;
    isLL1 = true;

    // Initialize table cells
    for (const auto& production : grammar) {
        char nonTerminal = production.nonTerminal;
        for (char terminal : terminals) {
            table[nonTerminal][terminal] = TableEntry();
        }
        table[nonTerminal]['$'] = TableEntry();
    }

    // Fill the table
    for (const auto& production : grammar) {
        char nonTerminal = production.nonTerminal;

        for (const string& derivation : production.derivations) {
            set<char> firstOfDerivation = calculateFirstOfString(derivation, firstSets);

            for (char terminal : firstOfDerivation) {
                if (terminal != 'ε') {
                    // If already has an entry, it's not LL(1)
                    if (!table[nonTerminal][terminal].production.empty()) {
                        isLL1 = false;
                        table[nonTerminal][terminal].isValid = false;
                    }
                    table[nonTerminal][terminal].production = derivation;
                } else {
                    // For epsilon, use Follow set
                    for (char followTerminal : followSets.at(nonTerminal)) {
                        if (!table[nonTerminal][followTerminal].production.empty()) {
                            isLL1 = false;
                            table[nonTerminal][followTerminal].isValid = false;
                        }
                        table[nonTerminal][followTerminal].production = derivation;
                    }
                }
            }
        }
    }

    return table;
}

// Print the parsing table
void printParsingTable(const map<char, map<char, TableEntry>>& table,
                       const vector<char>& nonTerminals,
                       const set<char>& terminals) {
    cout << "\nPredictive Parsing Table:\n";

    // Print the header
    cout << setw(5) << " ";
    for (char terminal : terminals) {
        cout << setw(10) << terminal;
    }
    cout << setw(10) << "$";
    cout << endl;

    // Calculate width for separator line
    int lineWidth = 5 + (terminals.size() + 1) * 10;

    // Print separator line
    cout << string(lineWidth, '-') << endl;

    // Print table rows
    for (char nonTerminal : nonTerminals) {
        cout << setw(5) << nonTerminal;
        for (char terminal : terminals) {
            string entry = table.at(nonTerminal).at(terminal).production;
            if (entry.empty()) {
                cout << setw(10) << "-";
            } else if (!table.at(nonTerminal).at(terminal).isValid) {
                cout << setw(10) << entry + "*"; // Mark conflicts
            } else {
                cout << setw(10) << entry;
            }
        }

        // Print entry for $
        string dollarEntry = table.at(nonTerminal).at('$').production;
        if (dollarEntry.empty()) {
            cout << setw(10) << "-";
        } else if (!table.at(nonTerminal).at('$').isValid) {
            cout << setw(10) << dollarEntry + "*"; // Mark conflicts
        } else {
            cout << setw(10) << dollarEntry;
        }

        cout << endl;
    }
}

// Validate input string using the parsing table
bool validateString(const string& input,
                   const map<char, map<char, TableEntry>>& table,
                   char startSymbol,
                   bool showSteps) {
    stack<char> parseStack;
    size_t index = 0;

    // Push end marker and start symbol
    parseStack.push('$');
    parseStack.push(startSymbol);

    string inputWithEndMarker = input + "$";

    if (showSteps) {
        cout << "\nParsing Steps for \"" << input << "\":" << endl;
        cout << left << setw(20) << "Stack" << setw(20) << "Input" << "Action" << endl;
        cout << string(60, '-') << endl;
    }

    while (!parseStack.empty()) {
        string stackStr = "";
        stack<char> tempStack = parseStack;
        while (!tempStack.empty()) {
            stackStr = tempStack.top() + stackStr;
            tempStack.pop();
        }

        string remainingInput = inputWithEndMarker.substr(index);

        // Get top of stack and current input
        char top = parseStack.top();
        char currentInput = inputWithEndMarker[index];

        if (showSteps) {
            // Print current state
            cout << left << setw(20) << stackStr << setw(20) << remainingInput;
        }

        // If top is a terminal, it should match current input
        if (isTerminal(top) || top == '$') {
            if (top == currentInput) {
                if (showSteps) {
                    cout << "Match and pop " << top << endl;
                }
                parseStack.pop();
                index++;
            } else {
                if (showSteps) {
                    cout << "Error: Expected " << top << ", got " << currentInput << endl;
                }
                return false;
            }
        }
        // If top is a non-terminal, look up in table
        else if (isNonTerminal(top)) {
            if (table.count(top) == 0 || table.at(top).count(currentInput) == 0 ||
                table.at(top).at(currentInput).production.empty()) {
                if (showSteps) {
                    cout << "Error: No production for " << top << " on input " << currentInput << endl;
                }
                return false;
            }

            string production = table.at(top).at(currentInput).production;
            if (showSteps) {
                cout << "Apply " << top << " -> " << production << endl;
            }

            parseStack.pop(); // Pop the non-terminal

            // Push the production in reverse order
            if (production != "ε") {
                for (int i = production.length() - 1; i >= 0; i--) {
                    parseStack.push(production[i]);
                }
            }
        }
    }

    return true;
}

// Parse and validate multiple test cases
void validateMultipleStrings(const vector<string>& testCases,
                           const map<char, map<char, TableEntry>>& table,
                           char startSymbol,
                           bool showDetails) {
    cout << "\nValidating Test Cases:" << endl;
    cout << string(60, '-') << endl;

    for (const string& testCase : testCases) {
        bool isValid = validateString(testCase, table, startSymbol, showDetails);
        cout << "\"" << testCase << "\" is " << (isValid ? "Valid" : "Invalid") << " string" << endl;

        if (showDetails) {
            cout << string(60, '-') << endl;
        }
    }
}

// Get terminals from the grammar
set<char> getTerminals(const vector<Production>& grammar) {
    set<char> terminals;

    for (const auto& production : grammar) {
        for (const string& derivation : production.derivations) {
            if (derivation == "ε") continue;

            for (char symbol : derivation) {
                if (isTerminal(symbol)) {
                    terminals.insert(symbol);
                }
            }
        }
    }

    return terminals;
}

// Get non-terminals from the grammar
vector<char> getNonTerminals(const vector<Production>& grammar) {
    vector<char> nonTerminals;

    for (const auto& production : grammar) {
        nonTerminals.push_back(production.nonTerminal);
    }

    return nonTerminals;
}

// Define the given grammar
vector<Production> defineGrammar() {
    return {
        {'S', {"ABC", "D"}},
        {'A', {"a", "ε"}},
        {'B', {"b", "ε"}},
        {'C', {"(S)", "c"}},
        {'D', {"AC"}}
    };
}

int main() {
    // Define the grammar
    vector<Production> grammar = defineGrammar();

    // Extract terminals and non-terminals
    set<char> terminals = getTerminals(grammar);
    vector<char> nonTerminals = getNonTerminals(grammar);

    // Compute First and Follow sets
    map<char, set<char>> firstSets = computeFirstSets(grammar);
    map<char, set<char>> followSets = computeFollowSets(grammar, firstSets);

    // Print First and Follow sets
    cout << "First Sets:" << endl;
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

    cout << "\nFollow Sets:" << endl;
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

    // Construct parsing table and check if grammar is LL(1)
    bool isLL1;
    map<char, map<char, TableEntry>> parsingTable =
        constructParsingTable(grammar, firstSets, followSets, isLL1, terminals);

    // Print parsing table
    printParsingTable(parsingTable, nonTerminals, terminals);

    // Check if grammar is LL(1)
    cout << "\nThe grammar is " << (isLL1 ? "LL(1)" : "not LL(1)") << endl;

    // Define all test cases
    vector<string> predefinedTestCases = {
        "abc", "ac", "(abc)", "c", "(ac)", "a", "()", "(ab)", "abcabc", "b"
    };

    char choice;
    cout << "\nSelect an option:" << endl;
    cout << "1. Validate predefined test cases (abc, ac, (abc), c, (ac), a, (), (ab), abcabc, b)" << endl;
    cout << "2. Enter custom test cases" << endl;
    cout << "Enter choice (1 or 2): ";
    cin >> choice;

    vector<string> testCases;

    if (choice == '1') {
        testCases = predefinedTestCases;
    } else if (choice == '2') {
        string testCaseInput;
        cout << "Enter test cases separated by spaces: ";
        cin.ignore(); // Clear newline from previous input
        getline(cin, testCaseInput);

        // Parse input string into individual test cases
        stringstream ss(testCaseInput);
        string testCase;
        while (ss >> testCase) {
            testCases.push_back(testCase);
        }
    } else {
        cout << "Invalid choice. Using predefined test cases." << endl;
        testCases = predefinedTestCases;
    }

    // Ask for detailed output
    char detailOption;
    cout << "Show detailed parsing steps? (y/n): ";
    cin >> detailOption;
    bool showDetails = (detailOption == 'y' || detailOption == 'Y');

    // If grammar is LL(1), validate test cases
    if (isLL1) {
        validateMultipleStrings(testCases, parsingTable, 'S', showDetails);
    } else {
        cout << "Cannot validate strings as the grammar is not LL(1)." << endl;
    }

    return 0;
}
