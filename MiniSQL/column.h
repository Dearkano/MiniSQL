#pragma once
#include<string>
#include "m_string.h"
using namespace std;


class column
{
public:
	
	column() {};
	column(m_string column_name, m_string data_type, int data_size, bool unique = false, bool isPrimaryKey = false);
	~column();
	//列名
	m_string column_name;
	//类型
	m_string data_type;
	//大小
	int data_size;
	//唯一
	bool unique;
	//主键
	bool isPrimaryKey;
	
};