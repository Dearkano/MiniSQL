#include "api.h"
#include "real_buffer_manager.h"
#include <cstring>

int create_table_api(InterTable newTable)
{
	data_dictionary dt;
	map<string, attrInfo>::iterator iter;
	const size_t columnNum = newTable.attrList.size();
	int ptr = 0;
	column col[15];
	iter = newTable.attrList.begin();
	// ���������Ϣ
	m_string tableName(newTable.tableName);
	while (iter != newTable.attrList.end())
	{
		m_string attrName(iter->second.attrName.c_str());
		m_string attrType(iter->second.attrType.c_str());
		int attrSize = iter->second.length;
		RESTRICT res = iter->second.resType;
		bool pk = false;
		if (res == PRKEY)
			pk = true;
		bool unique = iter->second.unique;
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
		if (db->tables[i].table_name == temp)
		{
			flag = i;
			break;
		}
	}
	if (flag != -1)
	{
		if (valueList.size() != db->tables[flag].column_num)
		{
			cerr << "value���󣺲����б��Ȳ�һ��"  << endl;
			return -1;
		}
		m_string value[15];
		int ptr = 0;
		for (int i = 0; i < db->tables[flag].column_num; i++)
		{
			m_string type = db->tables[flag].columns[i].data_type;
			if (strcmp (type.str, "char") == 0)
			{
				trim(valueList[i]);
				if (valueList[i][0] != '\'' || valueList[i][valueList[i].length() - 1] != '\'')
				{
					cerr << "value���󣺲���" + valueList[i] + "���ʹ���char��ʽ����" << endl;
					return -1;
				}
				int size = db->tables[flag].columns[i].data_size;
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
	if (option.size() == 0)
	{
		return -3;
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