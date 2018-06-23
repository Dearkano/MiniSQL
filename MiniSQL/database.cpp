#include "database.h"
#include<iostream>
#include<fstream>

using namespace std;


database::database()
{

	ifstream inf;
	inf.open("./data_dictionary.txt");
	int table_num;
	inf >> table_num;
	table *tables = (table*)malloc(10 * sizeof(table));
	for (int i = 0; i < table_num; i++) {
		m_string tableName, columnNumStr, rowNumStr,indexNumStr;
		inf >> tableName;
		inf >> columnNumStr;
		inf >> rowNumStr;
		inf >> indexNumStr;
		int columnNum, rowNum,indexNum;
		sscanf(columnNumStr.str, "%d", &columnNum);
		sscanf(rowNumStr.str, "%d", &rowNum);
		sscanf(indexNumStr.str, "%d", &indexNum);
		column *columns = (column*)malloc(15 * sizeof(column));
		m_string *indexNames = (m_string*)malloc(indexNum*sizeof(m_string));
		for (int i = 0; i < indexNum; i++) {
			m_string s;
			inf >> s;
			indexNames[i] = s;
		}
		for (int j = 0; j < columnNum; j++) {
			m_string columnName, dataType, sizeStr;
			int size = 0, unique, isPrimaryKey;

			inf >> columnName;
			inf >> dataType;
			inf >> sizeStr;
			inf >> unique;
			inf >> isPrimaryKey;		
			sscanf(sizeStr.str, "%d", &size);
			column *col = new column(columnName, dataType, size,unique, isPrimaryKey);
			columns[j] = *col;
		}
		 table *tb= new table(tableName, columnNum, rowNum, columns,indexNum,indexNames);
		 tables[i] = *tb;
	}
	this->tables = tables;
	this->tableNum = table_num;
	inf.close();
}


database::~database()
{
}
