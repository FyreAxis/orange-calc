//Currently v1.0 is in development
//It will feature a CLI to solve math problems
//Further implementations will use a GUI and
//The parser will be used for the entry field

//Utility
#include <iostream>
#include <memory>
#include <functional>
#include <cmath>
#include <cctype>

//Data structures
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>

using namespace std;

struct OperatorTable {
    //Simply maps operators to their corresponding functions
    //oneOpMap is not yet implemented
    static inline unordered_map<string, function<int(int)>> oneOpMap =
    {
        {"-",    [](int a) { return -a; }},
        {"sqrt", [](int a) { return sqrt(a); }},
        {"cbrt", [](int a) { return cbrt(a); }},
        {"abs",  [](int a) { return abs(a); }},
        {"log",  [](int a) { return log10(a); }},
        {"ln",   [](int a) { return log(a); }},
        {"sin",  [](int a) { return sin(a); }},
        {"cos",  [](int a) { return cos(a); }},
        {"tan",  [](int a) { return tan(a); }},
        {"asin", [](int a) { return asin(a); }},
        {"acos", [](int a) { return acos(a); }},
        {"atan", [](int a) { return atan(a); }}
    };
    static inline unordered_map<string, function<int(int, int)>> twoOpsMap =
    {
        {"+", [](int a, int b) { return a + b; }},
        {"-", [](int a, int b) { return a - b; }},
        {"*", [](int a, int b) { return a * b; }},
        {"/", [](int a, int b) { return a / b; }},
        {"^", [](int a, int b) { return pow(a, b); }},
        {"%", [](int a, int b) { return a % b; }}
    };
    //TODO: check for oneOpMap when implementing unary operators
    bool validOp(string s) {
        return twoOpsMap.find(s) != twoOpsMap.end();
    }
    //This is where the magic happens!
    int operator()(string op, int op1, int op2) {
        //Finds location of corresponding operator
        auto iter = twoOpsMap.find(op);
        //Not found. Replace with a proper error.
        if (iter == twoOpsMap.end()) { return 0; }
        //returns a ? b where "?" is an operator
        return iter->second(op1, op2);
    }
} opTable;

//Nodes hold numbers or expressions
//By default, they don't really do anything
class Node {
    public:
    virtual ~Node() = default;
    virtual int evaluate() = 0;
};

//This specialized node simply holds a value
class OperandNode : public Node {
    private:
    int value;
    public:
    int evaluate() override { return value; }
    OperandNode(int val) : value(val) {};
};

//This specialized node holds expressions
class OperatorNode : public Node {
    private:
    string op;
    vector<unique_ptr<Node>> children;
    public:
    //Finds corresponding operator to evaluate!
    int evaluate() override {
        //TODO: pass children vec in some way
        return opTable(op, children[0]->evaluate(), children[1]->evaluate());
    }
    OperatorNode(string op, vector<unique_ptr<Node>> newChildren) :
    op(op), children(std::move(newChildren)) {};
};

//This specialized node evaluates trees of nodes
//And helpfully parses input to build trees
class EvalTree {
    private:
    static inline bool isWhitespace(char& c) {
        return c == ' ' || c == '\t'
        || c == '\n' || c == '\0';
    }
    static void removeWhitespace(string& input) {
        auto iter = input.begin();
        while (iter != input.end()) {
            if (isWhitespace(*iter) || isBracket(*iter)) {
                iter = input.erase(iter); 
            }
            else iter++;
        }
    }
    //Removes and returns the first operand
    static int extractOperand(string& input) {
        string converted = "";
        auto iter = input.begin();
        while(iter != input.end()) {
            if (isdigit(*iter)) {
                converted += *iter;
                iter = input.erase(iter);
            }
            else break;
        }
        return stoi(converted);
    }
    //Removes and returns the first operator string
    static string extractOperator(string& input) {
        string converted = "";
        auto iter = input.begin();
        while(iter != input.end()) {
            if (!isdigit(*iter)) {
                converted += *iter;
                iter = input.erase(iter);
            }
            if (opTable.validOp(converted)) {
                break;
            }
            else iter++;
        }
        return converted;
    }
    static inline bool isBracket(char& c) {
        return c == '(' || c == ')'
            || c == '{' || c == '}'
            || c == '[' || c == ']';
    }
    static inline char oppositeBracket(char& c) {
        char bracket;
        switch (c) {
            case '{':
                bracket = '}';
                break;
            case '}':
                bracket = '{';
                break;
            case '(':
                bracket = ')';
                break;
            case ')':
                bracket = '(';
                break;
            case '[':
                bracket = '[';
                break;
            case ']':
                bracket = '[';
                break;
            default:
                bracket = ' ';
        }
        return bracket;
    }
    static bool validBrackets(string& input) {
        stack<char> brackets;
        //Get all brackets
        for (char c : input) {
            if (!isBracket(c)) {
                break;
            }
            if (!brackets.empty() && oppositeBracket(c) == brackets.top()) {
                brackets.pop();
            }
            else brackets.push(c);
        }
        return brackets.empty();
    }
    public:
    static unique_ptr<Node> parse(string& expr) {
        //Clean string
        removeWhitespace(expr);
        if (!validBrackets(expr)) { return nullptr; }
        unique_ptr<Node> left = make_unique<OperandNode>(extractOperand(expr));
        while (!expr.empty()) {
            string operator1 = extractOperator(expr);
            unique_ptr<Node> right = make_unique<OperandNode>(extractOperand(expr));
            vector<unique_ptr<Node>> children;
            children.push_back(std::move(left));
            children.push_back(std::move(right));
            //Converts operator string into node
            unique_ptr<Node> result = make_unique<OperatorNode>(operator1, std::move(children));
            //Updates left with expression's result
            left = std::move(make_unique<OperandNode>(result->evaluate()));
        }
        return left;
    }
};

class Test {
    public:
    static inline void printWelcome() {
        cout << "Welcome to Orange Calculator Alpha v0.3\n";
    }
    static inline void printProjStatus() {
        cout << "Orange Calc Features:\n";
        cout << "* TUI\n";
        cout << "! Reprompts user upon successful parsing";
        cout << "* Supports +, -, *, /, ^, and % operators\n";
        cout << "! Does not support precedence (PEMDAS/GEMDAS)";
        cout << "! Currently, evaluates left to right";
        cout << "* Supports nonnegative integers as operands\n";
        cout << "! Can now parse expressions with multiple operands and operators";
        cout << "* Supports (hopefully all) whitespace\n";
        cout << "! Supports brackets, though doesn't change evaluation order";
    }
    static inline void promptUser() {
        cout << "Type \"EXIT\" (all caps) to exit or an expression to be parsed: \n";
    }
};

int main() {
    bool userRetry = true;
    string input;
    vector<int> operands = {};
    string operator1 = "";
    Test::printWelcome();
    Test::printProjStatus();
    while (userRetry) {
        Test::promptUser();
        getline(cin, input);
        if (input == "EXIT") {
            userRetry = true;
            break;
        }
        unique_ptr<Node> resultPtr = EvalTree::parse(input);
        if (resultPtr != nullptr) {
            cout << resultPtr->evaluate() << "\n";
        }
    }
    return 0;
}