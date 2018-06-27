#include "interpreter.h"
#include "table.h"
#include "record_manager.h"
#include "api.h"
#include <fstream>

bool isLegalName(string a)
{
	trim(a);
	if (a == "")
	{
		return false;
	}
	 // string illgal = "{}[]/<>~`\"\\|:\?";
	string illgalForName = "{}[]/<>~`\"\\|:\';+-=!@#$%%^&*\?() "; // ȡ������ʹ�õ��ַ�
	for (int i = 0; i < a.length(); i++)
	{
		if (illgalForName.find(a[i]) != -1)
		{
			return false;
		}
	}
	return true;
}

bool IsLegalInt(string a)
{
	trim(a);
	if (a == "")
	{
		return false;
	}
	for (int i = 0; i < a.length(); i++)
	{
		if (a[i] == '-')
		{
			if (i == 0)
				continue;
			else
				return false;
		}
		else if (!(a[i] >= '0' && a[i] <= '9'))
		{
			return false;
		}
	}
	return true;
}

bool IsLegalFloat(string a)
{
	trim(a);
	int count = 0;
	if (a == "")
	{
		return false;
	}
	for (int i = 0; i < a.length(); i++)
	{
		if (a[i] == '-')
		{
			if (i == 0)
				continue;
			else
				return false;
		}
		if (!(a[i] >= '0' && a[i] <= '9') && (a[i] != '.'))
		{
			return false;
		}
		if (a[i] == '.')
		{
			count++;
			if (count > 1)
				return false;
			if (i == 0)
				return false;
		}

	}
	return true;
}



void RemoveEmpty(vector<string> &s)
{
	vector<string>::iterator it;
	for (it = s.begin(); it != s.end();)
	{
		trim(*it);
		if (*it == "")
		{
			it = s.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void RemoveSpace(string &s, const string mark)
{
	size_t nSize = mark.size();
	while (1)
	{
		size_t pos = s.find(mark);    //  ����������  
		if (pos == string::npos)
		{
			return;
		}

		s.erase(pos, nSize);
	}
}

void RemoveTabSpace(string &s)
{
	size_t nSize = 1;
	while (1)
	{
		size_t pos = s.find(" ");    //  ����������  
		if (pos == string::npos)
		{
			return;
		}

		s.erase(pos, nSize);
	}
	while (1)
	{
		size_t pos = s.find("\t");    //  ����������  
		if (pos == string::npos)
		{
			return;
		}
		s.erase(pos, nSize);
	}
}
bool Space(string str)
{
	int find1 = str.find(" ");
	int find2 = str.find("\t");
	if (find1 == string::npos && find2 == string::npos)
	{
		return false;
	}
	return true;
}
string Input()
{
	string sql = "";
	string temp;
	bool finish = false;
	cout << "sql> ";
	while (!finish)
	{
		getline(cin, temp);
		trim(temp);
		if (temp.length() == 0)
			continue;
		if (temp[temp.length() - 1] == ';')
		{
			temp[temp.length() - 1] = ' ';
			temp += ";";
			sql += temp;
			finish = true;
		}
		else
		{
			sql += (temp + " ");
			cout << "---> ";
		}
	}
	return sql;
}

string ExecFile(string path)
{
	int find = path.find("execfile");
	find += 8;
	path = path.substr(find);
	cout << path << endl;
	if (path.empty())
	{
		cerr << "�﷨���󣺿�·��" << endl;
		return "99";
	}
	string fileName = path;
	trim(fileName);
	while (fileName[fileName.length() - 1] == ';')
	{
		fileName.erase(fileName.end() - 1);
	}
	trim(fileName);
	ifstream inf(".\\sql\\" + fileName);
	// �����ļ�����
	int line = 1;
	if (!inf.is_open())
	{
		cout << "���ļ�ʱ��������!" << endl;
		return "99";
	}
	string sql = "";
	string temp;
	while (!inf.eof())
	{
		getline(inf, temp);
		trim(temp);
		if (temp.length() == 0)
			continue;
		if (temp[temp.length() - 1] == ';')
		{
			temp[temp.length() - 1] = ' ';
			temp += ";";
			sql += temp;
			string result = Interpreter(sql);
			if (result == "80")
			{
				// ��յ�ǰsql���
				sql = "";
			}
			else if (result == "99")
			{
				cerr << "�ű�ִ��ʧ��!�����������ڵ�" << line << "��" << endl;
				return "99";
			}
		}
		else
			sql += (temp + " ");
		line++;
	}
	cout << "�ű�ִ�гɹ�" << endl;
	return "80";
}
// ���������
string Interpreter(string statement)
{
	trim(statement);
	string sql;
	string temp;
	int start = 0, end;
	sql = statement;
	if (sql == "")
	{
		return "99";
	}
	end = sql.find(' ', start);
	temp = sql.substr(start, end - start);
	start = end + 1;
	// ���û�����룬��ӡ������Ϣ
	if (temp.empty())
	{
		cout << "�﷨�������Ϊ��" << endl;
		sql = "99";
	}
	// ��Ϊ�������
	else if (temp == "create")
		sql = Create(sql);
	else if (temp == "drop")
		sql = Drop(sql);
	else if (temp == "select")
		sql = Select(sql);
	else if (temp == "insert")
		sql = Insert(sql);
	else if (temp == "delete")
		sql = Delete(sql);
	else if (temp == "execfile")
		sql = ExecFile(sql);
	else if (temp == "update")
		sql = UpdateTable(sql);
	// ����ǷǷ����
	else
	{
		cout << "�﷨����: ��" << temp << "��---��Ч�ĵĹؼ��ֻ򷽷�" << endl;
		sql = "99";
		return sql;
	}
	// ���������ڲ���ʽ
	return sql;
}

string  Drop(string sql)
{
	string nowSql = sql.substr(5);
	int position = nowSql.find(' ', 0);
	if (position == -1)
	{
		cout << "�﷨�������Ϊ��" << endl;
		sql = "99";
		return sql;
	}
	string keyWord = nowSql.substr(0, position);
	trim(keyWord);
	if (keyWord == "table")
	{
		sql = DropTable(sql);
	}
	else if (keyWord == "index")
	{
		sql = DropIndex(sql);
	}
	else
	{
		cout << "�﷨����: ��" << keyWord << "��---��Ч�Ĺؼ��ֻ򷽷�";
		sql = "99";
	}
	return sql;
}



// ȷ���Ǵ���ʲô
string Create(string sql)
{
	string nowSql = sql.substr(7);
	int position = nowSql.find(' ', 0);
	if (position == -1)
	{
		cout << "�﷨�������Ϊ��" << endl;
		sql = "99";
		return sql;
	}
	string keyWord = nowSql.substr(0, position);
	trim(keyWord);
	if (keyWord == "table")
	{
		sql = CreateTable(sql);
	}
	else if (keyWord == "index")
	{
		sql = CreateIndex(sql);
	}
	else
	{
		cout << "�﷨����: ��" << keyWord << "��---��Ч�Ĺؼ��ֻ򷽷�" << endl;
		sql = "99";
		return sql;
	}
	return sql;
}

// ���Խ�����
// CreateTable: ����ĺ��ĺ���

string CreateTable(string sql)
{
	InterTable newTable;
	vector<string> innerItem;
	vector<string> orderAttr;
	map<string, attrInfo> attrList; // ������--�洢������Ϣ�Ľṹ��
	string nowSql = sql.substr(12);
	int position = nowSql.find('(', 0);
	if (position == -1)
	{
		cout << "�﷨����ȱ�١�(��" << endl;
		sql = "99"; // �׳�������룬����
		return sql;
	}
	string tableName = nowSql.substr(0, position); // ��ñ���
	trim(tableName);
	if (isLegalName(tableName) == false)
	{
		cout << "�﷨���󣺱������зǷ��ַ�" << endl;
		sql = "99"; // �׳�������룬����
		return sql;
	}
	// ����û�������ŵ����
	char last = nowSql[nowSql.length() - 1];
	while (last == ';')
	{
		nowSql.erase(nowSql.end() - 1);
		trim(nowSql);
		last = nowSql[nowSql.length() - 1];
	}
	if (last != ')')
	{
		cout << "�﷨����ȱ�١�)����)�����쳣�ַ�" << endl;
		sql = "99";
		return sql;
	}
	// ����û�������ŵ��������
	// ���������š���ȡ�����е�����
	int position1 = nowSql.find("(");
	int position2 = nowSql.length() - 1; // �ҵ����һ��
	while (nowSql[position2] != ')')
		position2--;

	nowSql = nowSql.substr(position1, position2 - position1 + 1);
	// ������ȥ��
	nowSql.erase(nowSql.begin());
	nowSql.erase(nowSql.end() - 1);
	// �Ѿ���ȡ�����ڵ�����
	// �������ڵ����ݰ��ն��ŷָ�
	// ʹ��boost����зָ�
	boost::split(innerItem, nowSql, is_any_of(","), token_compress_on);
	// �Զ��ŷָ���Ԫ�ؽ���һ�δ���
	// Ԫ��Ӧ������ ��attrname varchar(15)���������
	RemoveEmpty(innerItem);
	if (innerItem.empty())
	{
		cout << "�﷨���󣺱���Ϊ��" << endl;
		sql = "99"; // ���ش�����룻
		return sql;
	}
	bool Primary = false; // �������������������û��
	string primaryName;
	for (int i = 0; i < innerItem.size(); i++)
	{
		trim(innerItem[i]);
		if (innerItem[i] == "")
		{
			cout << "�﷨���󣺱�����ڸ�ʽ����" << endl;
			sql = "99"; // ���ش�����룻
			return sql;
		}
		// ����ÿһ���������
		attrInfo tempInfo = ProcessInfo(innerItem[i]);
		if (tempInfo.resType == ERR)
		{
			cout << "�﷨��������" << tempInfo.attrName << "�Ķ�����ڸ�ʽ����" << endl;
			sql = "99"; // ���ش�����룻
			return sql;
		}
		// ����ɹ�
		//������������Զ����Լ�����
		// Լ����ӵĻ���ProcessInfo����ֻ���ر��������µ�Լ�����Լ��Ƿ�unique
		// Ϊ�˱����������Ĳ�ͬ������ǵ�����Լ�������䣬length���ᱻ��Ϊ-2
		// ����Ǵ���unique����䣬length�ᱻ��Ϊ-3
		if (tempInfo.length == -2) // �����Լ��������
		{
			string getName = tempInfo.attrName;
			if (isLegalName(getName) == false)
			{
				cout << "�﷨�������������зǷ��ַ�" << endl;
				sql = "99"; // �׳�������룬����
				return sql;
			}
			RESTRICT newRestrict = tempInfo.resType;
			switch (newRestrict)
			{
			case PRKEY:
			{
				if (Primary == true) // ��������Ѿ����������
				{
					cout << "Լ�����������������ض���" << endl;
					sql = "99";
					return sql;
				}
				else if (Primary == false)
				{
					map<string, attrInfo>::iterator it;
					if ((it = attrList.find(getName)) != attrList.end())
					{
						// �޸�Լ��
						it->second.resType = PRKEY;
						Primary = true;
						primaryName = getName;

					}
					else
					{
						cout << "����Լ��������������������" << endl;
						sql = "99";
						return sql;
					}
				}
				break;
			}
			case FRKEY:
			{
				map<string, attrInfo>::iterator it;
				if ((it = attrList.find(getName)) != attrList.end())
				{
					//�޸�Լ��
					it->second.resType = FRKEY;
					//�޸����õı�
					it->second.reference = tempInfo.reference;
				}
				// �������������
				else
				{
					cout << "���Լ��������������������" << endl;
					sql = "99";
					return sql;
				}
				break;
			}
			default:
			{
				cout << "Լ���������󣺲����ڵ�����" << endl;
				sql = "99";
				return sql;
			}
			}
		}
		// �����unique������
		else if (tempInfo.length == -3)
		{
			string getName = tempInfo.attrName;
			bool newUnique = tempInfo.unique;
			map<string, attrInfo>::iterator it;
			if ((it = attrList.find(getName)) != attrList.end())
			{
				// �޸�unique
				it->second.unique = newUnique;
			}
			else
			{
				cout << "uniqueԼ��������������������" << endl;
				sql = "99";
				return sql;
			}
		}
		// ���û������������
		else
		{
			// �����Բ������
			try
			{
				map<string, attrInfo>::iterator it;
				if ((it = attrList.find(tempInfo.attrName)) != attrList.end())
					throw "�������ʱ����������������ͻ";
				else
				{
					attrList.insert(pair<string, attrInfo>(tempInfo.attrName, tempInfo));
					orderAttr.push_back(tempInfo.attrName);
				}
			}
			catch (const char* Msg)
			{
				cerr << Msg << endl;
				sql = "99";
				return sql;
			}
		}
	}
	// �����б��Ѿ�����
	newTable.attrList = attrList;
	newTable.tableName = tableName;
	newTable.prKey = primaryName;
	newTable.orderAttr = orderAttr;
	/**************************************/
	/*construct the table class and insert*/
	/**************************************/
	cout << newTable.tableName << endl;
	map<string, attrInfo>::iterator iter;
	iter = newTable.attrList.begin();
	/*while (iter != newTable.attrList.end()) {
		cout << iter->first << " : " << iter->second.attrType << " " << iter->second.length<< " " << \
			iter->second.resType << " " << iter->second.unique <<endl;
		iter++;
	}*/
	int result = create_table_api(newTable);
	if (result == 1)
	{
		cerr << "���ݿ���󣺱�����ͻ��" << endl;
		sql = "99";
		return sql;
	}
	else if (result == 0)
	{
		sql = "80";
		return sql;
	}
	return sql;
}
// ����Ĳ���Ӧ�������� cool char(20) not unique;

attrInfo ProcessInfo(string info)
{
	// ��������������������Զ���
	attrInfo newInfo;
	newInfo.unique = false;
	newInfo.resType = DEFAULT;
	newInfo.length = -1;
	newInfo.attrType = "unknown";
	RESTRICT res;
	trim(info);
	vector<string> strGroup;
	split(strGroup, info, is_any_of("\n \t()"), token_compress_on);
	RemoveEmpty(strGroup);
	if (strGroup.empty() || strGroup.size() <= 1)
	{
		cerr << "���Դ����޷�ʶ���������" << endl;
		// ����
		newInfo.resType = ERR;
		return newInfo;
	}
	int ptr = 0;
	string attrName = strGroup[ptr++];
	trim(attrName);
	// ��������Լ����
	if (attrName == "primary" || attrName == "unique")
	{
		// ��Ҫ�������������
		if (attrName == "primary")
		{
			trim(strGroup[ptr]);
			if (strGroup[ptr] != "key")
			{
				cerr << "Լ������δ�����Լ������" << strGroup[ptr] << endl;
				// ����
				newInfo.resType = ERR;
				return newInfo;
			}
			else
			{
				try
				{
					string attrName = strGroup.at(++ptr);
					trim(attrName);
					newInfo.attrName = attrName;
					newInfo.length = -2;
					newInfo.resType = PRKEY;

				}
				catch (std::out_of_range)
				{
					cerr << "Լ������(primary key)��δ�����������" << endl;
					// ����
					newInfo.resType = ERR;
				}
				return newInfo;
			}
		}
		else
		{
			try
			{
				string attrName = strGroup.at(++ptr);
				trim(attrName);
				newInfo.attrName = attrName;
				newInfo.length = -3;
				newInfo.unique = true;

			}
			catch (std::out_of_range)
			{
				cerr << "Լ������(unique)��δ�����������" << endl;
				// ����
				newInfo.resType = ERR;
			}
			return newInfo;
		}
	}
	else
	{
		string type = strGroup[ptr++];
		int length = -1;
		trim(type);
		if (type == "char")
		{
			if (ptr >= strGroup.size())
			{
				cerr << "���ʹ���δָ��char���͵ĳ���" << endl;
				// ����
				newInfo.resType = ERR;
				return newInfo;
			}
			//��ȡ����
			int tempLen = atoi(strGroup[ptr].c_str());
			if (tempLen <= 0)
			{
				cerr << "���ʹ�����Ч��char���ͳ���ֵ" << endl;
				// ����
				newInfo.resType = ERR;
				return newInfo;
			}
			length = tempLen;
			ptr++;
		}
		else if (type == "int")
		{
		}
		else if (type == "float")
		{
		}
		else
		{
			cerr << "���ʹ���δ�������������" << endl;
			// ����
			newInfo.resType = ERR;
			return newInfo;
		}
		// ����Լ��
		if (ptr >= strGroup.size())
		{
			res = DEFAULT;
		}
		else
		{
			string resInfo = strGroup[ptr++];
			if (resInfo == "primary")
			{
				if (ptr >= strGroup.size() || strGroup[ptr] != "key")
				{
					cerr << "Լ������δ�����Լ������" << endl;
					// ����
					newInfo.resType = ERR;
					return newInfo;
				}
				newInfo.resType = PRKEY;
			}
			else if (resInfo == "unique")
			{
				newInfo.unique = true;
			}
			else
			{
				cerr << "Լ������δ�����Լ������" << endl;
				// ����
				newInfo.resType = ERR;
				return newInfo;
			}
		}
		newInfo.attrName = attrName;
		newInfo.attrType = type;
		newInfo.length = length;
		newInfo.reference = "";
		return newInfo;
	}
}




string DropTable(string sql)
{
	string posSql = sql;
	int find = sql.find("table");
	if (find != string::npos)
	{
		// ��table ȥ��
		sql = sql.substr(find + 4);
		if (sql.length() == 1) return "80";
		sql = sql.substr(1);
		if (sql[0] != ' ' && sql[0] != '\t')
		{
			cerr << "�﷨����drop table:�޷�ȷ���ı���" << endl;
			return "99";
		}
		trim(sql);
		find = sql.find(";");
		if (find == string::npos)
		{
			find = sql.length();
		}
		// ��ñ���
		string tableName = sql.substr(0, find);
		trim(tableName);
		if (isLegalName(tableName) == false)
		{
			cout << "�﷨���󣺱������пո��Ƿ��ַ�" << endl;
			sql = "99"; // �׳�������룬����
			return sql;
		}
		if (tableName.length() == 0)
		{
			cerr << "�﷨����drop table:�޷�ȷ���ı���" << endl;
			return "99";
		}
		trim(tableName);
		int result = drop_table_api(tableName);
		if (result == 1)
		{
			cerr << "drop table���󣺲��޴˱�" << endl;
			return "99";
		}
		cout << "ɾ����" << tableName << "�ɹ�" << endl;
		sql = "80";
	}
	else
	{
		cerr << "�﷨����drop table:�޷�ȷ���ı���" << endl;
		return "99";
	}
	return sql;
}


string DropIndex(string sql)
{
	string posSql = sql;
	int find = sql.find("index");
	if (find != string::npos)
	{
		// ��table ȥ��
		sql = sql.substr(find + 4);
		if (sql.length() == 1) return "80";
		sql = sql.substr(1);
		if (sql[0] != ' ' && sql[0] != '\t')
		{
			cerr << "�﷨����drop index:�޷�ȷ����������" << endl;
			return "99";
		}
		trim(sql);
		find = sql.find(";");
		if (find == string::npos)
		{
			find = sql.length();
		}
		// ��ñ���
		string tableName = sql.substr(0, find);
		trim(tableName);
		if (isLegalName(tableName) == false)
		{
			cout << "�﷨�������������пո��Ƿ��ַ�" << endl;
			sql = "99"; // �׳�������룬����
			return sql;
		}
		if (tableName.length() == 0)
		{
			cerr << "�﷨����drop index:�޷�ȷ����������" << endl;
			return "99";
		}
		trim(tableName);
		string result = drop_index_api(tableName);
		return result;
		cout << "delete from " << tableName << " success" << endl;
	}
	else
	{
		cerr << "�﷨����drop index:�޷�ȷ����������" << endl;
		return "99";
	}
	return sql;
}

string Delete(string sql)
{
	string preSql = sql;
	int find = sql.find("from");
	if (find == string::npos)
	{
		cerr << "�﷨����ȱ�١�from��" << endl;
		return "99";
	}
	sql = sql.substr(find + 4);
	find = sql.find("where");
	vector<condition> cod;
	string tableName;
	//��where�����
	if (find != string::npos)
	{
		tableName = sql.substr(0, find);
		RemoveSpace(tableName, ";");
		trim(tableName);
		if (tableName.length() == 0)
		{
			cerr << "�﷨����δָ����Ҫ���ҵı�" << endl;
			return "99";
		}
		if (isLegalName(tableName) == false)
		{
			cout << "�﷨���󣺱������пո��Ƿ��ַ�" << endl;
			return "99"; // �׳�������룬����
		}
		// ��ñ�����������ȡ
		sql = sql.substr(find + 5);
		while (sql[sql.length() - 1] == ';')
			sql.pop_back();
		trim(sql);
		try
		{
			cod = WhereSplit(sql);
		}
		catch (...)
		{
			return "99";
		}
		for (int i = 0; i < cod.size(); i++)
		{
			cout << "condition: " << cod[i].attr << " " << cod[i].value << endl;
		}
	}
	else
	{
		tableName = sql.substr(0, sql.length());
		RemoveSpace(tableName, ";");
		trim(tableName);
		if (tableName.length() == 0)
		{
			cerr << "�﷨����δָ����Ҫ���ҵı�" << endl;
			return "99";
		}
		if (isLegalName(tableName) == false)
		{
			cout << "�﷨���󣺱������пո��Ƿ��ַ�" << endl;
			return "99"; // �׳�������룬����
		}
		cod.clear();
		cout << "������" << endl;
	}
	if (cod.size() <= 1)
	{
		int result = multi_delete(tableName, cod);
		if (result == -1)
		{
			cerr << "delete���󣺲��޴˱�" << endl;
			return "99";
		}
		else if (result == -2)
		{
			cerr << "delete����:where�����ṩ�����Բ�����" << endl;
			return "99";
		}
		else if (result < 0)
		{
			// ��������
			return "99";
		}
		cout << result << "����Ӱ��" << endl;
		return "80";
	}
	else
	{
		int result = multi_delete(tableName, cod);
		if (result == -1)
		{
			cerr << "delete���󣺲��޴˱�" << endl;
			return "99";
		}
		else if (result == -2)
		{
			cerr << "delete����:where�����ṩ�����Բ�����" << endl;
			return "99";
		}
		else if (result < 0)
		{
			// ��������
			return "99";
		}
		cout << result << "����Ӱ��" << endl;
		return "80";
	}
}

string Insert(string sql)
{
	string preSql = sql;
	int find = sql.find("into");
	if (find == string::npos)
	{
		cerr << "�﷨����ȱ�١�into��" << endl;
		return "99";
	}
	sql = sql.substr(find + 4);
	if (sql[0] != ' ' && sql[0] != '\t')
	{
		cerr << "�﷨�����޷���ʶ��Ŀ����" << endl;
		return "99";
	}
	trim(sql);
	find = sql.find("values");
	if (find == string::npos)
	{
		cerr << "�﷨����ȱ�١�values��" << endl;
		return "99";
	}
	string tableName = sql.substr(0, find);
	trim(tableName);
	if (tableName.length() == 0)
	{
		cerr << "�﷨������Ч�ı���" << endl;
		return "99";
	}
	sql = sql.substr(find + 5);
	// �������űպϵ����
	int findLeft = sql.find("(");
	int findRight = sql.length() - 1;
	while (findRight != -1 && sql[findRight] != ')')
		--findRight;
	if (findLeft == string::npos || findRight == -1 || findLeft >= findRight)
	{
		cerr << "�﷨�������Ų��պ�" << endl;
		return "99";
	}
	//��ȡ�����ŵĲ���
	string value = sql.substr(findLeft, findRight - findLeft + 1);
	// ȥ������
	trim(value);
	value.erase(value.begin());
	value.erase(value.end() - 1);
	// �ж��ŵ����ݽ���valueList
	vector<string> valueList;
	try
	{
		valueList = ValueSplit(value);
	}
	catch(...)
	{
		return "99";
	}
	try
	{
		int result = insert_into_api(valueList, tableName);
		if (result == 0)
		{
			return "80";
		}
		else
		{
			return "99";
		}
	}
	catch(...)
	{
		return "99";
	}
}


// ������Ϣ����ģ���Ӧ�����ɶ��ŷָ�������




string Select(string sql)
{
	if (sql.empty()) return "80";
	string preSql = sql;
	// ȥ��ͷ����select
	string mask1 = "select";
	string mask2 = "";
	vector<string> vec2;
	vec2.push_back(mask1);
	vec2.push_back(mask2);
	replace_first(sql, mask1, mask2);
	// ���select�ͱ���֮��û�пո�
	if (sql[0] != ' ' && sql[0] != '\t')
	{
		cerr << "�﷨�����޷�ʶ���sql����" << endl;
		return "99";
	}
	// ���û�����⣬��ȡ��Ҫ���ҵ�������
	trim(sql);
	// �������ҵ���һ��from����ȡǰ����ַ���
	int find = sql.find("from");
	if (find != string::npos)
	{
		// ��ȡ����
		string targetList = sql.substr(0, find);
		sql = sql.substr(find); // sql ��ȡ֮��Ĵ�
								//���������еĿո�ȫ��ȥ��
		RemoveSpace(targetList);
		if (targetList.length() == 0)
		{
			cerr << "�﷨����δָ����Ҫ���ҵ�����" << endl;
			return "99";
		}
		// �����ַ���
		vector<string> attr;
		bool all = false;
		split(attr, targetList, is_any_of(","), token_compress_on);
		vector<string> newAttr;
		// ����������С�"*"
		for (int i = 0; i < attr.size(); i++)
		{
			// �����Ϊ��
			if (!attr[i].empty())
			{
				trim(attr[i]);
				if (attr[i] == "*")
				{
					all = true;
					newAttr.clear();
					break;
				}
				else
				{
					all = false;
					newAttr.push_back(attr[i]);
				}
			}
		}
		// Ȼ��ʼ��������
		trim(sql);
		// ȥ�� from
		sql = sql.substr(4);
		find = sql.find("where");
		if (find != string::npos)
		{
			string tableName = sql.substr(0, find);
			trim(tableName);
			if (tableName.length() == 0)
			{
				cerr << "�﷨����δָ����Ҫ���ҵı�" << endl;
				return "99";
			}
			
			sql = sql.substr(find + 5);
			trim(sql);
			while (sql[sql.length() - 1] == ';')
				sql.erase(sql.end() - 1);
			trim(sql);
			vector<condition> cod = WhereSplit(sql);
			// ��������б�
			for (int i = 0; i < cod.size(); i++)
			{
				cout << "condition: " << cod[i].attr << " " << cod[i].value << endl;
			}
			/*----------------------------------------*/
			string a = new_select_api(tableName, newAttr, cod); // �����ȫ����newAttr�ǿյ�
			return a;
		}
		else
		{
			string tableName = sql.substr(0, sql.length());
			trim(tableName);
			RemoveSpace(tableName, ";");
			trim(tableName);
			if (tableName.length() == 0)
			{
				cerr << "�﷨����δָ����Ҫ���ҵı�" << endl;
				return "99";
			}
			vector<condition> cod;
			cod.clear();
			string a = new_select_api(tableName, newAttr, cod); // �����ȫ����newAttr�ǿյ�
			return a;
		}
	}
	else
	{
		cerr << "�﷨����ȱ��from" << endl;
		return "99";
	}
	return sql;
}


vector<string> ValueSplit(string input)
{
	vector<string> result;
	string stack = "";
	string number = "";
	bool close = true;
	bool submit = true;
	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == '\'' && close == true)
		{
			stack.push_back(input[i]);
			close = false; // ������ջ��ѹ���ַ���
		}
		else if (input[i] == '\'' && close == false)
		{
			stack.push_back(input[i]);
			close = true;
			int k = i + 1;
			while (k < input.length() - 1 && (!input[k] == ','))
			{
				if (input[k] != ' ' && input[k] != '\t')
				{
					throw "value����"+stack+"����δ����ı�ʶ��";
				}
			}
			i = k - 1;
			submit = false;
		}
		else
		{
			if (close == false)
			{
				stack.push_back(input[i]);
			}
			else if (close == true)
			{
				if (input[i] == ' ' || input[i] == '\t')
				{
					continue;
				}
				if (input[i] == ',' && submit == false)
				{
					result.push_back(stack);
					stack = "";
					submit = true;
				}
				else
				{
					int nextCut = input.find(",", i);
					if (nextCut == string::npos)
					{
						nextCut = input.length();
					}
					
					number = input.substr(i, nextCut - i);
					trim(number);
					i = nextCut;
					const char* temp = number.c_str();
					int t = number.find(".");
					if (t != string::npos)
					{
						int d = atof(temp);
						if (d == 0)
						{
							string error = "value���󣺴���ĸ�������" + number;
							cerr << error << endl;
							throw error.c_str();
						}
						else
						{
							result.push_back(number);
							number = "";
						}
					}
					else
					{
						int d = atoi(temp);
						if (d == 0 && number != "0")
						{
							string error = "value���󣺴������������" + number;
							cerr << error << endl;
							throw error.c_str();
						}
						else
						{
							result.push_back(number);
							number = "";
						}
					}
				}
			}
		}
	}
	if (close == true && submit == false)
	{
		result.push_back(stack);
		stack = "";
		submit = true;
	}
	else if (close == false)
	{
		string error = "value����δ�պϵĵ�����";
		cerr << error << endl;
		throw error.c_str();
	}
	return result;
}


vector<condition> WhereSplit(string input)
{
	string save = input;
	// step 1
	vector<string> codList;
	string stack1 = "";
	string stack2 = "";
	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == '\t') input[i] = ' ';
		if (input[i] == '\'')
		{
			int nextCut = input.find("\'",i + 1); // �ҵ��պϵ���һ��
			if (nextCut == string::npos)
			{
				string error = "where����δ�պϵ�\'";
				cerr << error << endl;
				throw error.c_str();
			}
			stack1 = input.substr(i, nextCut - i + 1);
			i = nextCut; //i �ƶ�����һλ��
			input[i] = ' ';
			/*list.push_back(stack2 + " " + stack1);
			stack2 = "";
			stack1 = "";*/
		}
		else
		{
			stack2.push_back(input[i]);
			int find = stack2.find(" and ");
			if (find != string::npos)
			{
				stack2 = stack2.substr(0, find);
				if (stack1 != "")
				{
					codList.push_back(stack2 + " " + stack1);
					stack2 = "";
					stack1 = "";
				}
				else
				{
					codList.push_back(stack2);
					stack2 = "";
				}
			}
		}
	}
	if (stack1 != "")
	{
		codList.push_back(stack2 + " " + stack1);
		stack2 = "";
		stack1 = "";
	}
	else
	{
		codList.push_back(stack2);
		stack2 = "";
	}
	vector<condition> temp;
	condition cod;
	for (int i = 0; i < codList.size(); i++)
	{
		trim(codList[i]);
		int find1 = codList[i].find("<");
		int find2 = codList[i].find(">");
		int find3 = codList[i].find("=");
		int find4 = codList[i].find(">=");
		int find5 = codList[i].find("<=");
		int find6 = codList[i].find("<>");
		// ����С��
		if (find1 != string::npos && find5 == string::npos && find6 == string::npos)
		{
			string attrName = codList[i].substr(0, find1);
			string condition = codList[i].substr(find1);
			condition.erase(condition.begin()); //ȥ����һ���ַ�
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = SMALL;
			cod.value = condition;
			temp.push_back(cod);
			// 
		}
		// ��������
		else if (find2 != string::npos && find4 == string::npos && find6 == string::npos)
		{
			string attrName = codList[i].substr(0, find2);
			string condition = codList[i].substr(find2);
			condition.erase(condition.begin()); //ȥ����һ���ַ�
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = BIG;
			cod.value = condition;
			temp.push_back(cod);
		}
		// ��������
		else if (find3 != string::npos && find4 == string::npos && find5 == string::npos)
		{
			string attrName = codList[i].substr(0, find3);
			string condition = codList[i].substr(find3);
			condition.erase(condition.begin()); //ȥ����һ���ַ�
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = EQUAL;
			cod.value = condition;
			temp.push_back(cod);
		}
		// ������
		else if (find5 != string::npos)
		{
			string attrName = codList[i].substr(0, find5);
			string condition = codList[i].substr(find5 + 1);
			condition.erase(condition.begin()); //ȥ����һ���ַ�
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = NOTBIG;
			cod.value = condition;
			temp.push_back(cod);
		}
		// ��С��
		else if (find4 != string::npos)
		{
			string attrName = codList[i].substr(0, find4);
			string condition = codList[i].substr(find4 + 1);
			condition.erase(condition.begin()); //ȥ����һ���ַ�
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = NOTSMALL;
			cod.value = condition;
			temp.push_back(cod);
		}
		// ������
		else if (find6 != string::npos)
		{
			string attrName = codList[i].substr(0, find6);
			string condition = codList[i].substr(find6 + 1);
			condition.erase(condition.begin()); //ȥ����һ���ַ�
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = NOTEQUAL;
			cod.value = condition;
			temp.push_back(cod);
		}
		else
		{
			string error = "where���󣺴��������" + codList[i];
			cerr << error << endl;
			throw error.c_str();
		}
	}
	return temp;
}

string CreateIndex(string sql)
{
	// ���뺯�������Ӧ����create index
	string preSql = sql;
	int find = sql.find("on");
	if (find == string::npos)
	{
		cerr << "�﷨����ȱ�١�on��" << endl;
		return "99";
	}
	int findPre = sql.find("index");
	findPre += 5;
	string indexName = sql.substr(findPre, find - findPre);
	trim(indexName);
	if (indexName.length() == 0)
	{
		cerr << "�﷨����:δָ��������" << endl;
		return "99";
	}
	if (!isLegalName(indexName))
	{
		cerr << "�﷨���󣺷Ƿ���������" << endl;
		return "99";
	}
	sql = sql.substr(find + 2);
	trim(sql);
	// �� on ȥ����
	find = sql.find(";");
	string tableAttr = sql.substr(0, find);
	trim(tableAttr);
	if (tableAttr.length() == 0)
	{
		cerr << "�﷨����:δָ������Ŀ��" << endl;
		return "99";
	}
	// �����������
	int find1 = tableAttr.find("(");
	int find2 = tableAttr.find(")");
	if (find1 == string::npos || find2 == string::npos || find1 > find2)
	{
		cerr << "�﷨�������Ų��պ�" << endl;
		return "99";
	}
	string table = tableAttr.substr(0, find1);
	string attr = tableAttr.substr(find1, find2 - find1);
	attr.erase(attr.begin());
	trim(table);
	trim(attr);
	if (!(isLegalName(table)) || (!isLegalName(attr)))
	{
		cerr << "�﷨���󣺷Ƿ��ı���������" << endl;
		return "99";
	}
	cout << indexName << " " << table << " " << attr << endl;
	string result = create_index_api(table, attr, indexName);
	sql = result;
	return sql;
}


void clear()
{

}

string UpdateTable(string sql)
{
	trim(sql);								//	���ַ������˿ո�ȥ��
	string formSql = sql;					//	����ԭ�ַ���
	sql = sql.substr(6);					//	ȥ��update����Ϊ6λ
	if (sql[0] != ' ' || sql[0] != '\t')	//	���update�����ַ��ǿ�
	{
		cerr << "Syntax Error: Invalid table name in update expression" << endl;
		return "99";
	}
	// ��ʱӦ����¶table
	trim(sql);
	int find = sql.find("set");
	if (find == string::npos)
	{
		cerr << "Syntax Error: keyword \'set'\ is missed" << endl;
		return "99";
	}
	string tableName = sql.substr(0, find);
	trim(tableName);
	if (!isLegalName(tableName))
	{
		cerr << "Syntax Error: illegal table name" << endl;
		return "99";
	}
	sql = sql.substr(find + 3);
	if (sql[0] != ' ' || sql[0] != '\t')	//	���update�����ַ��ǿ�
	{
		cerr << "Syntax Error: Invalid attribute name in update expression" << endl;
		return "99";
	}
	trim(sql);
	string setStr;
	// ������ȡset������Ϊ��������Ҫ���ҵ�where
	int find2 = sql.find("where");
	// û��
	if (find2 == string::npos)
	{
		int find3 = sql.find(";");
		setStr = sql.substr(0, find3);
		trim(setStr);
	}
	else
	{
		setStr = sql.substr(0, find2);
		trim(setStr);
	}
	// ����setstr
	vector<condition> condList;
	condList.clear(); // һ��ʼ���
	if (find2 != string::npos)
	{
		int find4 = find2 + 5;
		if (find4 >= sql.length() || (sql[find4] != ' ' && sql[find4] != '\t'))
		{
			cerr << "Syntax Error: Invalid expression after \'where\'" << endl;
			return "99";
		}
		int find5 = sql.find(";");
		if (find5 < find4)
		{
			cerr << "Syntax Error: Unexpected \';\'" << endl;
			return "99";
		}
		string whereStr = sql.substr(find4, find5 - find4);
		trim(whereStr);
		condList = WhereSplit(whereStr); // ��ȡ������
	}
	vector<string> container;
	boost::split(container, setStr, is_any_of("="), token_compress_off);
	if (container.size() != 2)
	{
		cerr << "Syntax Error: Unexpected \'=\'" << endl;
		return "99";
	}
	trim(container[0]);
	trim(container[1]);
	if (!isLegalName(container[0]))
	{
		cerr << "Syntax Error: illgal attribute name" << endl;
		return "99";
	}
	//  ��ʼ���ýӿ�


	return "80";
}

