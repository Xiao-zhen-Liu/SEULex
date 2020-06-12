#include "pch.h"
#include "Common.h"

using namespace std;

const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string& s)
{
	return rtrim(ltrim(s));
}

void split(const string& s, string& left, string& right) {
	bool inBrackets = false;
	bool inMarks = false;
	if (s[0] == '[') {
		inBrackets = true;
		int i = 1;
		for (i = 1; i < s.size(); i++)
		{
			if (s[i] == ']')
			{
				inBrackets = false;
			}
			if ((s[i + 1] == ' ' || s[i + 1] == '\t') && !inBrackets)
			{
				break;
			}
		}
		left = s.substr(0, i + 1);
		right = s.substr(i + 1);
		right = trim(right);
	}
	else if (s[0] == '\"') {
		inMarks = true;
		int i = 1;
		for (i = 1; i < s.size(); i++)
		{
			if (s[i] == '\"')
			{
				inMarks = false;
			}
			if ((s[i + 1] == ' ' || s[i + 1] == '\t') && !inMarks)
			{
				break;
			}
		}
		left = s.substr(0, i + 1);
		right = s.substr(i + 1);
		right = trim(right);
	}
	else {
		auto i = s.find_first_of(WHITESPACE);
		left = s.substr(0, i);
		right = s.substr(i + 1);
		right = trim(right);
	}
}

void nextLine(ifstream& inStm, string& lineBuf, int& lineNum, string& tempLine, bool& error) {
	getline(inStm, lineBuf), lineNum++;

	// Preprocessing: check for comment, if any, delete it.

	if (lineBuf.find(" /*") != string::npos || lineBuf.find("\t/*") != string::npos) {
		if (lineBuf.find("*/") == string::npos) {
			tempLine = lineBuf;
			while (tempLine.find("*/") == string::npos && !inStm.eof()) {
				getline(inStm, tempLine), lineNum++;
				lineBuf += tempLine;
			}
		}
		if (!inStm.eof()) {
			auto comBegin = lineBuf.find(" /*");
			if (comBegin == string:: npos) comBegin = lineBuf.find("\t/*");
			auto comEnd = lineBuf.find("*/") + 1;
			lineBuf = lineBuf.erase(comBegin, comEnd - comBegin + 1); // MAY HAVE A PROBLEM! NOT SURE!
		}
		else {
			cout << "ERROR: Comment not finished." << endl;
			error = true;
		}
	}

	// Trim white spaces.

	lineBuf = rtrim(lineBuf);
}

bool read_parse_lex_file(string path, vector<string>& regedTermsVec, unordered_map<string, string>& regdMap, vector<RERule>& regexRulesVec, string& codeBegin, string& codeEnd)
{
	ifstream inStm;
	inStm.open(path);
	if (!inStm) {
		cout << "File <" << path << "> NOT FOUND. FAILED." << endl;
		return false;
	}

	string lineBuf, tempLine, leftPart, rightPart, regex;
	int lineNum = 0;
	bool error = false;

	enum LexFileState{BEGIN, CONSTANT_DECLARATIONS, REGULAR_DEFINITIONS, TRANSITION_RULES, AUXILIARY_FUNCTIONS};

	LexFileState state = BEGIN;

	vector<string> actions;

	while (!inStm.eof() && !error) {
		if (state == CONSTANT_DECLARATIONS || state == AUXILIARY_FUNCTIONS) {
			getline(inStm, lineBuf), lineNum++;
		}
		else {
			nextLine(inStm, lineBuf, lineNum, tempLine, error);
			if (lineBuf.empty()) continue;
		}

		switch (state)
		{
		case BEGIN:
			if (lineBuf.compare("%{") == 0) state = CONSTANT_DECLARATIONS;
			else {
				cout << "ERROR: No entry token <%{> at file start." << endl;
				error = true;
			}
			break;
		case CONSTANT_DECLARATIONS:
			if (lineBuf.compare("%}") == 0) state = REGULAR_DEFINITIONS;
			else if (inStm.eof()) {
				cout << "ERROR: No exit token <%}> after constant declarations." << endl;
				error = true;
			}
			else {
				codeBegin += lineBuf + "\n";
			}
			break;
		case REGULAR_DEFINITIONS:
			if (lineBuf.compare("%%") == 0) state = TRANSITION_RULES;
			else {
				split(lineBuf, leftPart, rightPart);
				regdMap[leftPart] = rightPart;
				regedTermsVec.push_back(leftPart);
			}
			break;
		case TRANSITION_RULES:
			if (lineBuf.compare("%%") == 0) state = AUXILIARY_FUNCTIONS;
			else {
				lineBuf = trim(lineBuf);
				split(lineBuf, leftPart, rightPart);
				regex = leftPart;
				if (rightPart.compare("{") == 0 ) {
					// Only allow content in a new line.
					nextLine(inStm, lineBuf, lineNum, tempLine, error), lineBuf = trim(lineBuf);
					while (lineBuf.compare("}") != 0  && !inStm.eof()) {
						actions.push_back(lineBuf);
						nextLine(inStm, lineBuf, lineNum, tempLine, error);
						lineBuf = trim(lineBuf);
					}
					if (inStm.eof()) {
						cout << "ERROR: Incomplete regular expression - action sequence format (missing '}')." << endl;
						error = true;
					}
					else {
						regexRulesVec.push_back(RERule{ regex, actions });
						actions.clear();
					}
				}
				else {
					actions.push_back(rightPart);
					regexRulesVec.push_back(RERule{ regex, actions });
					actions.clear();
				}
			}
			break;
		case AUXILIARY_FUNCTIONS:
			codeEnd += lineBuf + "\n";
			break;
		default:
			break;
		}
	}
	if (error) {
		cout << "at: Line " << lineNum << endl;
		return false;
	}
	inStm.close();
	return true;
}