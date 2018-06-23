#include "column.h"

column::column(m_string column_name, m_string data_type, int data_size,  bool unique, bool isPrimaryKey)
{
	this->column_name=column_name;
	this->data_size = data_size;
	this->unique = unique;
	this->isPrimaryKey = isPrimaryKey;
	this->data_type=data_type;
}

column::~column()
{
}
