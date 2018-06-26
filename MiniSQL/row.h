#pragma once
#include"m_string.h"
class row
{
public:
	row();
	row(m_string *data,int col_num);
	row(m_string *data, int col_num, int colId[]); 
	~row();
	m_string *data;
	int id;
};

