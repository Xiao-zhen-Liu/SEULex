// SEULex.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "Common.h"

using namespace std;

// Predefinitions

bool read_parse_lex_file(string path, vector<string>& regedTermsVec, unordered_map<string, string>& regdMap, vector<RERule>& regexRulesVec, vector<string>& leadingConstantsVec, vector<string>& trailingConstantsVec);
bool parse_regex(vector<RERule>& rulesVec, const vector<string>& regedTermsVec, unordered_map<string, string>& termsMap);
void convert_rules_2_NFA(vector<RERule>& rulesVec, NFA& nfa);
void convert_NFA_2_DFA(NFA& nfa, DFA& dfa);
void minimize_DFA(DFA& originalDFA, DFA& minimizedDFA);
void generate_C_code(const DFA& dfa, const vector<string>& p1Vec, const vector<string>& p4Vec);

int main()
{
	unordered_map<string, string> regdMap;
	vector<RERule> regexRulesVec;
	vector<string> regedTermsVec, leadingConstantsVec, trailingConstantsVec;
	NFA finalNFA;
	cout << read_parse_lex_file("lex.l", regedTermsVec, regdMap, regexRulesVec, leadingConstantsVec, trailingConstantsVec) << endl;
	cout << parse_regex(regexRulesVec, regedTermsVec, regdMap);
	cout << endl;
	/* for (auto s : leadingConstantsVec) cout << s << endl;
	 for (auto pair : regdMap) {
		 cout << pair.first << " " << pair.second << endl;
	 }
	 cout << endl;
	 for (auto r : regexRulesVec) {
		 cout << r.regex << " ";
		 for (auto v : r.actions) {
			 cout << v << endl;
		 }
	 }
	 cout << endl;
	 for (auto s : trailingConstantsVec) cout << s << endl;*/

	convert_rules_2_NFA(regexRulesVec, finalNFA);
}