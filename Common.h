#pragma once
#ifndef COMMON_H
#define COMMON_H

#define LEX_TEST 0
#define YACC_TEST 1

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
	std::unordered_multimap<char, int> transitionTableMap;//<边上的值，边指向的结点标号>
};

//struct NFA
//{
//	int startState;
//	unordered_map<int, RERule> finalStatesMap;
//	unordered_map<int, NFAState> statesMap;
//};

struct NFA
{
	int startState;
	std::unordered_map<int, std::vector<std::string>> finalStatesMap;//<终态标号，对应的动作>
	std::unordered_map<int, NFAState> statesMap;//NFA中结点的标号与其state的对应关系
};

struct DFAState
{
	int num;
	std::unordered_map<char, int> transitionTableMap;//<边上的值，边指向的结点标号>
	std::unordered_set<int> NFAStatesSet;
};

//struct DFA
//{
//	int startState;
//	unordered_map<int, RERule> finalStatesMap;
//	unordered_map<int, DFAState> statesMap;
//};

struct DFA
{
	int startState;
	std::unordered_map<int, std::vector<std::string>> finalStatesMap;//存储终态和对应的动作
	std::unordered_map<int, DFAState> statesMap;//DFA中结点的标号与其state的对应关系
};

const std::string OPERATOR_CHARS("\\[]^-?.*+|()$/{}%<>");

const std::string ALL_CHARS("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#%'()*+,-./:;<=>\?[\\]^{|}_ \n\t\v\f~&");


#endif // !COMMON_H
