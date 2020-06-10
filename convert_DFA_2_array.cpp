#include "Common.h"
#include "pch.h"

using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::queue;
using std::stack;
using std::vector;
using std::string;
using std::cout;
using std::endl;


//extern vector<RERule> regexRulesVec;

void convert_DFA_2_array(const DFA& dfa, vector<pair<int*, int>>& arrays, vector<vector<string>>& endVec)
{
	string allset(ALL_CHARS);
	const int set_size = allset.size();

	//建立四个表：

	//1、ec表：该表用于将字符的ascii码映射为DFA状态转化表中的列数。
	//索引为char的ascii码，值是对应的列号。
	int* ec = new int[256];//建表
	memset(ec, 0, 256 * sizeof(int));
	//从下标为1开始给char标值，0代表非法字符
	for (int i = 0; i < set_size; ++i)
	{
		ec[int(allset[i])] = i + 1;//录入所有可能输入的字符
	}
	arrays.push_back(pair<int*, int>(ec, 256));

	//2、base表：索引是状态序列，值是行数*宽度。
	//将状态对应的行的首地址映射到一维数组中的具体位置。其索引是状态序列，值是状态序号*字符列宽度。
	const int DFA_size = dfa.statesMap.size();
	int* base = new int[DFA_size];
	memset(base, 0, DFA_size * sizeof(int));

	//3、next表：索引是base+ec。值是下一个跳转状态。
	//表的大小是base*ec
	int sizeOfNext = DFA_size * (set_size + 1);
	int* next = new int[sizeOfNext];//set从1开始计数
	//初始化
	for (int i = 0; i < sizeOfNext; i++)
	{
		next[i] = -1;
	}
	vector<DFAState> statesVec;
	for (int i = 0; i < dfa.statesMap.size(); i++)
	{
		statesVec.push_back(dfa.statesMap.at(i));
	}

	for (int i = 0; i < dfa.statesMap.size(); i++)
	{
		base[statesVec[i].num] = i * (set_size + 1);
		for (auto it = statesVec[i].transitionTableMap.begin(); it != statesVec[i].transitionTableMap.end(); it++) 
		{
			next[base[statesVec[i].num] + ec[int(it->first)]] = it->second;//用base+ec作为索引查询下一个跳转状态的标号
		}
	}
	arrays.push_back(pair<int*, int>(base, DFA_size));
	arrays.push_back(pair<int*, int>(next, sizeOfNext));

	//4、accept表：索引是终态状态号，值是对应的序列号。
	int* accept = new int[DFA_size];//建表
	memset(accept, 0, DFA_size * sizeof(int));

	int numOfend = 0;//计数
	for (auto it = dfa.finalStatesMap.begin(); it != dfa.finalStatesMap.end(); ++it)
	{
		accept[it->first] = numOfend + 1;
		//endVec.push_back(regexRulesVec[it->second]);
		endVec.push_back(it->second);
		++numOfend;//计数
	}

	arrays.push_back(pair<int*, int>(accept, DFA_size));
}