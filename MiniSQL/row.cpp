#include "row.h"



row::row()
{
}

row::row(m_string * data,int col_num)
{
	this->data = new m_string[col_num];
	for (int i = 0; i < col_num; i++) {
		this->data[i] = data[i];
	}
}
row::row(m_string *data, int col_num, int colId[]) {
	this->data = new m_string[col_num];
	int t = 0;
	for (int i = 0; i < col_num; i++) {
		if(i==colId[t])
		this->data[t++] = data[i];
	}
}

row::~row()
{
}
