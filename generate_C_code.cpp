#include "Common.h"
#include "pch.h"

using namespace std;

//打印数组，name为数组名，size为数组大小，value为数组值，out为写入的文件流
static void print_array(string name, int size, const int* value, ofstream& out);

struct Token
{
	string token_type;
	string token_value;
};

//生成.c文件,arrays为包含多个相关数组的容器，endVec为终态对应的动作
int generate_C_code(vector<pair<int*, int>>& arrays, vector<vector<string>>& endVec, string& codeBegin, string& codeEnd, int startState, int mode)
{
	ofstream out;
	if (mode == 0)//lex mode
	{
		out.open("lex.c", ios::out);
	}
	if (mode == 1)//yacc mode
	{
		out.open("lex.h", ios::out);
	}
	//首先判断size的大小是否为4
	if (arrays.size() != 4)
	{
		return -1;
	}
	//主函数的开始
	out << "#define _CRT_SECURE_NO_WARNINGS" << endl;
	out << "#define START_STATE " << startState << endl;
	//out << "#include\"stdio.h\"" << endl;
	out << "#include \"stdlib.h\"" << endl;
	out << "#include<string.h>" << endl;
	if (mode == YACC_TEST)
	{
		out << "#include <string>" << endl;
		out << "#include <vector>" << endl;
		out << "using namespace std;" << endl;
	}
	out << codeBegin;
	
	//函数声明
	out << "char* getCharPtr(const char* fileName);" << endl;
	out << "int findAction(int action);" << endl; 

	//依次输出ec表,base表,next表,accept表
	vector<string> array_name;
	array_name.push_back(string("yy_ec"));
	array_name.push_back(string("yy_base"));
	array_name.push_back(string("yy_next"));
	array_name.push_back(string("yy_accept"));

	for (int i = 0; i < 4; i++)
	{
		print_array(array_name[i], arrays[i].second, arrays[i].first, out);
		out << endl;
	}

	if (mode == YACC_TEST)
	{
		//定义数据结构
		out << "struct Token" << endl;
		out << "{" << endl;
		out << "	string token_type;" << endl;
		out << "	string token_value;" << endl;
		out << "};" << endl;
	}

	//定义变量
	out << "int yy_current_state = START_STATE;" << endl;
	out << "int yy_last_accepting_state = -1;" << endl;
	out << "char *yy_cp = NULL;" << endl;
	out << "char *yy_last_accepting_cpos = NULL;" << endl;
	out << "int yy_act = 0;" << endl;
	out << "int isEnd = 0;" << endl;
	out << "int yy_c = -1;" << endl;
	out << "int correct = 1;" << endl;
	out << "int flag_mlc = 0;" << endl;

	if (mode == YACC_TEST)
	{
		out << "vector <Token> tokens;" << endl;
		out << "string str_temp = \"\";" << endl;
	}

	out << endl;

	//初始化
	out << "void lex_init(const char* fileName)" << endl;
	out << "{" << endl;
	out << "	yy_cp = getCharPtr(fileName);" << endl;//调用char* getCharPtr(char* fileName)得到文件字符指针
	out << "}" << endl;
	out << endl;

	if (mode == YACC_TEST)
	{
		out << "vector<Token> yy_lex(const char* fileName)" << endl;
		out << "{" << endl;
	}

	if (mode == LEX_TEST)
	{
		out << "int main( int argc, char** argv )" << endl;
		out << "{" << endl;
		out << "	if( argc == 2 )" << endl;
		out << "	{" << endl;
		out << "		lex_init(argv[1]);" << endl;
		out << "	}" << endl;
		out << "	else" << endl;
		out << "	{" << endl;
		out << "		printf(\"ERROR: invalid argument!\\n\");" << endl;
		out << "		return -1;" << endl;
		out << "	}" << endl;
		out << endl;

		out << "	if (isEnd && correct)" << endl;
		out << "	{" << endl;
		out << "		return -1;" << endl;
		out << "	}" << endl;
		out << "	else if (isEnd && !correct)" << endl;
		out << "	{" << endl;
		out << "		return -2;" << endl;
		out << "	}" << endl;
		out << endl;
	}
	if (mode == YACC_TEST)
	{
		out << "	lex_init(fileName);" << endl << endl;
		out << "	if (isEnd && correct)" << endl;
		out << "	{" << endl;
		out << "		return tokens;" << endl;
		out << "	}" << endl;
		out << "	else if (isEnd && !correct)" << endl;
		out << "	{" << endl;
		out << "		return tokens;" << endl;
		out << "	}" << endl;
		out << endl;
	}

	out << "	int result = 0;" << endl;
	out << "	while (*yy_cp != 0)" << endl;
	out << "	{" << endl;
	out << "		yy_c = yy_ec[(int)*yy_cp];" << endl;
	if (mode == YACC_TEST)
	{
		out << "		str_temp = str_temp + *(yy_cp);" << endl;
	}
	out << "		if (yy_accept[yy_current_state])" << endl;
	out << "		{" << endl;
	out << "			yy_last_accepting_state = yy_current_state;" << endl;
	out << "			yy_last_accepting_cpos = yy_cp;" << endl;
	//out << "			str_temp = str_temp + *(yy_cp);" << endl;
	out << "		}" << endl;
	out << "		if (yy_next[yy_base[yy_current_state] + yy_c] == -1 && yy_last_accepting_state != -1)" << endl;
	out << "		{" << endl;
	out << "			yy_current_state = yy_last_accepting_state;" << endl;
	out << "			yy_cp = yy_last_accepting_cpos;" << endl;
	out << "			yy_act = yy_accept[yy_current_state];" << endl;
	out << "			if (flag_mlc == 1)" << endl;
	out << "			{" << endl;
	out << "				flag_mlc = 0;" << endl;
	out << "				yy_current_state = START_STATE;" << endl;
	out << "				yy_last_accepting_state = -1;" << endl;
	out << "				++yy_cp;" << endl;
	out << "				yy_current_state = yy_next[yy_base[yy_current_state] + yy_c];" << endl;
	out << "				continue;" << endl;
	out << "			}" << endl;
	out << "			result = findAction(yy_act);" << endl;

	if (mode == YACC_TEST)
	{
		out << "			if (result != -1)" << endl;
		out << "			{" << endl;
		out << "				yy_current_state = START_STATE;" << endl;
		out << "				yy_last_accepting_state = -1;" << endl;
		out << "				++yy_cp;" << endl;
		out << "				yy_current_state = yy_next[yy_base[yy_current_state] + yy_c];" << endl;
		out << "				break;" << endl;
		out << "			}" << endl;
		out << "			if (result == -1)" << endl;
		out << "			{" << endl;
		out << "				yy_current_state = START_STATE;" << endl;
		out << "				yy_last_accepting_state = -1;" << endl;
		out << "				++yy_cp;" << endl;
		out << "				yy_current_state = yy_next[yy_base[yy_current_state] + yy_c];" << endl;
		out << "				continue;" << endl;
		out << "			}" << endl;
	}
	else if (mode == LEX_TEST)
	{
		out << "			printf(\" \");" << endl;
		out << "			yy_current_state = START_STATE;" << endl;
		out << "			yy_last_accepting_state = -1;" << endl;
		out << "			++yy_cp;" << endl;
		out << "			yy_current_state = yy_next[yy_base[yy_current_state] + yy_c];" << endl;
		out << "			continue;" << endl;

	}

	out << "		}" << endl;
	out << "		if (yy_next[yy_base[yy_current_state] + yy_c] == -1 && yy_last_accepting_state == -1)" << endl;
	out << "		{" << endl;
	out << "			printf(\"ERROR DETECTED IN INPUT FILE !\");" << endl;

	if (mode == LEX_TEST)
	{
		out << "			return -1;" << endl;
	}
	out << "		}" << endl;
	out << "		if (yy_next[yy_base[yy_current_state] + yy_c] != -1) " << endl;
	out << "		{" << endl;
	out << "			yy_current_state = yy_next[yy_base[yy_current_state] + yy_c];" << endl;
	out << "			++yy_cp;" << endl;
	out << "		}" << endl;
	out << "	}" << endl;
	out << endl;
	out << "	if (*yy_cp == 0)" << endl;
	out << "	{" << endl;
	out << "		isEnd = 1;" << endl;
	out << "		if (yy_accept[yy_current_state] && yy_cp == yy_last_accepting_cpos + 1)" << endl;
	out << "		{" << endl;
	out << "			yy_act = yy_accept[yy_current_state];" << endl;
	if (mode == YACC_TEST)
	{
		out << "			str_temp += *(yy_cp - 1);" << endl;
	}
	out << "			result = findAction(yy_act);" << endl;
	out << "		}" << endl;
	out << "		else " << endl;
	out << "		{" << endl;
	out << "			printf(\"ERROR DETECTED IN INPUT FILE !\");" << endl;
	out << "			correct = 0;" << endl;

	if (mode == LEX_TEST)
	{
		out << "			return -1;" << endl;
	}

	out << "		}" << endl;
	out << "	}" << endl;

	if (mode == LEX_TEST)//lex
	{
		out << "	return 0;" << endl;
	}
	else //yacc
	{
		out << "	return tokens;" << endl;
	}
	out << "}" << endl;
	out << endl;
	//lex_mian函数结束


	//int findAction(int action)函数
	out << "int findAction(int action)" << endl;
	out << "{" << endl;

	if (mode == LEX_TEST)
	{
		out << "	switch (action) " << endl;//根据endVec打印switch语句
		out << "	{" << endl;
		out << "		case 0:" << endl;
		out << "		break;" << endl;
		for (int i = 0; i < endVec.size(); i++)
		{
			out << "		case " << i + 1 << ":" << endl;
			for (int j = 0; j < endVec[i].size(); j++)
			{
				out << "		" << endVec[i][j] << endl;//正常输出
			}
			out << "		break;" << endl;
		}
	}

	//———————————————————————这段是yacc用的——————————————————————————
	if (mode == YACC_TEST)
	{
		string s;
		//int space_flag = 0;//等于1的时候说明是注释或者空格
		out << "	Token temp;" << endl;
		out << "	string s;" << endl;
		out << "	string temp_s;" << endl;
		//out << "	int flag_mult_line_comment = 0;" << endl;//对于多行注释的特殊处理：如果接收到多行注释，置一个flag为有效，遇到有效的flag时，这个终结态不做任何动作
		out << "	switch (action) " << endl;//根据endVec打印switch语句
		out << "	{" << endl;
		out << "		case 0:" << endl;
		out << "		break;" << endl;

		for (int i = 0; i < endVec.size(); i++)
		{
			out << "		case " << i + 1 << ":" << endl;
			/*out << "		if (flag_mult_line_comment == 1)" << endl;
			out << "		{" << endl;
			out << "			str_temp = str_temp[str_temp.size()-1];" << endl;
			out << "			flag_mult_line_comment = 0;" << endl;
			out << "			break;" << endl;
			out << "		}" << endl;*/
			for (int j = 0; j < endVec[i].size(); j++)
			{

				//这段有一部分应该是写在.c里面的，要在每个接受态添一段，把当前的token记录下来
				if (endVec[i][j].find_first_of('p', 0) == 0)//如果是printf这一行，把动作切出来
				{
					s = endVec[i][j].erase(0, 8);//删掉printf("
					s.erase(s.end() - 1);//删掉;
					s.erase(s.end() - 1);//删掉)
					s.erase(s.end() - 1);//删掉"
					//temp.token_type = s;
					//给一个判断，如果s是注释/空格，那就不要加进去了（给个flag，置为无效
					if (s == "SPACE" || s == "MULTI_LINE_COMMENT" || s == "SINGLE_LINE_COMMENT")
					{
						//space_flag = 1;
						/*if (s == "MULTI_LINE_COMMENT")
						{
							out << "		flag_mult_line_comment = 1;" << endl;
						}*/
						out << "		str_temp = str_temp[str_temp.size()-1];" << endl;//对str_temp的重置
						out << "		break;" << endl;
						out << endl;
					}
					else
					{
						//space_flag = 0;
						out << "		s = \"" << s << "\";" << endl;
						//out << "		str_temp.erase(str_temp.end() - 1);" << endl;
						out << "		temp_s = str_temp;" << endl;
						out << "		temp_s.erase(temp_s.end() - 1);" << endl;
						out << "		temp.token_type = s;" << endl;
						out << "		temp.token_value = temp_s;" << endl;
						out << "		str_temp = str_temp[str_temp.size()-1];" << endl;
						out << "		tokens.push_back(temp);" << endl;
						out << "		break;" << endl;
						out << endl;
					}
				}
				else {
					out << "		" << endVec[i][j] << endl;//正常输出
				}
			}
		}
	}
	//———————————————————————这段是yacc用的——————————————————————————

	out << "		default:" << endl;
	out << "		break;" << endl;
	out << "	}" << endl; 
	out << "	return -1;" << endl;
	out << "}" << endl; 
	out << endl;
	//int findAction(int state）函数结束

	//char* getCharPtr(char* fileName)函数(获取输入lex文件内容
	out << "char* getCharPtr(const char* fileName)" << endl;
	out << "{" << endl;
	out << "	char* cp=NULL;" << endl;
	out << "	FILE *fp;" << endl;
	out << "	fp=fopen(fileName,\"r\");" << endl;
	out << "	if(fp==NULL)" << endl;
	out << "	{" << endl;
	out << "		printf(\"can't open file\");" << endl;
	out << "		exit(0);" << endl;
	out << "	}" << endl;
	out << endl;
	out << "	fseek(fp,0,SEEK_END);" << endl;
	out << "	int flen = ftell(fp);" << endl; //得到文件大小
	out << "	cp = (char *)malloc(flen + 1);" << endl; //根据文件大小动态分配内存空间
	out << "	if (cp == NULL)" << endl;
	out << "	{" << endl;
	out << "		fclose(fp);" << endl;
	out << "		return 0;" << endl;
	out << "	}" << endl;
	out << "	rewind(fp);" << endl; //定位到文件开头
	out << "	memset(cp,0,flen+1);" << endl;
	out << "	fread(cp, sizeof(char), flen, fp);" << endl; //一次性读取全部文件内容
	out << "	cp[flen] = 0; " << endl;//设置字符串结束标志
	out << "	return cp;" << endl;
	out << "}" << endl;

	out << codeEnd;

	out.close();
	return 0;
}

void print_array(string name, int size, const int* value, ofstream& out)//打表
{
	const int* array_buf = value;
	out << "static int	" << name << "[" << size << "]" << " =" << endl;
	out << "	{	0," << endl;
	for (int i = 1; i < size; i++)
	{
		out << setw(4) << left << array_buf[i];
		if (i != size - 1)
		{
			out << ",";
		}
		if (i % 20 == 0)//1行20个
		{
			out << endl;
		}
	}
	out << "};" << endl;
}

