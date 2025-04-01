#include <iostream>
#include <vector>

using namespace std;

class RDP {
    string input;
    size_t ip;
    bool flag;

public:
    RDP(const string& str) : input(str), ip(0), flag(true) {}

    void match(char expected) {
        if (ip < input.length() && input[ip] == expected) {
            ip++;
        } else {
            flag = false;
        }
    }

    void S() {
        if (ip < input.length() && input[ip] == '(') {
            match('(');
            L();
            match(')');
        } else if (ip < input.length() && input[ip] == 'a') {
            match('a');
        } else {
            flag = false;
        }
    }

    void L() {
        S();
        while (ip < input.length() && input[ip] == ',') {
            match(',');
            S();
        }
    }

    bool parse() {
        S();
        return flag && ip == input.length();
    }
};

int main() {
    int n;
    cout << "Enter number of strings: ";
    cin >> n;
    vector<string> inputs(n);
    cout << "Enter " << n << " strings: " << endl;
    for (auto &input : inputs) {
        cin >> input;
    }

    for (const auto& input : inputs) {
        RDP parser(input);
        cout << (parser.parse() ? "Valid string: " : "Invalid string: ") << input << endl;
    }

    return 0;
}
