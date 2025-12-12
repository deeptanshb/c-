#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <set>

using namespace std;

struct Production {
    string left;
    string right;
};

bool derive(const string& current, const string& target, const vector<Production>& productions,
            unordered_set<string>& visited, int depth = 0) {
    if (depth > 15)
        return false;
    
    if (current == target)
        return true;
    
    if (current.size() > target.size())
        return false;
    
    if (visited.find(current) != visited.end())
        return false;
    visited.insert(current);
    
    for (const auto& prod : productions) {
        size_t pos = current.find(prod.left);
        while (pos != string::npos) {
            string next = current;
            next.replace(pos, prod.left.length(), prod.right);
                      
            if (derive(next, target, productions, visited, depth + 1))
                return true;
            pos = current.find(prod.left, pos + 1);
        }
    }
    
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Enter the starting symbol:" << endl;
    string start;
    getline(cin, start);
    
    cout << "Enter the number of production rules:" << endl;
    int n;
    cin >> n;
    cin.ignore();      
    vector<Production> productions;
    set<string> nonTerminals;
    
    cout << "Enter the production rules:" << endl;
    for (int i = 0; i < n; i++) {
        string rule;
        getline(cin, rule);
        size_t arrowPos = rule.find("->");
        if (arrowPos == string::npos) {
            cerr << "Invalid production rule format: " << rule << endl;
            return 1;
        }
        Production prod;
        prod.left = rule.substr(0, arrowPos);
        prod.right = rule.substr(arrowPos + 2);
        productions.push_back(prod);
        
        nonTerminals.insert(prod.left);
    }
    
    cout << "\nThe grammar has " << nonTerminals.size() << " non terminal symbol(s):" << endl;
    for (const auto &nt : nonTerminals) {
        cout << nt << "\n";
    }
    
    cout << "\nEnter the target string:" << endl;
    string target;
    getline(cin, target);
    
    unordered_set<string> visited;
    bool canParse = derive(start, target, productions, visited);
    
    cout << "\nResult: ";
    if (canParse)
        cout << "The target string can be derived by the grammar." << endl;
    else
        cout << "The target string cannot be derived by the grammar." << endl;
    
    return 0;
}


