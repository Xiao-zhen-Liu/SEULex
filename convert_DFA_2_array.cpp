#include "Common.h"
#include <queue>
#include <vector>
#include <stack>
#include <algorithm>

using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::queue;
using std::stack;
using std::vector;
using std::string;
using std::cout;
using std::endl;


extern vector<RERule> rules;

void convert_DFA_2_array(const DFA& dfa, vector<pair<int*, int>>& arrays, vector<RERule>& endVec)
{
	string allset(ALL_CHARS);
	const int set_size = allset.size();

	//建立四个表：
	//1、ec表：索引char的ascii码，值是对应的列数
	int* ec = new int[256];
	memset(ec, 0, 256 * sizeof(int));
	//从下标为1开始给char标值，0代表非法字符
	for (int i = 0; i < set_size; ++i)
	{
		ec[int(allset[i])] = i + 1;
	}
	arrays.push_back(pair<int*, int>(ec, 256));

	//2、base表：索引是状态序列，值是行数*宽度。
	const int DFA_size = dfa.statesMap.size();
	int* base = new int[DFA_size];
	memset(base, 0, DFA_size * sizeof(int));

	//3、next表：索引是base+ec。值是下一个跳转状态。大小是base*ec
	int sizeOfNext = DFA_size * (set_size + 1);
	int* next = new int[sizeOfNext];//set从1开始计数
	//初始化
	for (int i = 0; i < sizeOfNext; i++)
	{
		next[i] = -1;
	}
	vector<DFA> statesVec;
	for (int i = 0; i < dfa.statesMap.size(); i++)
	{
		statesVec.push_back(dfa.statesMap[i]);
	}
	for (int i = 0; i < dfa.statesMap.size(); i++)
	{
		base[dfa.statesVec[i].number] = i * (set_size + 1);
		for (auto it = dfa.statesVec[i].exEdgesMap.begin(); it != dfa.statesVec[i].exEdgesMap.end(); it++) {
			next[base[dfa.statesVec[i].number] + ec[int(it->first)]] = it->second;
		}
	}
	arrays.push_back(pair<int*, int>(base, DFA_size));
	arrays.push_back(pair<int*, int>(next, sizeOfNext));

	//4、accept表：索引是终态状态号，值是对应的序列号。
	int* accept = new int[DFA_size];
	memset(accept, 0, DFA_size * sizeof(int));

	int numOfend = 0;
	for (auto it = dfa.endStatesMap.begin(); it != dfa.endStatesMap.end(); ++it)
	{
		accept[it->first] = numOfend + 1;
		endVec.push_back(rules[it->second]);
		++numOfend;
	}

	arrays.push_back(pair<int*, int>(accept, DFA_size));

}