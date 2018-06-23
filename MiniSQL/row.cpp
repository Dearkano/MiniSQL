#include "row.h"



row::row()
{
}

row::row(char ** data,int col_num)
{
	this->data = new char*[col_num];
	for (int i = 0; i < col_num; i++) {
		this->data[i] = data[i];
	}
}
row::row(char **data, int col_num, int colId[]) {
	this->data = new char*[col_num];
	int t = 0;
	for (int i = 0; i < col_num; i++) {
		if(i==colId[t])
		this->data[t++] = data[i];
	}
}

row::~row()
{
}
