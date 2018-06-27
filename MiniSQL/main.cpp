#include "interpreter.h"
#include"record_manager.h"
#include"catalog_manager.h"
#include"table.h"

using namespace std;



int main(int argc, char* argv[])
{

	int md = 0;
	if (md != 1) {
		int mode;
		data_dictionary db;
		string input;

		while (true)
		{
			input = Input();
			string temp = input;
			RemoveSpace(temp);
			if (temp == "describe;" || temp == "describe ;")
			{
				db.db = new database();
				db.listTable();
			}
			else if (temp == "quit;")
			{
				break;
			}
			else
			{
				string result = Interpreter(input); //ִ��sql���
				if (result == "99")
				{
					cout << "���ִ��ʧ��" << endl;
				}
				else if (result == "80")
				{
					cout << "���ִ�гɹ�" << endl;
				}
				else
				{
					cout << "���ִ��ʧ��" << endl;
				}
			}
		}
		system("pause");
		return 0;
	}
	else {
		/*Py_SetProgramName(argv[0]);
		Py_Initialize();
		PyRun_SimpleString("print 'Hello Python!'\n");
		Py_Finalize();
		getchar();
		return 0;*/
	}
}

//int main() {
//	
//	data_dictionary d;
//	m_string tableName("book");
//	m_string colName("bno");
//	m_string name("index1");
//	record_manager r;
//	int rows[2]{ 1,4 };
//	r._delete_2(m_string("t2"),rows,2);
//	d.listTable();
//	system("pause");
//}


