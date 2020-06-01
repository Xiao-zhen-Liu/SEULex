#pragma once
#ifndef COMMON_H
#define COMMON_H

#include "pch.h"

constexpr auto EPSILON = '`';

struct REDefinition
{
	std::string name;
	std::string regex;
};

struct RERule
{
	std::string regex; // Regular Expression
	std::vector<std::string> actions; // Action(s) associated with it
};

struct NFAState
{
	int num;
	std::unordered_multimap<char, int> transitionTableMap;
};

struct NFA
{
	int startState;
	std::unordered_map<int, std::vector<std::string>> finalStatesMap;
	std::unordered_map<int, NFAState> statesMap;
};

struct DFAState
{
	int num;
	std::unordered_map<char, int> transitionTableMap;
	std::unordered_set<int> NFAStatesSet;
};

struct DFA
{
	int startState;
	std::unordered_map<int, std::vector<std::string>> finalStatesMap;
	std::unordered_map<int, DFAState> statesMap;
};

const std::string OPERATOR_CHARS("\\[]^-?.*+|()$/{}%<>");

const std::string ALL_CHARS("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#%'()*+,-./:;<=>\?[\\]^{|}_ \n\t\v\f~&");


#endif // !COMMON_H