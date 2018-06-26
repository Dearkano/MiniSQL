#include "database.h"
#include<iostream>
#include<fstream>

using namespace std;


database::database()
{

	FILE *fp;
	fp = fopen("./data_dictionary.dat", "rb");
	int table_num;
	table **tables =new table*[10];
	if (fp == NULL) {
		fp = fopen("./data_dictionary.dat", "wb");
		table_num = 0;
		fwrite(&table_num, sizeof(int), 1, fp);
	}
	else {
		fread(&table_num, sizeof(int), 1, fp);
		for (int i = 0; i < table_num; i++) {
			tables[i] = new table();
			int columnNum, rowNum, indexNum;
			fread(tables[i]->table_name.str, sizeof(char), sizeof(m_string), fp);
			fread(&tables[i]->column_num, sizeof(int), 1, fp);
			fread(&tables[i]->row_num, sizeof(int), 1, fp);
			fread(&tables[i]->index_num, sizeof(int), 1, fp);
			//column *columns = new column[columnNum];
			//m_string *indexNames = new m_string[indexNum];
			for (int i = 0; i < tables[i]->index_num; i++) {
				fread(tables[i]->index_names[i].str, sizeof(char), sizeof(m_string), fp);
			}
			for (int j = 0; j < tables[i]->column_num; j++) {
				m_string columnName, dataType;
				int size = 0, unique, isPrimaryKey;
				fread(columnName.str, sizeof(char), sizeof(m_string), fp);
				fread(dataType.str, sizeof(char), sizeof(m_string), fp);
				fread(&size, sizeof(int), 1, fp);
				fread(&unique, sizeof(int), 1, fp);
				fread(&isPrimaryKey, sizeof(int), 1, fp);
				tables[i]->columns[j] = *new column(columnName, dataType, size, unique, isPrimaryKey);
			}
			//tables[i] = new table(tableName, columnNum, rowNum, columns, indexNum, indexNames);
		}
	}
	this->tables = tables;
	this->tableNum = table_num;

	fclose(fp);
}


database::~database()
{
	delete this->tables;
}
