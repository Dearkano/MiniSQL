#include "api.h"
#include "real_buffer_manager.h"
#include <cstring>
#include <set>
#include <algorithm>
#include"IndexManager.h"

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
// ��condition������
// ֻ��һ�����
table* select_from_api(string tableName, vector<string> attrList, condition opt)
{
	m_string attr[50];
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
	m_string attr[50];
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
		all.clear();
		vector<int> stack;
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
			cout << attr[i] << "\t\t\t|";
		cout << endl;
		for (int i = 0; i < all.size(); i++)
		{
			for (int j = 0; j < all[i].size(); j++)
				cout << all[i][j] << "\t\t\t|";
			cout << endl;
		}

	}

	return "80";
}


int drop_table_api(string tableName)
{
	m_string m_tableName(tableName.c_str());
	return 0;

}