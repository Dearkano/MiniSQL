#include "interpreter.h"
#include"record_manager.h"
#include"catalog_manager.h"
#include"table.h"
using namespace std;

int main()
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
				string result = Interpreter(input); //÷¥––sql”Ôæ‰
				if (result == "99")
				{
					cout << "”Ôæ‰÷¥–– ß∞‹" << endl;
				}
				else if (result == "80")
				{
					cout << "”Ôæ‰÷¥––≥…π¶" << endl;
				}
				else
				{
					cout << "”Ôæ‰÷¥–– ß∞‹" << endl;
				}
			}
		}
		system("pause");
		return 0;
	}
	else {
		record_manager r;
		for (int i = 0; i < 100; i++) {
			r.select_2(m_string("t1"));
			cout << i << endl;
		}
	
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