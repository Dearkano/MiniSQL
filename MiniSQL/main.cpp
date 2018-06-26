#include "interpreter.h"
#include"record_manager.h"
#include"catalog_manager.h"
#include"table.h"
using namespace std;

//int main()
//{
//	//int mode;
//	//data_dictionary db;
//	//string input;
//	//cout << "*************************************" << endl;
//	//cout << "*    欢迎使用MiniSQL微型数据库      *" << endl;
//	//cout << "*        Date:2018-6-13             *" << endl;
//	//cout << "* 作者:田子珺，蒋文，林宇翔，沈子衿 *" << endl;
//	//cout << "*************************************" << endl;
//	//cout << "1. 语句执行模式" << endl;
//	//cout << "2. 脚本执行模式" << endl;
//	//cout << "3. 帮助" << endl;
//	//cout << "4. 退出本程序" << endl;
//	//while (true)
//	//{
//	//	cout << "请选择模式[1/2/3(帮助)/4]：";
//	//	cin >> mode;
//	//	getchar();
//	//	switch (mode)
//	//	{
//	//	case 1:
//	//	{
//	//		while (true)
//	//		{
//	//			input = Input();
//	//			string temp = input;
//	//			RemoveSpace(temp);
//	//			if (temp == "describe;")
//	//			{
//	//				db.listTable();
//	//			}
//	//			if (temp == "quit;")
//	//			{
//	//				break;
//	//			}
//	//			else
//	//			{
//	//				string result = Interpreter(input); //执行sql语句
//	//				if (result == "99")
//	//				{
//	//					cout << "语句执行错误！" << endl;
//	//				}
//	//				else if(result == "80")
//	//				{
//	//					cout << "语句执行正确" << endl;
//	//				}
//	//				else
//	//				{
//	//					cout << "语句执行错误！" << endl;
//	//				}
//	//			}
//	//		}
//	//		break;
//	//	}
//	//	case 2:
//	//	{
//
//	//	}
//	//	case 3:
//	//	{
//	//		cout << "*****操作手册****" << endl;
//	//		cout << "1. 语句执行模式" << endl;
//	//		cout << "2. 脚本执行模式" << endl;
//	//		cout << "3. 帮助" << endl;
//	//		cout << "4. 退出本程序" << endl;
//	//		cout << "*****************" << endl;
//	//		break;
//	//	}
//	//	case 4:
//	//	{
//	//		char iff;
//	//		cout << "你确定要退出吗? [Y/N]:";
//	//		cin >> iff;
//	//		if (iff == 'Y')
//	//		{
//	//			cout << "告辞! " << endl;
//	//			system("pause");
//	//			return 0;
//	//		}
//	//		else if (iff == 'N')
//	//		{}
//	//		break;
//	//	}
//	//	}
//	//}
//	//system("pause");
//	//return 0;
//	int mode;
//	data_dictionary db;
//	string input;
//	
//	while (true)
//	{
//		input = Input();
//		string temp = input;
//		RemoveSpace(temp);
//		if (temp == "describe;")
//		{
//			db.listTable();
//		}
//		if (temp == "quit;")
//		{
//			break;
//		}
//		else
//		{
//			string result = Interpreter(input); //执行sql语句
//			if (result == "99")
//			{
//				cout << "run error" << endl;
//			}
//			else if (result == "80")
//			{
//				cout << "success" << endl;
//			}
//			else
//			{
//				cout << "run error" << endl;
//			}
//		}
//	}
//	system("pause");
//	return 0;
//}

int main() {
	
	record_manager r;
	m_string *m1 = new m_string[2]{ m_string("id"),m_string("name") };
	m_string *m2 = new m_string[2]{ m_string("2"),m_string("a4") };
	m_string *m3 = new m_string[1]{ m_string("id") };
	char *opt = new char[2] {'g', '<'};
	table* tb = r.select_2(m_string("t1"), 2, m1, m2, opt,m3,1);
	
	system("pause");
}