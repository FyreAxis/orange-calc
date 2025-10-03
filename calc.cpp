//Currently v1.0 is in development
//It will feature a CLI to solve math problems
//Further implementations will use a GUI and
//The parser will be used for the entry field
//And 
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cctype>
#include <memory>
using namespace std;

class Node {
    public:
    virtual ~Node() = default;
    virtual int evaluate() = 0;
};

class OperandNode : public Node {
    private:
    int value;
    public:
    int evaluate() override { return value; }
    OperandNode(int val) : value(val) {};
};

class OperatorNode : public Node {
    private:
    string op;
    vector<unique_ptr<Node>> children;
    public:
    int evaluate() override {
        int result = 0;
        if (op == "+") {
            result = children[0]->evaluate() +
            children[1]->evaluate();
        }
        else if (op == "-") {
            result = children[0]->evaluate() -
            children[1]->evaluate();
        }
        return result;
    }
    OperatorNode(string newOperator, vector<unique_ptr<Node>> newChildren) :
    op(newOperator), children(std::move(newChildren)) {};
};

class EvalTree {
    public:
    unique_ptr<Node> root;
    int evaluate() { return root->evaluate(); }
    EvalTree(vector<int> operands, string operator1) {
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
            if (*iter == '+' || *iter == '-') {
                converted += *iter;
                iter = input.erase(iter);
            }
            else iter++;
        }
        return converted;
    }
    public:
    static string removeWhitespace(string& input) {
        auto iter = input.begin();
        while (iter != input.end()) {
            if (*iter == ' ' || *iter == '\t'
            || *iter == '\n' || *iter == '\0') {
                iter = input.erase(iter); 
            }
            else iter++;
        }
        return input;
    }
    static void convert(string& input, vector<int>& operands, string& operator1) {
        operands.push_back(extractNum(input));
        operator1 = extractOperator(input);
        operands.push_back(extractNum(input));
    }
};

class Test {
    public:
    static inline void printWelcome() {
        cout << "Welcome to Orange Calculator Alpha 0.1\n";
    }
    static inline void printProjStatus() {
        cout << "Orange Calc Features:\n";
        cout << "+ CLI\n";
        cout << "+ Supports addition and subtraction (+, -)\n";
        cout << "+ Supports exactly two nonnegative integers e.g. 0, 1, 2048, 999999999, etc.\n";
        cout << "+ Supports whitespace e.g. tabs, spaces, etc.\n";
    }
    static inline void promptUser() {
        cout << "Type an expression to be parsed and solved: \n";
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
    Parser::convert(input, operands, operator1);
    EvalTree tree(operands, operator1);
    cout << operands.at(0) << " " << operator1 << " ";
    cout << operands.at(1) << " = " << tree.evaluate() << "\n";
    return 0;
}
