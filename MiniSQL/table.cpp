#include "table.h"
#include <stdlib.h>;
table::table() { this->isError = 0; }
table::table(m_string table_name, int column_num, int row_num, column *columns,int index_num,m_string *index_names)
{
	this->isError = 0;
	this->table_name= table_name;
	this->column_num = column_num;
	this->row_num = row_num;
	column *cls = new column[15];
	for (int i = 0; i < column_num; i++) {
		cls[i] = columns[i];
	}
	this->index_num = index_num;
	this->index_names = new m_string[index_num];
	for (int i = 0; i < index_num; i++) {
		this->index_names[i] = index_names[i];
	}
	this->columns = cls;
}

table::~table()
{
}
