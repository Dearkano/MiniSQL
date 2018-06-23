#pragma once
class row
{
public:
	row();
	row(char **data,int col_num);
	row(char **data, int col_num, int colId[]); 
	~row();
	char **data;
};

