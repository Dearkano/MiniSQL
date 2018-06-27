#pragma once
#include<string>
#include "column.h"
#include "row.h"
#include"m_string.h"
using namespace std;
class table
{
public:
	table();
	table(m_string table_name, int column_num, int row_num, column *columns,int index_num=0,m_string *index_names=NULL);
	~table();
	m_string table_name;
	int column_num;
	int row_num;
	int index_num;
	m_string *index_names;
	column *columns;
	int isError;
	row *rows[];
};

