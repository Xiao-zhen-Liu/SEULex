// SEULex.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "Common.h"

using namespace std;

// Predefinitions

bool read_parse_lex_file(string path, vector<string>& regedTermsVec, unordered_map<string, string>& regdMap, vector<RERule>& regexRulesVec, string& codeBegin, string& codeEnd);
bool parse_regex(vector<RERule>& rulesVec, const vector<string>& regedTermsVec, unordered_map<string, string>& termsMap);
void convert_rules_2_NFA(vector<RERule>& rulesVec, NFA& nfa);
void convert_NFA_2_DFA(NFA& nfa, DFA& dfa);
void minimize_DFA(const DFA& DFA_origin, DFA& DFA_minimum);
void convert_DFA_2_array(const DFA& dfa, vector<pair<int*, int>>& arrays, vector<vector<string>>& endVec);
//void generate_C_code(const DFA& dfa, const vector<string>& p1Vec, const vector<string>& p4Vec);
int generate_C_code(vector<pair<int*, int>>& arrays, vector<vector<string>>& endVec, string& codeBegin, string& codeEnd, int startState, int mode);

vector<RERule> regexRulesVec;

int main(int argc, char** argv)
{
	unordered_map<string, string> regdMap;
	vector<string> regedTermsVec, leadingConstantsVec, trailingConstantsVec;
	NFA finalNFA;
	DFA originDFA;
	DFA minimunDFA;
	int mode = -1;
	string input, codeBegin, codeEnd;
	map<string, string> terms;
	vector<pair<int*, int>> arrays;//<表（指针），表的大小>
	vector<vector<string>> endVec;

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

	if (read_parse_lex_file("lex.l", regedTermsVec, regdMap, regexRulesVec, codeBegin, codeEnd)) cout << "Finished Lex File Reading." << endl;
	if (parse_regex(regexRulesVec, regedTermsVec, regdMap)) cout << "Finished Regex parsing." << endl;
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

	convert_rules_2_NFA(regexRulesVec, finalNFA); cout << "Finished converting to NFA." << endl;
	convert_NFA_2_DFA(finalNFA, originDFA); cout << "Finished converting to DFA." << endl;
	minimize_DFA(originDFA,minimunDFA); cout << "Finished DFA minimization." << endl;
	convert_DFA_2_array(minimunDFA,arrays,endVec); cout << "Finished DFA to array." << endl;
	generate_C_code(arrays, endVec, codeBegin, codeEnd, minimunDFA.startState, mode); cout << "Finished generating C code." << endl;

	/*for (int i = 0; i < minimunDFA.statesMap.size(); i++)
	{
		cout << minimunDFA.
	}*/
}