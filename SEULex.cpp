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
void minimize_DFA(const DFA& DFA_origin, DFA& DFA_minimum);
void convert_DFA_2_array(const DFA& dfa, vector<pair<int*, int>>& arrays, vector<RERule>& endVec);
//void generate_C_code(const DFA& dfa, const vector<string>& p1Vec, const vector<string>& p4Vec);
int generate_C_code(vector<pair<int*, int>>& arrays, vector<RERule>& endVec, vector<string>& part1, vector<string>& part4, int startState, int mode);

int main(int argc, char** argv)
{
	unordered_map<string, string> regdMap;
	vector<RERule> regexRulesVec;
	vector<string> regedTermsVec, leadingConstantsVec, trailingConstantsVec;
	NFA finalNFA;
	DFA originDFA;
	DFA minimunDFA;
	int mode = -1;
	string input;
	map<string, string> terms;
	vector<string>part1, part4;
	vector<pair<int*, int>> arrays;
	vector<RERule> endVec;

	if (argc == 2)//argc: 整数,用来统计运行程序时送给main函数的命令行参数的个数
	{
		input = string(argv[1]);//argv[1] 指向在DOS命令行中执行程序名后的第一个字符串
		if (input == "lex") {
			mode = 0;
		}
		else if (input == "yacc") {
			mode = 1;
		}
		else {
			cout << "ERROR: invalid mode!" << endl;
			return -1;
		}
	}
	else {
		cout << "ERROR: invalid arguments number!" << endl;
		return -1;
	}

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
	convert_NFA_2_DFA(finalNFA, originDFA);
	minimize_DFA(originDFA,minimunDFA);
	convert_DFA_2_array(minimunDFA,arrays,endVec);
	generate_C_code(arrays, endVec, part1, part4, minimunDFA.startState, mode);
}