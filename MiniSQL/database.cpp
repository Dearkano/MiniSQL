#include "database.h"
#include<iostream>
#include<fstream>

using namespace std;


database::database()
{

	FILE *fp;
	fp = fopen("./data_dictionary.dat", "rb");
	int table_num;
	table *tables = (table*)malloc(10 * sizeof(table));
	if (fp == NULL) {
		fp = fopen("./data_dictionary.dat", "wb");
		table_num = 0;
		fwrite(&table_num, sizeof(int), 1, fp);
	}
	else {
		fread(&table_num, sizeof(int), 1, fp);
		for (int i = 0; i < table_num; i++) {
			m_string tableName, columnNumStr, rowNumStr, indexNumStr;
			int columnNum, rowNum, indexNum;
			fread(tableName.str, sizeof(char), sizeof(m_string), fp);
			fread(&columnNum, sizeof(int), 1, fp);
			fread(&rowNum, sizeof(int), 1, fp);
			fread(&indexNum, sizeof(int), 1, fp);
			column *columns = (column*)malloc(columnNum * sizeof(column));
			m_string *indexNames = (m_string*)malloc(indexNum * sizeof(m_string));
			for (int i = 0; i < indexNum; i++) {
				m_string s;
				fread(s.str, sizeof(char), sizeof(m_string), fp);
				indexNames[i] = s;
			}
			for (int j = 0; j < columnNum; j++) {
				m_string columnName, dataType;
				int size = 0, unique, isPrimaryKey;

				fread(columnName.str, sizeof(char), sizeof(m_string), fp);
				fread(dataType.str, sizeof(char), sizeof(m_string), fp);
				fread(&size, sizeof(int), 1, fp);
				fread(&unique, sizeof(int), 1, fp);
				fread(&isPrimaryKey, sizeof(int), 1, fp);
				column *col = new column(columnName, dataType, size, unique, isPrimaryKey);
				columns[j] = *col;
			}
			table *tb = new table(tableName, columnNum, rowNum, columns, indexNum, indexNames);
			tables[i] = *tb;
		}
	}
	this->tables = tables;

	this->tableNum = table_num;
	fclose(fp);
}


database::~database()
{
}
