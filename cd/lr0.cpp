#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

// Structure for an LR(0) Item
struct Item {
    string lhs;
    string rhs;
    int dotPos;

    // Comparator for set
    bool operator<(const Item& other) const {
        return tie(lhs, rhs, dotPos) < tie(other.lhs, other.rhs, other.dotPos);
    }
};

// Grammar productions storage
vector<string> productions;
map<int, set<Item>> states;

// Function to compute closure of a given item set
set<Item> closure(set<Item> items) {
    set<Item> result = items;
    bool changed = true;

    while (changed) {
        changed = false;
        set<Item> newItems = result;

        for (const Item& item : result) {
            if (item.dotPos < item.rhs.length()) {
                char symbol = item.rhs[item.dotPos];  // Symbol after the dot
                
                // Check if symbol is a non-terminal
                if (isupper(symbol)) {
                    for (const string& prod : productions) {
                        if (prod[0] == symbol) {
                            Item newItem = {string(1, symbol), prod.substr(3), 0}; // Convert char to string
                            if (newItems.insert(newItem).second) {
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
        result = newItems;
    }
    return result;
}

// Function to generate LR(0) items
void generateLR0Items() {
    set<Item> startState = closure({Item{string(1, productions[0][0]), productions[0].substr(3), 0}});
    states[0] = startState;
    
    cout << "LR(0) Items:\n";
    int stateId = 0;
    for (const auto& state : states) {
        cout << "State " << state.first << ":\n";
        for (const Item& item : state.second) {
            cout << "  " << item.lhs << " -> " << item.rhs.substr(0, item.dotPos) << "." << item.rhs.substr(item.dotPos) << "\n";
        }
        cout << "----------------------\n";
        stateId++;
    }
}

int main() {
    int numProductions;
    
    cout << "Enter number of productions: ";
    cin >> numProductions;
    cin.ignore();

    cout << "Enter grammar productions (format: S->AB):\n";
    for (int i = 0; i < numProductions; i++) {
        string production;
        getline(cin, production);
        productions.push_back(production);
    }

    generateLR0Items();
    
    return 0;
}

