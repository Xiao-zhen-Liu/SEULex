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

/*
通过子集构造法将NFA转化为DFA：

输入：NFA
输出：DFA
方法：
	一开始，e-closure（s0）是DFAstate中的唯一状态，且它未加标记（标记表示未处理的）
	while (在DFAstate中有一个未标记状态T)（这里不太方便，把标记换成未处理队列吧
	{
		给T加上标记；
		for (每个输入符号a)
		{
			U = e-closure（move（T，a））；
			if（U不在DFAstate中）
				将U加入到DFAstate中，且不加标记；
			DFA[T,a]=U；
		}
	}
*/

static int NFA_state_num = 0;

//e_closure(T)：能够从T中的某个NFA状态s开始只通过e转换到达的NFA状态集合
/*算法：
将T的所有状态压入stack；
将e_closure(T)初始化为T；
while(stack非空)
{
	将栈顶元素t弹出；
	for（每个满足如下条件的u：从t出发有一个标号为e的转换到达状态u）
		if（u不在e_closure（T）中）
		{
			将u加入到e_closure(T)中；
			将u压入栈中
		}
}
*/
void e_closure(unordered_set<int>& NFAStatesSet, const unordered_map<int, NFAState>& statesMap)//参数1：e_closure(T)（初始时就是T），参数2：T中所有NFA状态
{
	stack<int> stack;
	vector<bool> flag(NFA_state_num);//用于区分是否处理过该状态

	//将T中所有状态入栈
	for (const auto& s : NFAStatesSet)
	{
		stack.push(s);
		flag[s] = true;
	}

	while (!stack.empty())//stack非空
	{
		auto bounds = statesMap.find(stack.top())->second.transitionTableMap.equal_range('`');//`代表epsilon
		//在statesMap中，找到本次循环处理的那个NFA的状态标号，获取对应的NFAstate；再找这个状态发出的epsilon边，由于这是NFA，所以可能会有多个匹配项，我们要一个一个来查，用equal_range返回一个头尾的范围
		stack.pop();//弹出栈顶元素
		auto start = bounds.first;
		auto end = bounds.second;
		while (start != end) 
		{
			int new_item_num = (*start).second;
			if (flag[new_item_num]) continue;//已经处理过这个结点了，直接跳过
			else//u不在e_closure（T）中，需要在DFA中加入新的NFA结点
			{
				stack.push(new_item_num);
				NFAStatesSet.emplace(new_item_num);
				start += 1;
			}
		}
	}
}

//move(T,a）：能够从T中的某个状态s出发、通过标号为a的转换到达的NFA状态的集合
bool move(const unordered_set<int>& NFAStatesSet, const unordered_map<int, NFAState>& NFAStatesMap, unordered_set<int>& subset, const char inputC)//参数1：待处理的DFA，参数2：T中所有NFA状态，参数3：放结果的，参数4：待查询的输入字符
{
	bool flag = false;//有没有这条边
	for (const auto& state : NFAStatesSet) //NFAStatesSet是指DFA每个状态中所包含的NFA状态标号的集合
	{
		auto bounds = NFAStatesMap.find(state)->second.transitionTableMap.equal_range(inputC);//在NFAStatesMap中，找到本次循环处理的那个NFA的状态标号，获取对应的NFAstate；再找这个状态发出的边，由于这是NFA，所以可能会有多个匹配项，我们要一个一个来查，用equal_range返回一个头尾的范围
		auto start = bounds.first;
		auto end = bounds.second;
		//这个状态没有这条发出边，直接跳过（flag为false
		if (start == end) continue;
		//这个状态有这条发出边
		while (start != end)
		{
			subset.insert(start->second);
			++start;
		}
		flag = true;
	}
	return flag;
}

bool find_actions(const unordered_set<int>& NFAStatesSet, unordered_map<int, vector<string>>& finalStatesMap_NFA, string& actions)//参数1：DFA状态中的NFA状态；参数2：NFA中的终态<终态标号，对应的动作>；参数3：动作
{
	decltype(finalStatesMap_NFA.find(0)) endState_Iter;//最终选择的终态
	bool find = false;//是否已经在这个DFA结点中找到了终态
	for (const auto& NFAState : NFAStatesSet)//遍历DFA中所有的NFA结点的状态
	{
		auto Iter = finalStatesMap_NFA.find(NFAState);//查找一下这个NFA状态是不是一个终态（用键值查找，如果成功则返回对应的迭代器，否则返回unordered_map::end
		if (Iter != finalStatesMap_NFA.end())//能找到，说明该DFA结点是终态
		{
			if (find) 
			{
				if (endState_Iter->first > Iter->first)//如果有位置更靠前的，更新终态的选择
					endState_Iter = Iter;
			}
			else 
			{
				endState_Iter = Iter;
				find = true;
			}
		}
	}
	if (find)//确定终态
	{
		actions = endState_Iter->second;//动作的选择
		return true;
	}
	else return false;
}

//子集构造法的实现
void convert_NFA_2_DFA(const NFA &nfa,DFA &dfa)
{	
	int Dcounter = 0;//给DFA状态编号用的
	NFA_state_num = nfa.statesMap.size();
	//int c = 1;
	const string inputSet(ALL_CHARS);//所有的输入符号

	//一开始，e - closure（s0）是DFAstate中的唯一状态，且它未加标记
	DFAState s0;//s0
	s0.num = Dcounter;//给DFA初始状态s0编号
	Dcounter += 1;
	//定义初始结点s0
	s0.NFAStatesSet.insert(nfa.startState);
	e_closure(s0.NFAStatesSet, nfa.statesMap);
	//将s0加入DFA中
	dfa.statesMap.insert(pair<int, DFAState>(s0.num,s0));

	queue<int> unmarked_DFAStates;//未加标记的DFA状态的编号
	unmarked_DFAStates.push(s0.num);

	//遍历DFA中未处理的结点，遍历所有输入，调用函数查找该结点在NFA中能通过这个输入到达的结点，
	//将这些能到达的结点加入DFA的新节点中
	while (!unmarked_DFAStates.empty())//在DFAstate中有一个未标记状态T
	{
		//取出一个要处理的状态的编号（相当于加上了标记
		int this_state = unmarked_DFAStates.front();//指向当前状态的指针
		unmarked_DFAStates.pop();
		//遍历输入集中的所有输入字符
		for (const char& inputC : inputSet)
		{
			unordered_set<int> U;
			//如果有这条边
			if (move(dfa.statesMap[this_state].NFAStatesSet, nfa.statesMap, U, inputC))//先求move(T,a)
			{
				e_closure(U, nfa.statesMap);//再求move(T,a)的e_closure

				int next_state;//通过标号为a的转换能到达的状态的编号

				//检查U在不在DFAstate中
				bool in = false;
				for (const auto& s : dfa.statesMap)//遍历DFA的所有结点 
				{
					if (s.second.NFAStatesSet == U)//如果U已经在DFAstate中
					{
						next_state = s.first; //指向当前这个已有的结点
						in = true;//U在DFAstate中
						break;
					}
				}

				//如果U不在DFAstate中，将U加入到DFAstate中(即创建新结点)，且不加标记
				if (!in)
				{
					DFAState newState;
					newState.num = Dcounter++;//新的标号
					next_state = newState.num; //指向新建的这个结点
					newState.NFAStatesSet = U;//U是这个DFA新结点的状态内容
					dfa.statesMap.insert(pair<int, DFAState>(newState.num, newState));
					unmarked_DFAStates.push(newState.num);//新结点等待处理
					//判断是否是终态并找确定终态对应动作
					string actions;
					if (find_actions(newState.NFAStatesSet, nfa.finalStatesMap, actions))//如果包含一个终态
					{ 
						dfa.finalStatesMap.insert(pair<int, string>(newState.num, actions));//决定这个dfa终态对应的动作
					}
				}
				dfa.statesMap[this_state].transitionTableMap.insert(pair<char, int>(inputC, next_state));//DFA图上加一条边
			}
		}
	}
}
