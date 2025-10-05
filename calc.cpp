//Currently v1.0 is in development
//It will feature a CLI to solve math problems
//Further implementations will use a GUI and
//The parser will be used for the entry field
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cctype>
#include <memory>
#include <functional>
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
    bool validOp(char c) {
        string target{c};
        return twoOpsMap.find(target) != twoOpsMap.end();
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

//This specialized holds expressions
class OperatorNode : public Node {
    private:
    string op;
    vector<unique_ptr<Node>> children;
    public:
    //Finds corresponding operator to evaluate!
    int evaluate() override {
        return opTable(op, children[0]->evaluate(), children[1]->evaluate());
    }
    OperatorNode(string op, vector<unique_ptr<Node>> newChildren) :
    op(op), children(std::move(newChildren)) {};
};

class EvalTree {
    private:
    unique_ptr<Node> root;
    public:
    int evaluate() { return root->evaluate(); }
    EvalTree(string operator1, vector<int> operands) {
        vector<unique_ptr<Node>> children;
        for (int operand : operands) {
            children.push_back(make_unique<OperandNode>(operand));
        }
        root = make_unique<OperatorNode>(operator1, std::move(children));
    }
};

class Parser {
    private:
    static int extractNum(string& input) {
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
    static string extractOperator(string& input) {
        string converted = "";
        auto iter = input.begin();
        while(iter != input.end()) {
            if (opTable.validOp(*iter)) {
                converted += *iter;
                iter = input.erase(iter);
            }
            else iter++;
        }
        return converted;
    }
    public:
    static void removeWhitespace(string& input) {
        auto iter = input.begin();
        while (iter != input.end()) {
            if (*iter == ' ' || *iter == '\t'
            || *iter == '\n' || *iter == '\0') {
                iter = input.erase(iter); 
            }
            else iter++;
        }
    }
    static void convert(string& input, string& operator1, vector<int>& operands) {
        operands.push_back(extractNum(input));
        operator1 = extractOperator(input);
        operands.push_back(extractNum(input));
    }
};

class Test {
    public:
    static inline void printWelcome() {
        cout << "Welcome to Orange Calculator Alpha v0.2\n";
    }
    static inline void printProjStatus() {
        cout << "Orange Calc Features:\n";
        cout << "* CLI\n";
        cout << "* Supports +, -, *, /, ^, and % operators\n";
        cout << "* Supports nonnegative integers as operands\n";
        cout << "* Supports (hopefully all) whitespace\n";
    }
    static inline void promptUser() {
        cout << "Type an expression to be parsed: \n";
    }
};

int main() {
    string input;
    vector<int> operands = {};
    string operator1 = "";
    Test::printWelcome();
    Test::printProjStatus();
    Test::promptUser();
    getline(cin, input);
    Parser::removeWhitespace(input);
    Parser::convert(input, operator1, operands);
    EvalTree tree(operator1, operands);
    cout << operands.at(0) << " " << operator1 << " ";
    cout << operands.at(1) << " = " << tree.evaluate() << "\n";
    return 0;
}
