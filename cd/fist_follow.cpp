#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
using namespace std;
map<char, set<char>> firstSet, followSet;
map<char, vector<string>> grammar;
set<char> nonTerminals;
char startSymbol;
string removeSpaces(const string &s) {
string result;
for (char ch : s) {
if (!isspace(ch))
result.push_back(ch);
}
return result;
}
vector<string> split(const string &s, char delim) {
vector<string> tokens;
string token;
istringstream tokenStream(s);
while (getline(tokenStream, token, delim)) {
if (!token.empty())
tokens.push_back(token);
}return tokens;
}
int main() {
int numProductions;
cout << "Enter number of productions: ";
cin >> numProductions;
cin.ignore();
cout << "Enter the grammar:\n";
for (int i = 0; i < numProductions; i++) {
string line;
getline(cin, line);
line = removeSpaces(line);
size_t pos = line.find("->");
if (pos == string::npos) {
cerr << "Invalid production (missing '->'): " << line << "\n";
continue;
}
char lhs = line[0];
nonTerminals.insert(lhs);
if (i == 0)
startSymbol = lhs;
string rhs_all = line.substr(pos + 2);
vector<string> alternatives = split(rhs_all, '|');
for (const auto &alt : alternatives) {
grammar[lhs].push_back(alt);
}
}
for (const auto &p : grammar) {
for (const auto &prod : p.second) {
for (char ch : prod) {
if (!isupper(ch) && ch != '#') {
firstSet[ch].insert(ch);}
}
}
}
for (char nt : nonTerminals) {
if (firstSet.find(nt) == firstSet.end())
firstSet[nt] = set<char>();
}
bool changed = true;
while (changed) {
changed = false;
for (const auto &p : grammar) {
char A = p.first;
for (const auto &prod : p.second) {
if (!prod.empty()) {
char X = prod[0];
size_t before = firstSet[A].size();
firstSet[A].insert(firstSet[X].begin(), firstSet[X].end());
if (firstSet[A].size() > before)
changed = true;
}
}
}
}
for (char nt : nonTerminals) {
followSet[nt] = set<char>();
}
followSet[startSymbol].insert('$');
changed = true;
while (changed) {
changed = false;
for (const auto &p : grammar) {char A = p.first;
for (const auto &prod : p.second) {
for (size_t i = 0; i < prod.size(); i++) {
char B = prod[i];
if (!isupper(B))
continue;
if (i + 1 < prod.size()) {
char beta = prod[i + 1];
size_t before = followSet[B].size();
followSet[B].insert(firstSet[beta].begin(), firstSet[beta].end());
followSet[B].erase('#');
if (followSet[B].size() > before)
changed = true;
} else {
size_t before = followSet[B].size();
followSet[B].insert(followSet[A].begin(), followSet[A].end());
if (followSet[B].size() > before)
changed = true;
}
}
}
}
}
cout << "\nComputed FIRST and FOLLOW sets for non-terminals:\n";
for (char nt : nonTerminals) {
cout << "FIRST(" << nt << ") = { ";
for (char ch : firstSet[nt])
cout << ch << " ";
cout << "}\n";
}
for (char nt : nonTerminals) {
cout << "FOLLOW(" << nt << ") = { ";for (char ch : followSet[nt])
cout << ch << " ";
cout << "}\n";
}
return 0;
}
