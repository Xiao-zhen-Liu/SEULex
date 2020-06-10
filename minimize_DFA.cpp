#include "Common.h"
#include "pch.h"

using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::queue;
using std::stack;
using std::vector;
using std::string;

static bool scan(const vector<DFAState>&);
static void split_to_sets(const DFA& dfa);

static unordered_map<int, int> statesSetsMap;//新旧状态编号的对应关系，<原来的标号，现在的标号>
static vector<unordered_set<int>> statesSetsVec;//所有的划分（每个vec是一个划分，每个vec里是划分中的state
static int count = 0;

/*
DFA最小化算法一：

输入：一个DFA D，其状态集合为S。输入字母表为A，开始状态为s0，接受状态集为F
输出：一个DFA D'
方法：
	(1)首先构造包含两个组F和S-F的初始化分pi，这两个组分别是D的接受状态组和非接受状态组。
	(2)
	令pi_new=pi；
	for(pi中的每个组G)
	{
		将G划分为更小的组，使得两个状态s和t在同一个小组中当且仅当对于所有的输入符号a，状态s和t在a上的转换都达到pi中的同一组；
		在pi_new中将G替换为对G进行分划得到的那些小组；
	}
	(3)如果pi_new=pi，令pi_final=pi并接着执行步骤4；否则用pi_new替换pi并重复步骤2。
	(4)在划分pi_final的每个组中选取一个状态作为该组的代表。这些代表构成了最小DFA D'的状态。D'的其他部分按如下步骤构建：
		a.D'的开始状态是包含了D的开始状态的组的代表。
		b.D'的接受状态是那些包含了D的接受状态的组的代表。
		c.令s是pi_final中某个组G的代表，并令DFA D中在输入a上离开s的转换到达状态t。令r为t所在组H的代表。
		那么在D'中存在一个从s刀r在输入a上的转换。（注意，在D中，组G中的每一个状态必然在输入a上进入组H中的某个状态，
		否则，组G应该已经被步骤2分割成更小的组了。


DFA最小化算法二：
(1)遍历原DFA，将每个终态单独存入一个集合中，其余非终态放入一个集合
(2)直至没有新建集合为止，遍历所有集合：
	a.从集合k中任意取一个状态s，遍历其余状态s’
		i.遍历s所有发出边e
		ii.如果s’中也有e且与s到同一个集合，跳至下一个
		iii.否则新建集合k’，将s’并入k’中
	b.如果新建集合了，就终止循环
	c.将k’中元素从k中删除

*/

void minimize_DFA(const DFA& DFA_origin, DFA& DFA_minimum) 
{
	split_to_sets(DFA_origin);//划分

	//设置初态
	DFA_minimum.startState = statesSetsMap.find(DFA_origin.startState)->second;
	//设置终态
	for (const auto& p : DFA_origin.finalStatesMap)
	{
		DFA_minimum.finalStatesMap.emplace(statesSetsMap.find(p.first)->second, p.second);
	}
	//设置其他状态
	vector<DFAState> statesVec;
	for (int k = 0; k < statesSetsVec.size(); ++k)
	{
		for (int i = 0; i < DFA_origin.statesMap.size(); i++)
		{
			statesVec.push_back(DFA_origin.statesMap.find(i)->second);
		}
		auto& pivotState = statesVec[*statesSetsVec[k].cbegin()];
		DFAState newState;
		newState.num = k;
		for (const auto& p : pivotState.transitionTableMap)
		{
			newState.transitionTableMap.emplace(p.first, statesSetsMap.find(p.second)->second);
		}
		DFA_minimum.statesMap.insert({ newState.num,newState });
	}
}

void split_to_sets(const DFA& dfa)//生成最小化后的状态集合
{
	auto& finalStatesMap = dfa.finalStatesMap;
	//auto& statesMap = dfa.statesMap;

	vector<DFAState> statesMap;
	for (const auto& s : dfa.statesMap)
	{
		statesMap.push_back(s.second);
	}


	for (const auto& p : finalStatesMap)//遍历终态，收集终态
	{
		statesSetsVec.push_back(unordered_set<int>{p.first});//将状态标号入栈，每个终态单独放
		statesSetsMap.emplace(p.first, count++);//记录状态标号+第几个
	}

	unordered_set<int> tempSet;
	for (const auto& e : statesMap)//收集非终态
	{
		if (finalStatesMap.find(e.num) == finalStatesMap.end())//如果当前状态是非终态
		{
			tempSet.insert(e.num);//非终态的状态标号放在一起
			statesSetsMap.emplace(e.num, count);
		}
	}

	statesSetsVec.push_back(tempSet);//划分的集合
	++count;

	while (scan(statesMap));//不停的扫描直到不再变化为止
}


bool scan(const vector<DFAState>& statesVec)//扫描整个二叉树，分裂一个不满足等价要求的集合（一次只分裂一个集合
{
	bool flag = false;//是否可以再分割
	int splitSetNumber = 0;
	unordered_set<int> newSet;
	for (int k = 0; k < statesSetsVec.size(); ++k)//遍历所有划分，处理一个集合
	{
		auto& state = statesSetsVec[k];//取出一个状态
		if (state.size() == 1) continue;//跳过已经只有单独状态的划分
		else 
		{
			auto& standard = statesVec[*(state.begin())];//找一个作为基准
			for (const auto& edge : ALL_CHARS)//遍历基准状态的所有边 
			{
				for (const auto& i : state)//检查集合每一个状态 
				{
					const auto& state = statesVec[i];
					//判断是否具有相同的边数和转移状态
					auto findStateIt = state.transitionTableMap.find(edge);//在state中找出这条边
					auto findStandardIt = standard.transitionTableMap.find(edge);//在standard中找出这条边

					//三种需要分开的情况
					if (findStateIt == state.transitionTableMap.end() && findStandardIt != standard.transitionTableMap.end())//standard中的边找不到 
					{
						flag = true;
						newSet.insert(i);
					}
					else if ((findStateIt != state.transitionTableMap.end()) && (findStandardIt == standard.transitionTableMap.end()))//state中的边找不到 
					{
						flag = true;
						newSet.insert(i);
					}
					else if (*(statesSetsMap.find(findStandardIt->second)) != *(statesSetsMap.find(findStateIt->second)))//边都找到了但是转移到的状态不同
					{
						flag = true;
						newSet.insert(i);
					}
				}
				if (flag) break; //一旦发现可以分割就停
			}
			if (flag)//分开一个状态就停
			{
				splitSetNumber = k;//记录哪个划分需要被分割
				break;
			}
		}
	}
	//新的划分：加入一个新的划分（小），把该划分（小）中的状态从原先的k号划分（大）中删除
	for (const auto& s : newSet)
	{
		statesSetsVec[splitSetNumber].erase(s);
		statesSetsMap.insert_or_assign(s, count);
	}

	statesSetsVec.push_back(newSet);
	++count;

	return flag;
}