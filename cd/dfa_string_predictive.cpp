#include <iostream>
#include <vector>
#include <map>
using namespace std;

class DFA {
public:
    int numStates;
    vector<char> alphabet;
    map<pair<int, char>, int> transitionTable;
    vector<bool> isFinalState;

    DFA(int states, vector<char> alphabet, vector<pair<pair<int, char>, int>> transitions, vector<int> finalStates) {
        numStates = states;
        this->alphabet = alphabet;
        isFinalState.resize(states + 1, false);
        for (const auto& t : transitions) {
            transitionTable[{t.first.first, t.first.second}] = t.second;
        }
        for (int state : finalStates) {
            isFinalState[state] = true;
        }
    }

    bool isAccepted(string input) {
        int currentState = 1; // Start from state 1

        for (char c : input) {
            if (transitionTable.find({currentState, c}) == transitionTable.end()) {
                return false;
            }
            currentState = transitionTable[{currentState, c}]; // Take the transition
        }
        return isFinalState[currentState];
    }

    void displayTransitionTable() {
        cout << "Transition Table:\n";
        cout << "State\t";
        for (char c : alphabet) {
            cout << c << "\t";
        }
        cout << endl;

        for (int state = 1; state <= numStates; ++state) {
            cout << state << "\t";
            for (char c : alphabet) {
                if (transitionTable.find({state, c}) != transitionTable.end()) {
                    cout << transitionTable[{state, c}] << "\t";
                } else {
                    cout << "-\t";
                }
            }
            cout << endl;
        }
    }
};

int main() {
    // User input for DFA definition
    int states;
    cout << "Enter number of states: ";
    cin >> states;

    int alphabetSize;
    cout << "Enter number of symbols in alphabet: ";
    cin >> alphabetSize;

    vector<char> alphabet(alphabetSize);
    cout << "Enter the symbols in alphabet: ";
    for (int i = 0; i < alphabetSize; ++i) {
        cin >> alphabet[i];
    }

    int numTransitions;
    cout << "Enter number of transitions: ";
    cin >> numTransitions;

    vector<pair<pair<int, char>, int>> transitions(numTransitions);
    cout << "Enter transitions in the format (fromState, symbol, toState):\n";
    for (int i = 0; i < numTransitions; ++i) {
        int fromState, toState;
        char symbol;
        cout << "From state: ";
        cin >> fromState;
        cout << "Symbol: ";
        cin >> symbol;
        cout << "To state: ";
        cin >> toState;
        transitions[i] = {{fromState, symbol}, toState};
    }

    int numFinalStates;
    cout << "Enter number of final states: ";
    cin >> numFinalStates;

    vector<int> finalStates(numFinalStates);
    cout << "Enter the final states: ";
    for (int i = 0; i < numFinalStates; ++i) {
        cin >> finalStates[i];
    }

    // Create DFA
    DFA dfa(states, alphabet, transitions, finalStates);
    dfa.displayTransitionTable();

    // Input string
    string input;
    cout << "Enter a string: ";
    cin >> input;

    if (dfa.isAccepted(input)) {
        cout << "The string is accepted by the DFA." << endl;
    } else {
        cout << "The string is not accepted by the DFA." << endl;
    }

    return 0;
}
