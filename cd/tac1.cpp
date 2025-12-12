#include <iostream>
#include <stack>
#include <unordered_map>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

unordered_map<string, string> subexpressionMap; 
int tempCounter = 1; 

string getTempVariable() {
    return "t" + to_string(tempCounter++);
}

string handleSubexpression(const string& expr) {
    if (subexpressionMap.find(expr) != subexpressionMap.end()) {
        return subexpressionMap[expr];
    } else {
        string tempVar = getTempVariable();
        subexpressionMap[expr] = tempVar;
        return tempVar;
    }
}

void generateThreeAddressCode(const string& expression) {
    stack<string> operands; 
    stack<char> operators;  

    stringstream ss(expression);

    for (int i = 0; i < expression.size(); i++) {
        char token = expression[i];

        if (isspace(token)) {
            continue; 
        }

        if (isalnum(token) ) {
            string operand(1, token);
            operands.push(operand);
        } 
        else if (token == '(') {
            operators.push(token);
        }
        else if (token == ')') {
            while (!operators.empty() && operators.top() != '(') {
                char op = operators.top();
                operators.pop();
                string operand2 = operands.top(); operands.pop();
                string operand1 = operands.top(); operands.pop();
                string expr = operand1 + " " + op + " " + operand2;
                string tempVar = handleSubexpression(expr);
                cout << tempVar << " = " << operand1 << " " << op << " " << operand2 << endl;
                operands.push(tempVar);
            }
            operators.pop();
        }
        else if (token == '+' || token == '-' || token == '*' || token == '/') {
            while (!operators.empty() && (operators.top() == '*' || operators.top() == '/' || 
                    (operators.top() == '+' || operators.top() == '-') && (token == '+' || token == '-'))) {
                char op = operators.top();
                operators.pop();
                string operand2 = operands.top(); operands.pop();
                string operand1 = operands.top(); operands.pop();
                string expr = operand1 + " " + op + " " + operand2;
                string tempVar = handleSubexpression(expr);
                cout << tempVar << " = " << operand1 << " " << op << " " << operand2 << endl;
                operands.push(tempVar);
            }
            operators.push(token);
        }
    }

    while (!operators.empty()) {
        char op = operators.top();
        operators.pop();
        string operand2 = operands.top(); operands.pop();
        string operand1 = operands.top(); operands.pop();
        string expr = operand1 + " " + op + " " + operand2;
        string tempVar = handleSubexpression(expr);
        cout << tempVar << " = " << operand1 << " " << op << " " << operand2 << endl;
        operands.push(tempVar);
    }

    cout << "Result: " << operands.top() << endl;
}

int main() {
    string expression;

    cout << "Enter an expression: ";
    getline(cin, expression);

    generateThreeAddressCode(expression);

    return 0;
}


