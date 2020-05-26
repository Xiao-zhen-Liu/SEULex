// SEULex.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Structures.h"

using namespace std;

// Predefinitions

bool read_parse_lex_file(string path, unordered_map<string, string> &termsMap, vector<RERule> &rulesVec, vector<string> p1Vec, vector<string> p4Vec);
void parse_regex(vector<RERule> &rulesVec, unordered_map<string, string> &termsMap);
void convert_rules_2_NFA(vector<RERule> &rulesVec, NFA &nfa);
void convert_NFA_2_DFA(NFA &nfa, DFA &dfa);
void minimize_DFA(DFA &originalDFA, DFA &minimizedDFA);
void generate_C_code(const DFA& dfa, const vector<string>& p1Vec, const vector<string>& p4Vec);

int main()
{
    
}