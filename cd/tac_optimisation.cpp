#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <regex>

using namespace std;

// Function to check if a string is a number (integer or float)
bool isNumber(const string &s) {
    return regex_match(s, regex(R"(^[-+]?[0-9]+(\.[0-9]+)?$)"));
}

// Function to perform constant folding
string constantFolding(const string &line) {
    istringstream iss(line);
    string lhs, eq, arg1, op, arg2;
    
    if (!(iss >> lhs >> eq >> arg1)) {
        return line; // Return unchanged if the format is incorrect
    }
    
    if (iss >> op >> arg2) { // Check if it's a binary operation
        if (isNumber(arg1) && isNumber(arg2)) {
            int val1 = stoi(arg1);
            int val2 = stoi(arg2);
            int res;
            if (op == "+") res = val1 + val2;
            else if (op == "-") res = val1 - val2;
            else if (op == "*") res = val1 * val2;
            else if (op == "/" && val2 != 0) res = val1 / val2;
            else return line;

            return lhs + " = " + to_string(res); // Replace with computed result
        }
    }
    
    return line;
}

// Function to perform common subexpression elimination
vector<string> commonSubexpressionElimination(vector<string> &lines) {
    unordered_map<string, string> exprMap;
    vector<string> optimizedLines;

    for (string &line : lines) {
        istringstream iss(line);
        string lhs, eq, arg1, op, arg2;
        
        if (!(iss >> lhs >> eq >> arg1)) {
            optimizedLines.push_back(line); // Keep unchanged if parsing fails
            continue;
        }

        if (iss >> op >> arg2) {
            string expr = arg1 + " " + op + " " + arg2;
            if (exprMap.find(expr) != exprMap.end()) {
                optimizedLines.push_back(lhs + " = " + exprMap[expr]);
            } else {
                exprMap[expr] = lhs;
                optimizedLines.push_back(line);
            }
        } else {
            optimizedLines.push_back(line);
        }
    }
    return optimizedLines;
}

// Function to perform dead code elimination
vector<string> deadCodeElimination(vector<string> &lines) {
    unordered_map<string, bool> usedVars;
    vector<string> optimizedLines;

    // Identify used variables
    for (const string &line : lines) {
        istringstream iss(line);
        string lhs, eq, arg1, op, arg2;
        
        if (!(iss >> lhs >> eq >> arg1)) continue; // Skip invalid lines

        if (iss >> op >> arg2) {
            usedVars[arg1] = true;
            usedVars[arg2] = true;
        } else {
            usedVars[arg1] = true;
        }
    }

    // Remove unused assignments
    for (const string &line : lines) {
        istringstream iss(line);
        string lhs, eq, arg1;
        if (!(iss >> lhs >> eq >> arg1)) continue;

        if (usedVars[lhs]) {
            optimizedLines.push_back(line);
        }
    }
    return optimizedLines;
}

int main() {
    string inputFileName = "tac_input.txt";
    string outputFileName = "tac_optimized.txt";

    ifstream inputFile("input.txt");
    if (!inputFile) {
        cerr << "Error: Unable to open " << inputFileName << "!" << endl;
        return 1;
    }

    vector<string> lines;
    string line;

    // Read input file
    while (getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();

    // Apply optimizations
    for (string &l : lines) {
        l = constantFolding(l);
    }
    lines = commonSubexpressionElimination(lines);
    lines = deadCodeElimination(lines);

    // Write optimized TAC
    ofstream outputFile(outputFileName);
    if (!outputFile) {
        cerr << "Error: Unable to write to " << outputFileName << "!" << endl;
        return 1;
    }

    for (const string &l : lines) {
        outputFile << l << "\n";
    }
    outputFile.close();

    cout << "Optimization complete! Check " << outputFileName << "." << endl;
    return 0;
}

