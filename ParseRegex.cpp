#include "pch.h"
#include "Common.h"

using namespace std;

char convert_escape_char(const char& c) {
	switch (c)
	{
	case 'n':
		return '\n';
		break;
	case 't':
		return '\t';
		break;
	case 'v':
		return '\v';
		break;
	case 'f':
		return '\f';
		break;
	case '\\':
		return '\\';
		break;
	case '\"':
		return '\"';
		break;
	case '\'':
		return '\'';
		break;
	case 'r':
		return '\r';
		break;
	default:
		return c;
		break;
	}
}

// Used only for curly brackets
void process_escape_inside_square_brackets(string& charClass) {
	string rst;
	char cur;
	bool flag = false;
	for (size_t i = 0; i < charClass.length(); i++) {
		cur = charClass[i];
		if (cur == '\\')
		{
			flag = true;
			continue;
		}
		if (flag)
		{
			rst += convert_escape_char(cur);
			flag = false;
			continue;
		}
		rst += cur;
	}
	charClass = rst;
}

bool process_curly_brackets(string& regex, unordered_map<string, string>& termsMap) {
	string term, rst;
	bool inside = false;
	char cur;
	for (size_t i = 0; i < regex.length(); i++)
	{
		cur = regex[i];
		if (cur == '{' && ((i > 0 && regex[i - 1] != '\\') || i == 0)) {
			// Entered bracket.
			if (!inside) {
				inside = true;
				term.clear();
				continue;
			} else {
				cout << "REGEX ERROR: Multiple curly brackets in regex <" << regex << ">" << endl;
				return false;
			}
		}
		if (cur == '}' && ((i > 0 && regex[i - 1] != '\\') || i == 0)) {
			if (inside) {
				// Exit bracket.
				inside = false;
				// Lookup the map
				if (termsMap.find(term) != termsMap.end()) {
					rst += termsMap[term];
					continue;
				} else {
					cout << "REGEX ERROR: term <" << term << ">" << " not defined!"<< endl;
					return false;
				}
			}
		}
		if (inside) term += cur;
		else rst += cur;
	}
	if (inside) {
		cout << "REGEX ERROR: Unclosed curly bracket mark for regex <" << regex << ">" << endl;
		return false;
	}
	regex = rst; // replace original regex
	return true;
}

bool get_char_set_string(const string& content, string& rst) {
	set<char> charset;
	string tmp;
	bool neg = false;
	char cur;
	if (content[0] == '^') {
		// Negative set
		tmp = content.substr(1);
		neg = true;
	}
	else tmp = content;
	process_escape_inside_square_brackets(tmp); // Replace escapes with normal characters;
	// place chars into charset
	for (size_t i = 0; i < tmp.length(); i++)
	{
		cur = tmp[i];
		if (cur == '-' && i > 0) {
			char start = tmp[i - 1];
			if (i + 1 > tmp.length() - 1) {
				cout << "REGEX ERROR: wrong character set format in <" << content << ">" << endl;
				return false;
			}
			else {
				char end = tmp[i + 1];
				auto start_position = ALL_CHARS.find(start);
				auto end_position = ALL_CHARS.find(end);
				if (isalnum(start) && isalnum(end) && start_position <= end_position) {
					for (auto ch : ALL_CHARS.substr(start_position, end_position - start_position + 1)) {
						charset.insert(ch);
					}
				}
				else {
					cout << "REGEX ERROR: illegal character set in <" << content << ">" << endl;
					return false;
				}
			}
		}
		else charset.insert(cur);
	}
	// Process negation
	if (neg) {
		set<char> tempset;
		for (auto ch : ALL_CHARS) {
			if (charset.find(ch) == charset.end()) {
				tempset.insert(ch);
			}
		}
		charset = tempset;
	}
	
	// Construct result string with standard alternation format.
	rst = "(";
	for (auto ch : charset) {
		// Process escape chars
		if (OPERATOR_CHARS.find(ch) != string::npos) {
			rst += '\\';
		}
		rst += ch;
		rst += '|';
	}
	rst.pop_back(); // Delete reduant '|'
	rst += ")";
	return true;
}

bool process_square_brackets(string& regex) {
	string content, rst, altstr;
	bool inside = false;
	char cur;
	for (size_t i = 0; i < regex.length(); i++)
	{
		cur = regex[i];
		if (cur == '[' && ((i > 0 && regex[i - 1] != '\\') || i == 0)) {
			// Entered bracket.
			if (!inside) {
				inside = true;
				content.clear();
				continue;
			}
			else {
				cout << "REGEX ERROR: Multiple square brackets in regex <" << regex << ">" << endl;
				return false;
			}
		}
		if (cur == ']' && ((i > 0 && regex[i - 1] != '\\') || i == 0)) {
			if (inside) {
				// Exit bracket.
				inside = false;
				if (!get_char_set_string(content, altstr)) return false;
				rst += altstr;
				continue;
			}
		}
		if (inside) content += cur;
		else rst += cur;
	}
	if (inside) {
		cout << "REGEX ERROR: Unclosed square bracket mark for regex <" << regex << ">" << endl;
		return false;
	}
	regex = rst; // replace original regex
	return true;
}

void process_match_all(string& regex) {
	string rst;
	char cur;
	for (size_t i = 0; i < regex.length(); i++)
	{
		cur = regex[i];
		if (cur == '.' && (i == 0 || regex[i - 1] != '\\')) {
			string tmp;
			get_char_set_string("^\n", tmp);
			rst += tmp;
			continue;
		}
		rst += cur;
	}
	regex = rst;
}

bool is_escape_char(size_t i, const string& regex) {
	char tmp;
	size_t j = i - 1;
	int ct = 0;
	// Look back, find how many consective '\\'
	while (j >= 0)
	{
		tmp = regex[j];
		if (tmp != '\\') break;
		else ct++, j--;
	}
	// Odd number of '\\'s before this char mean this is an escape char
	return (ct & 1);
}

// SHOULD BLANK BE COUNTED IN?
bool process_double_quotation_marks(string& regex) {
	string rst;
	bool inside = false;
	char cur;
	for (size_t i = 0; i < regex.length(); i++)
	{
		cur = regex[i];
		if (cur == '\"') {
			if (!is_escape_char(i, regex)) {
				// Entered double quotation mark start, or touched end.
				inside = !inside; // Flip
				continue; // Ignore this char
			}
		}
		if (inside) {
			// Inside double quotation mark, watch out for escape chars.
			if (OPERATOR_CHARS.find(cur) != string::npos && !is_escape_char(i, regex) && cur != '\\') rst += '\\'; // is an escape char but not escaped, put '\\' before it
		}
		rst += cur;
	}
	if (inside) {
		cout << "REGEX ERROR: Unclosed double quotation mark for regex <" << regex << ">" << endl;
		return false;
	}
	regex = rst; // replace original regex
	return true;
}

bool process_special_operators(string& regex) {
	string rst, substring;
	char cur;
	size_t i = 0;
	while (i < regex.length())
	{
		cur = regex[i];
		if ((cur == '+' || cur == '?') && !is_escape_char(i, regex)) {
			// Process round brackets.
			if (i - 1 >= 0 && regex[i - 1] == ')' && !is_escape_char(i, regex)) {
				int tmp = i - 1;
				size_t ctr = 1;
				size_t sublen = 1;
				rst.pop_back();
				while (ctr > 0)
				{
					rst.pop_back();
					tmp--, sublen++;
					if (tmp < 0) {
						cout << "REGEX ERROR: Wrong round brackets in <" << regex << ">" << endl;
						return false;
					}
					if (!is_escape_char(tmp, regex)) {
						if (regex[tmp] == '(') ctr--;
						if (regex[tmp] == ')') ctr++;
					}
				}
				substring = regex.substr(tmp, sublen);
			}
			else {
				if (i - 1 < 0) {
					cout << "REGEX ERROR: No content before + or ? operator in <" << regex << ">" << endl;
					return false;
				}
				rst.pop_back();
				if (!is_escape_char(i - 1, regex)) {
					substring = regex[i - 1];
				}
				else {
					rst.pop_back();
					substring = regex[i - 2];
					substring += regex[i - 1];
				}
			}
			if (cur == '?') {
				// X? -> (Ɛ|X)
				rst += "(`|";
				rst += substring;
				rst += ")";
			}
			else {
				// X+ -> XX*
				rst += substring + substring + "*";
			}

			// Prevent recursive brackets and operators
			rst += regex.substr(i + 1);
			regex = rst;
			rst = "";
			i = 0;
			continue;
		}
		rst += cur;
		i++;
	}
	return true;
}

void add_dot_splitter(string& regex) {
	string rst;
	char cur;
	for (size_t i = 0; i < regex.length(); i++)
	{
		cur = regex[i];
		rst += cur;
		// Is an operator (including '\\') and not an escape char 
		/*if (i == regex.length() - 1 || OPERATOR_CHARS.find(cur) != string::npos && !is_escape_char(i, regex) || OPERATOR_CHARS.find(regex[i + 1]) != string::npos && regex[i + 1] != '\\' && !is_escape_char(i + 1, regex)) continue;*/
		if (i == regex.length() - 1) continue;
		if (!is_escape_char(i, regex) && (cur == '(' || cur == '|' || cur == '\\')) continue;
		char next = regex[i + 1];
		if (!is_escape_char(i + 1, regex) && (next == '|' || next == '*' || next == ')')) continue;
		rst += '.';
	}
	regex = rst;
}

int operator_precendence(char ch) {
	switch (ch)
	{
	case '*':
		return 3;
		break;
	case '.':
		return 2;
		break;
	case '|':
		return 1;
		break;
	case '(':
		return 0;
		break;
	default:
		return -1;
		break;
	}
}

bool convert_to_suffix_form(string& regex) {
	string rst;
	queue<char> que;
	stack<char> stk;
	char cur;
	for (size_t i = 0; i < regex.length(); i++)
	{
		cur = regex[i];
		if (!is_escape_char(i, regex)) {
			if (cur == '(') stk.push(cur);
			else if (cur == ')') {
				while (!stk.empty() && stk.top() != '(')
				{
					que.push(stk.top());
					stk.pop();
				}
				if (stk.empty())
				{
					cout << "REGEX ERROR: Wrong round brackets in <" << regex << ">" << endl;
					return false;
				}
				else {
					stk.pop();
				}
			}
			else if (cur == '|' || cur == '.' || cur == '*') {
				if (!stk.empty()) {
					while (!stk.empty() && operator_precendence(stk.top()) >= operator_precendence(cur))
					{
						que.push(stk.top());
						stk.pop();
					}
				}
				stk.push(cur);
			}
			else if (cur == '\\') continue; // Ignore because this is part of an escape char.
			else que.push(cur);
		}
		else {
			// Escape char
			que.push(regex[i - 1]);
			que.push(cur);
		}
	}
	while (!stk.empty()) {
		que.push(stk.top());
		stk.pop();
	}
	while (!que.empty()) {
		rst += que.front();
		que.pop();
	}
	regex = rst;
	return true;
}

bool parse_regex(vector<RERule>& rulesVec, const vector<string>& regedTermsVec, unordered_map<string, string>& termsMap)
{
	// Process regular definition in order
	for (auto s : regedTermsVec) {
		if (!process_double_quotation_marks(termsMap[s])) return false;
		if (!process_curly_brackets(termsMap[s], termsMap)) return false;
	}
	
	// Process regular expression rules, watch out for reference passing.
	for (auto &r : rulesVec) {
		if (!process_double_quotation_marks(r.regex)) return false;
		if (!process_curly_brackets(r.regex, termsMap)) return false;
		if (!process_square_brackets(r.regex)) return false;
		process_match_all(r.regex);
		if (!process_special_operators(r.regex)) return false;
		add_dot_splitter(r.regex);
		if (!convert_to_suffix_form(r.regex)) return false;
	}
}

void convert_rules_2_NFA(vector<RERule>& rulesVec, NFA& nfa)
{
	int NFANumber = 0;
	stack<NFA> NFAStk;
	// Temporary end state action
	const vector<string> emptyRuleVec;

	// Convert all the regex rules into separate NFAs
	for (auto rv : rulesVec) {
		string regex = rv.regex;
		char cur;
		for (size_t i = 0; i < regex.length(); i++)
		{
			cur = regex[i];
			if (cur == '\\' && is_escape_char(i + 1, regex)) continue;
			else {
				NFA upNFA, downNFA, toPushNFA;
				NFAState startState, endState;
				int upEnd, downEnd;
				if (!is_escape_char(i, regex) && (cur == '|' || cur == '*' || cur == '.')) {
					switch (cur)
					{
					case '|': {
						// Alternation
						upNFA = NFAStk.top();
						NFAStk.pop();
						downNFA = NFAStk.top();
						NFAStk.pop();
						startState.num = NFANumber++;
						endState.num = NFANumber++;
						// Connect upNFA to new end.
						upEnd = upNFA.finalStatesMap.begin()->first;
						upNFA.statesMap.find(upEnd)->second.transitionTableMap.insert({ EPSILON, endState.num });
						upNFA.finalStatesMap.clear();
						upNFA.finalStatesMap.insert({ endState.num, emptyRuleVec });
						// Connect downNFA to new end.
						downEnd = downNFA.finalStatesMap.begin()->first;
						downNFA.statesMap.find(downEnd)->second.transitionTableMap.insert({ EPSILON, endState.num });
						downNFA.finalStatesMap.clear();
						downNFA.finalStatesMap.insert({ endState.num, emptyRuleVec });
						// Merge into a new NFA
						startState.transitionTableMap.insert({ EPSILON, upNFA.startState });
						startState.transitionTableMap.insert({ EPSILON, downNFA.startState });
						toPushNFA.startState = startState.num;
						toPushNFA.statesMap.insert({ startState.num, startState });
						toPushNFA.statesMap.insert({ endState.num, endState });
						toPushNFA.statesMap.insert(upNFA.statesMap.begin(), upNFA.statesMap.end());
						toPushNFA.statesMap.insert(downNFA.statesMap.begin(), downNFA.statesMap.end());
						toPushNFA.finalStatesMap.insert({ endState.num, emptyRuleVec });
						NFAStk.push(toPushNFA);
					}
							break;
					case '.': {
						// Concatenation (downNFA -> upNFA)
						upNFA = NFAStk.top();
						NFAStk.pop();
						downNFA = NFAStk.top();
						NFAStk.pop();
						downEnd = downNFA.finalStatesMap.begin()->first;
						downNFA.statesMap.find(downEnd)->second.transitionTableMap.insert({ EPSILON, upNFA.startState });
						downNFA.finalStatesMap.clear();
						downNFA.finalStatesMap = upNFA.finalStatesMap;
						downNFA.statesMap.insert(upNFA.statesMap.begin(), upNFA.statesMap.end());
						NFAStk.push(downNFA);
					}
							break;
					case '*': {
						// Closure
						upNFA = NFAStk.top();
						NFAStk.pop();
						startState.num = NFANumber++;
						endState.num = NFANumber++;
						startState.transitionTableMap.insert({ EPSILON, upNFA.startState });
						startState.transitionTableMap.insert({ EPSILON, endState.num });
						upEnd = upNFA.finalStatesMap.begin()->first;
						upNFA.statesMap.find(upEnd)->second.transitionTableMap.insert({ EPSILON, endState.num });
						upNFA.statesMap.find(upEnd)->second.transitionTableMap.insert({ EPSILON, upNFA.startState });
						// Convert to a new NFA
						upNFA.startState = startState.num;
						upNFA.finalStatesMap.clear();
						upNFA.finalStatesMap.insert({ endState.num, emptyRuleVec });
						upNFA.statesMap.insert({ startState.num, startState });
						upNFA.statesMap.insert({ endState.num, endState });
						NFAStk.push(upNFA);
					}
							break;
					default:
						break;
					}
				}
				else {
					if (is_escape_char(i, regex)) cur = convert_escape_char(cur);
					// Normal char (including escape char)
					startState.num = NFANumber++;
					endState.num = NFANumber++;
					startState.transitionTableMap.insert({ cur, endState.num });
					toPushNFA.startState = startState.num;
					toPushNFA.statesMap.insert({ startState.num, startState });
					toPushNFA.statesMap.insert({ endState.num, endState });
					toPushNFA.finalStatesMap.insert({ endState.num, emptyRuleVec });
					NFAStk.push(toPushNFA);
				}
			}
		}
		// Now we can add an actual end state action.
		NFAStk.top().finalStatesMap.begin()->second = rv.actions;
		NFA tmp = NFAStk.top();

		//for (auto m : tmp.finalStatesMap) {
		//	cout << m.first << ":     ";
		//	for (auto a : m.second) {
		//		cout << a << endl;
		//	}
		//}

		//cout << "SS:    " << tmp.startState << endl;
		//for (auto r : tmp.statesMap) {
		//	cout << r.first << ":     ";
		//	for (auto s : r.second.transitionTableMap) {
		//		cout << s.first << " -> " << s.second << "    ";
		//	}
		//	cout << endl;
		//}
		//cout << "-------------------------------------------------------------------------" << endl;
	}

	// Now we can merge all the NFAs into a final NFA

	nfa = NFAStk.top();
	NFAStk.pop();

	NFA currentSubNFA;

	while (!NFAStk.empty())
	{
		currentSubNFA = NFAStk.top();
		NFAStk.pop();

		NFAState startState;
		startState.num = NFANumber++;

		startState.transitionTableMap.insert({ EPSILON, nfa.startState });
		startState.transitionTableMap.insert({ EPSILON, currentSubNFA.startState });

		nfa.startState = startState.num;
		nfa.statesMap.insert({ startState.num, startState });
		nfa.statesMap.insert(currentSubNFA.statesMap.begin(), currentSubNFA.statesMap.end());
		nfa.finalStatesMap.insert(currentSubNFA.finalStatesMap.begin(), currentSubNFA.finalStatesMap.end());
	}
	//cout << NFANumber << endl;
	//for (auto m : nfa.finalStatesMap) {
	//	cout << m.first << ":     ";
	//	for (auto a : m.second) {
	//		cout << a << endl;
	//	}
	//}
}