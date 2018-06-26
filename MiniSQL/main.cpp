#include "interpreter.h"
#include"record_manager.h"
#include"catalog_manager.h"
#include"table.h"
using namespace std;

//int main()
//{
//	int mode;
//	string input;
//	cout << "*************************************" << endl;
//	cout << "*    欢迎使用MiniSQL微型数据库      *" << endl;
//	cout << "*        Date:2018-6-13             *" << endl;
//	cout << "* 作者:田子珺，蒋文，林宇翔，沈子衿 *" << endl;
//	cout << "*************************************" << endl;
//	cout << "1. 语句执行模式" << endl;
//	cout << "2. 脚本执行模式" << endl;
//	cout << "3. 帮助" << endl;
//	cout << "4. 退出本程序" << endl;
//	while (true)
//	{
//		cout << "请选择模式[1/2/3(帮助)/4]：";
//		cin >> mode;
//		getchar();
//		switch (mode)
//		{
//		case 1:
//		{
//			while (true)
//			{
//				input = Input();
//				string temp = input;
//				RemoveSpace(temp);
//				if (temp == "quit;")
//				{
//					break;
//				}
//				else
//				{
//					string result = Interpreter(input); //执行sql语句
//					if (result == "99")
//					{
//						cout << "语句执行错误！" << endl;
//					}
//					else if(result == "80")
//					{
//						cout << "语句执行正确" << endl;
//					}
//					else
//					{
//						cout << "语句执行错误！" << endl;
//					}
//				}
//			}
//			break;
//		}
//		case 2:
//		{
//
//		}
//		case 3:
//		{
//			cout << "*****操作手册****" << endl;
//			cout << "1. 语句执行模式" << endl;
//			cout << "2. 脚本执行模式" << endl;
//			cout << "3. 帮助" << endl;
//			cout << "4. 退出本程序" << endl;
//			cout << "*****************" << endl;
//			break;
//		}
//		case 4:
//		{
//			char iff;
//			cout << "你确定要退出吗? [Y/N]:";
//			cin >> iff;
//			if (iff == 'Y')
//			{
//				cout << "告辞! " << endl;
//				system("pause");
//				return 0;
//			}
//			else if (iff == 'N')
//			{}
//			break;
//		}
//		}
//	}
//	system("pause");
//	return 0;
//}

int main() {
	
	data_dictionary d;
	m_string tableName("book");
	m_string colName("bno");
	m_string name("index1");
	//d.add_index(tableName, colName, name);
	d.listTable();
	system("pause");
}