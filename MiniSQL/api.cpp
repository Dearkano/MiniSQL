#include "api.h"
#include "real_buffer_manager.h"
#include <cstring>
#include <set>
#include <algorithm>
#include"IndexManager.h"
#include "catalog_manager.h"
#include "record_manager.h"
// #define DEBUG
IndexManager im;

int create_table_api(InterTable newTable)
{
	data_dictionary dt;
	map<string, attrInfo>::iterator iter;
	vector<string> order = newTable.orderAttr;
	const size_t columnNum = newTable.attrList.size();
	int ptr = 0;
	column col[15];
	iter = newTable.attrList.begin();
	// ���������Ϣ
	m_string tableName(newTable.tableName);
	for(int i = 0; i < order.size(); i++)
	{
		string attr = order[i];
		m_string attrName(attr.c_str());
		m_string attrType(newTable.attrList[attr].attrType.c_str());
		int attrSize = newTable.attrList[attr].length;
		RESTRICT res = newTable.attrList[attr].resType;
		bool pk = false;
		if (res == PRKEY)
			pk = true;
		bool unique = newTable.attrList[attr].unique;
		if (pk == true)
			unique = true;
		column tempCol(attrName, attrType, attrSize, unique, pk);
		col[ptr++] = tempCol;
		++iter;
	}
	table t(tableName, columnNum, 0, col);
	int result = dt.create_table(t);
	real_buffer_manager rb;
	rb.create_dbFile(tableName);
	return result;
}


int insert_into_api(vector<string> valueList, string tableName)
{
	data_dictionary dt;
	record_manager rc;
	database *db = dt.db;
	int flag = -1;
	for (int i = 0; i < db->tableNum; i++)
	{
		m_string temp(tableName.c_str());
		if (db->tables[i]->table_name == temp)
		{
			flag = i;
			break;
		}
	}
	if (flag != -1)
	{
		if (valueList.size() != db->tables[flag]->column_num)
		{
			cerr << "value���󣺲����б��Ȳ�һ��"  << endl;
			return -1;
		}
		m_string value[15];
		int ptr = 0;
		for (int i = 0; i < db->tables[flag]->column_num; i++)
		{
			m_string type = db->tables[flag]->columns[i].data_type;
			if (strcmp (type.str, "char") == 0)
			{
				trim(valueList[i]);
				if (valueList[i][0] != '\'' || valueList[i][valueList[i].length() - 1] != '\'')
				{
					cerr << "value���󣺲���" + valueList[i] + "���ʹ���char��ʽ����" << endl;
					return -1;
				}
				int size = db->tables[flag]->columns[i].data_size;
				valueList[i].erase(valueList[i].begin());
				valueList[i].erase(valueList[i].end() - 1);
				if (valueList[i].length() > size)
				{
					cerr << "value���󣺲���" + valueList[i] + "���ʹ���char�ַ������ȳ�������" <<endl;
					return -1;
				}
				m_string str(valueList[i].c_str());
				value[ptr++] = str;
			}
			else if (strcmp(type.str, "int") == 0)
			{
				RemoveTabSpace(valueList[i]);
				if (!IsLegalInt(valueList[i]))
				{
					cerr << "value���󣺲���" + valueList[i] + "���ʹ���int��ʽ����" << endl;
					return -1;
				}
				m_string _int(valueList[i].c_str());
				value[ptr++] = _int;
			}
			else if (strcmp(type.str, "float") == 0)
			{
				RemoveTabSpace(valueList[i]);
				if (!IsLegalFloat(valueList[i]))
				{
					cerr << "value���󣺲���" + valueList[i] + "���ʹ���:float��ʽ����" << endl;
					return -1;
				}
				m_string _float(valueList[i].c_str());
				value[ptr++] = _float;
			}
		}
		m_string m_tableName(tableName.c_str());
		int result = rc.add(m_tableName, value);
		if (result == -1)
		{
			cerr << "value���󣺲��޴˱�" << endl;
			return -1;
		}
		else
		{
			if(result == 1)
				cerr << "value����������unique��ͻ" << endl;
			return result;
		}
	}
	cerr << "value���󣺲��޴˱�" << endl;
	return -1;
}


int delete_from_api(string tableName, vector<condition> option)
{
	/*enum COD { BIG, SMALL, EQUAL, NOTSMALL, NOTBIG, NOTEQUAL };*/
	record_manager rc;
	m_string m_tableName(tableName.c_str());
	if (option.empty())
	{
		int result = rc._delete(m_tableName);
		return result;
	}
	condition opt = option[0];
	m_string optAttr(opt.attr.c_str()); //��ȡ����
	char cond = ' ';
	switch (opt.cond)
	{
		case BIG:
			cond = '>';
			break;
		case SMALL:
			cond = '<';
			break;
		case EQUAL:
			cond = '=';
			break;
		case NOTSMALL:
			cond = 'g';
			break;
		case NOTBIG:
			cond = 'l';
			break;
		case NOTEQUAL:
			cond = '!';
			break;
		default:
			break;
	}
	string temp = opt.value;
	trim(temp);
	if (temp[0] == '\'' && temp[temp.length() - 1] == '\'')
	{
		temp.erase(temp.begin());
		temp.erase(temp.end() - 1);
	}
	else
	{
		RemoveTabSpace(temp);
		if (IsLegalInt(temp) == false && IsLegalFloat(temp) == false)
		{
			cerr << "where�������󣺴���������򸡵���" << temp << endl;
			return -3;
		}
	}
	m_string value(temp.c_str());
	int result = rc._delete(m_tableName, optAttr, value, cond);
	return result;
}


int multi_delete(string tableName, vector<condition> option)
{
	record_manager r;
	trim(tableName);
	if (option.size() == 0)
	{
		int result = r._delete_2(m_string(tableName));
		return result;
	}
	int size = option.size();
	m_string name[15];
	m_string value[15];
	char opt[15];
	for (int i = 0; i < size; i++)
	{
		name[i] = m_string(option[i].attr);
		value[i] = m_string(option[i].value);
		char cond;
		switch (option[i].cond)
		{
		case BIG:
			cond = '>';
			break;
		case SMALL:
			cond = '<';
			break;
		case EQUAL:
			cond = '=';
			break;
		case NOTSMALL:
			cond = 'g';
			break;
		case NOTBIG:
			cond = 'l';
			break;
		case NOTEQUAL:
			cond = '!';
			break;
		default:
			break;
		}
		opt[i] = cond;
	}
	int result = r._delete_2(m_string(tableName), size, name, value, opt);
	return result;

}

// ��condition������
// ֻ��һ�����
table* select_from_api(string tableName, vector<string> attrList, condition opt)
{
	m_string attr[15];
	int columnNum;
	if (attrList.empty())
	{
		attr[0] = m_string("*");
		columnNum = 1;
	}
	else
	{
		columnNum = attrList.size();
		for (int i = 0; i < columnNum; i++)
		{
			trim(attrList[i]);
			strcpy(attr[i].str, attrList[i].c_str());
		}
	}
	record_manager rc;
	m_string m_tableName(tableName.c_str());
	m_string optAttr(opt.attr.c_str()); //��ȡ����
	char cond = ' ';
	switch (opt.cond)
	{
	case BIG:
		cond = '>';
		break;
	case SMALL:
		cond = '<';
		break;
	case EQUAL:
		cond = '=';
		break;
	case NOTSMALL:
		cond = 'g';
		break;
	case NOTBIG:
		cond = 'l';
		break;
	case NOTEQUAL:
		cond = '!';
		break;
	default:
		break;
	}
	string temp = opt.value;
	trim(temp);
	if (temp[0] == '\'' && temp[temp.length() - 1] == '\'')
	{
		temp.erase(temp.begin());
		temp.erase(temp.end() - 1);
	}
	else
	{
		RemoveTabSpace(temp);
		if (IsLegalInt(temp) == false && IsLegalFloat(temp) == false)
		{
			cerr << "where�������󣺴���������򸡵���" << temp << endl;
			table* errTable = new table;
			errTable->isError = 2;
			return errTable;
		}
	}
	m_string value(temp.c_str());
	// �Ѿ���ȡ����ֵ
	// ��ʼ���ú���
	// ���Ա��
	table* result = rc.select(m_tableName, attr,columnNum, optAttr, value, cond);
	return result;
}

// ���غ�����û�������ĵ�Լ�����
table* select_from_api(string tableName, vector<string> attrList)
{
	m_string attr[15];
	int columnNum;
	if (attrList.empty())
	{
		attr[0] = m_string("*");
		columnNum = 1;
	}
	else
	{
		columnNum = attrList.size();
		for (int i = 0; i < columnNum; i++)
		{
			trim(attrList[i]);
			attr[i] = m_string(attrList[i].c_str());
		}
	}
	record_manager rc;
	m_string m_tableName(tableName.c_str());
	table* result = rc.select(m_tableName, attr, columnNum);
	return result;
}

// ������Ϲ���
string select_api(string tableName, vector<string> attrList, vector<condition> options)
{
	// ���û������
	if (options.empty())
	{
		table* result  = select_from_api(tableName, attrList);
		if (result->isError == 1)
		{
			cerr << "��ѯ���󣺲����ڵı���" << endl;
			return "99";
		}
		if (result->row_num == 0)
		{
			cerr << "��ѯ�ɹ�����û��ƥ�������" << endl;
			return "80";
		}
		if (attrList.empty())
		{
			for (int i = 0; i < result->column_num; i++)
				cout << result->columns[i].column_name << "\t\t";
			cout << endl;
			for (int i = 0; i < result->row_num; i++)
			{
				for (int j = 0; j < result->column_num; j++)
					cout << result->rows[i]->data[j] << "\t\t";
				cout << endl;
			}
			result = NULL;
		}
		else
		{
			for (int i = 0; i < result->column_num; i++)
				cout << attrList[i] << "\t\t";
			cout << endl;
			for (int i = 0; i < result->row_num; i++)
			{
				for (int j = 0; j < result->column_num; j++)
					cout << result->rows[i]->data[j] << "\t\t";
				cout << endl;
			}
		}
		
	}
	// ��Ҫ���������ֺ�
	else
	{
		vector<string> attr;
		vector<vector<string>> all;
		all.clear();
		vector<int> rowNumber;
 		table* temp = select_from_api(tableName, attrList, options[0]);
		if (temp->isError == 1)
		{
			cerr << "��ѯ���󣺲����ڵı���" << endl;
			return "99";
		}
		if (temp->isError == 2)
		{
			return "99";
		}
		// �Ȱ���������ȡ��
		for (int i = 0; i < temp->column_num; i++)
			attr.push_back(temp->columns[i].column_name.str);
		vector<vector<string>> ts;
		for (int k = 0; k < options.size(); k++)
		{
			table* result = select_from_api(tableName, attrList, options[k]);
			if (result->isError == 1)
			{
				cerr << "��ѯ���󣺲����ڵı���" << endl;
				return "99";
			}
			for (int i = 0; i < result->row_num; i++)
			{
				vector<string> tv;
				for (int j = 0; j < result->column_num; j++)
				{
					tv.push_back(result->rows[i]->data[j].str);
				}
				rowNumber.push_back(result->rows[i]->id);
				ts.push_back(tv);
			}
		}
		for (int i = 0; i < ts.size(); i++)
		{
			if (rowNumber[i] >= 0 && count(rowNumber.begin(), rowNumber.end(), rowNumber[i]) >= options.size())
			{
				all.push_back(ts[i]);
				for (int j = i; j < ts.size(); j++)
				{
					if (rowNumber[j] == rowNumber[i])
					{
						rowNumber[j] = -1; //��Ч
					}
				}
			}
		}
		// ��������
		if (all.empty())
		{
			cout << "��ѯ�ɹ�������ƥ������" << endl;
			return "80";
		}
		for (int i = 0; i < attr.size(); i++)
			cout << attr[i] << "\t\t";
		cout << endl;
		for (int i = 0; i < all.size(); i++)
		{
			for (int j = 0; j < all[i].size(); j++)
				cout << all[i][j] << "\t\t";
			cout << endl;
		}
		cout << "��ѯ�ɹ�,��" <<all.size()<< "��" << endl;
		return "80";
	}

	return "80";
}

string new_select_api(string tableName, vector<string> attrList, vector<condition> options)
{
	/*table *select_2(m_string tableName, int opt_num = 0, m_string column_name[] = NULL, m_string value[] = NULL, char opt[] = NULL, m_string res_name[] = NULL, int col_num = -1);*/
	record_manager rc;
	m_string column_name[15]; // �Ƚ�
	m_string res_name[15]; // ����
	m_string value[15];
	char opt[15];
	table *result = nullptr;
	int searchAttr = attrList.size(); //����
	int opt_num = options.size(); // �Ƚ�
	if (searchAttr == 0)
	{
		if (opt_num == 0)
		{
			result = rc.select_2(m_string(tableName));
		}
		else
		{
			for (int i = 0; i < opt_num; i++)
			{
				column_name[i] = m_string(options[i].attr);
				value[i] = m_string(options[i].value);
				char cond;
				switch (options[i].cond)
				{
				case BIG:
					cond = '>';
					break;
				case SMALL:
					cond = '<';
					break;
				case EQUAL:
					cond = '=';
					break;
				case NOTSMALL:
					cond = 'g';
					break;
				case NOTBIG:
					cond = 'l';
					break;
				case NOTEQUAL:
					cond = '!';
					break;
				default:
					break;
				}
				opt[i] = cond;
			}
			result = rc.select_2(m_string(tableName), opt_num, column_name, value, opt);
		}
	}
	else
	{
		for (int i = 0; i < searchAttr; i++)
		{
			res_name[i] = m_string(attrList[i]);
		}
		if (opt_num == 0)
		{
			result = rc.select_2(m_string(tableName), 0, NULL, NULL, NULL, res_name, searchAttr);
		}
		else
		{
			for (int i = 0; i < opt_num; i++)
			{
				for (int i = 0; i < opt_num; i++)
				{
					column_name[i] = m_string(options[i].attr);
					value[i] = m_string(options[i].value);
					char cond;
					switch (options[i].cond)
					{
					case BIG:
						cond = '>';
						break;
					case SMALL:
						cond = '<';
						break;
					case EQUAL:
						cond = '=';
						break;
					case NOTSMALL:
						cond = 'g';
						break;
					case NOTBIG:
						cond = 'l';
						break;
					case NOTEQUAL:
						cond = '!';
						break;
					default:
						break;
					}
					opt[i] = cond;
				}
				result = rc.select_2(m_string(tableName), opt_num, column_name, value, opt,res_name,searchAttr);
			}
		}
	}
	if (result->isError == 1)
	{
		cerr << "��ѯ���󣺲����ڵı���" << endl;
		return "99";
	}
	else
	{
		for (int i = 0; i < result->column_num; i++)
			cout << result->columns[i].column_name << "\t\t";
		cout << endl;
		for (int i = 0; i < result->row_num; i++)
		{
			for (int j = 0; j < result->column_num; j++)
				cout << result->rows[i]->data[j] << "\t\t";
			cout << endl;
		}
		return "80";
	}
}

int drop_table_api(string tableName)
{
	record_manager rc;
	m_string m_tableName(tableName.c_str());
	int result = rc.drop_table(m_tableName);
	return result;
}

string create_index_api(string tableName, string attr, string indexName)
{
	
	data_dictionary dt;
	database *db = dt.db;
	int flag = -1;
	for (int i = 0; i < db->tableNum; i++)
	{
		m_string temp(tableName.c_str());
		if (db->tables[i]->table_name == temp)
		{
			flag = i;
			break;
		}
	}
	if (flag == -1)
	{
		cerr << "������������û�д˱�" << endl;
		return "99";
	}
	else
	{
		trim(attr);
		string type;
		if (!isLegalName(attr))
		{
			cerr << "�������������������Ϸ�" << endl;
			return "99";
		}
		int result = dt.add_index(m_string(tableName),m_string(attr), m_string(indexName));
		switch (result)
		{
		case 0:
			// return "80";
			break;
		case 1:
			cout << "create index error: no such table" << endl;
			return "99";
		case 2:
			cout << "create index error: index already exists" << endl;
			return "99";
		case 3:
			cout << "create index error: no such attribute" << endl;
			return "99";
		default:
			return "99";
			break;
		}
		int i;
		for (i = 0; i < db->tables[flag]->column_num; i++)
		{
			if (db->tables[flag]->columns[i].column_name.str == attr)
			{
				type = db->tables[flag]->columns[i].data_type.str;
				break;
			}
		}
		/*if (i >= db->tables[flag]->column_num)
		{
			cerr << "�������������޴�����" << endl;
			return "99";
		}*/
		if (type == "char")
		{
			try {
				im.create_index(tableName + "$" + indexName, im.max_var_char);
			}
			catch (std::exception& e) {
#ifdef DEBUG
				cerr << e.what() << endl;
#endif // DEBUG

			}
		}
		else if (type == "int")
		{
			try {
				im.create_index(tableName + "$" + indexName, im.type_int);
			}
			catch (std::exception& e) {
#ifdef DEBUG
				cerr << e.what() << endl;
#endif // DEBUG

			}
		}
		else if (type == "float")
		{
			try {
				/*time_t t = time(NULL);
				t = t % 100;
				stringstream fuck;
				fuck << t;
				string shit;
				fuck >> shit;*/
				im.create_index(indexName, im.type_float);
			}
			catch (std::exception& e) {
#ifdef DEBUG
				cerr << e.what() << endl;
#endif // DEBUG

			}
		}
		else
		{
			cerr << "�ڲ����󣺴��������" << endl;
			return "99";
		}
		return "80";
	}
}

string drop_index_api(string indexName)
{
	data_dictionary dc;
	int result = dc.delete_index(m_string(indexName));
	switch (result)
	{
		case 0:
			break;
		case 1:
			cout << "drop index error: no such table" << endl;
			return "99";
		case 2:
			cout << "drop index error: no such index" << endl;
			return "99";
		default:
			return "99";
			break;
	}
	try {
		im.drop_index(indexName);
		cout << "drop index success" << endl;
		return "80";
	}
	catch (std::exception& e)
	{
		e.what();
		return "99";
	}
}

