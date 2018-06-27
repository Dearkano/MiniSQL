#include "row.h"



row::row()
{
}

row::row(m_string * data,int col_num)
{
	this->data = new m_string[col_num];
	for (int i = 0; i < col_num; i++) {
		m_string s = data[i];
		this->data[i] = s;
	}
	//this->data = data;
}
row::row(m_string *data, int col_num, int colId[]) {
	this->data = new m_string[col_num];
	int t = 0;
	for (int i = 0; i < col_num; i++) {
		if (i == colId[t]) {
			m_string *s= new m_string(data[i]); 
			this->data[t++]=*s;
			i = -1;
			continue;
		}
	}
}

row::~row()
{
	delete[]this->data;
}