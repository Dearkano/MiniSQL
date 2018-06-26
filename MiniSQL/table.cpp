#include "table.h"
#include <stdlib.h>;
table::table() {
	columns = new column[15];
	index_names = new m_string[15];
}

table::table(m_string table_name, int column_num, int row_num, column *columns, int index_num, m_string *index_names)
{
	this->isError = 0;
	this->table_name = table_name;
	this->column_num = column_num;
	this->row_num = row_num;
	this->columns = columns;
	this->index_num = index_num;
	this->index_names = index_names;
}

table::~table()
{
	//delete index_names;
	//delete columns;
}
