#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "pch.h"
using namespace std;

struct RERule
{
	string regex; // Regular Expression
	vector<string> actions; // Action(s) associated with it
};

struct NFAState
{
	int num;
	unordered_multimap<char, int> transitionTableMap;
};

struct NFA
{
	int startState;
	unordered_map<int, RERule> finalStatesMap;
	unordered_map<int, NFAState> statesMap;
};

struct DFAState
{
	int num;
	unordered_map<char, int> transitionTableMap;
	unordered_set<int> NFAStatesSet;
};

struct DFA
{
	int startState;
	unordered_map<int, RERule> finalStatesMap;
	unordered_map<int, DFAState> statesMap;
};

#endif // !STRUCTURES_H
