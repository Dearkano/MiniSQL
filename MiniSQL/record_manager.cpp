#include"database.h"
#include"table.h"
#include"column.h"
#include"record_manager.h"
#include"catalog_manager.h"
#include"block_manager.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<string.h>


using namespace std;

char table1[][2][10] = {
	"001","name1",
	"002","name2",
	"003","name3",
	"004","name4"
};


char ***initData() {

	char table2[][4][10] = {
		"001","andy","computer","4.0",
		"002","peter","biology","3.1",
		"003","sam","chemistry","4.4"
	};
	char ***data;
	int row_num = 3;
	int column_num = 4;
	data = new char**[row_num + 1];
	for (int i = 0; i < row_num; i++) {
		data[i] = new char*[column_num];
		for (int j = 0; j < column_num; j++) {
			data[i][j] = new char[10];
			strcpy(data[i][j], table2[i][j]);
		}
	}
	return data;
}

table* record_manager::select(m_string tableName, m_string *columns, int columnNum, m_string column, m_string value, char opt) {

	database *db = this->dict.db;
	table *tb = new table();
	real_buffer_manager r;
	int rs = 0;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i]->table_name) {
			tb = db->tables[i];
			rs = 1;
			break;
		}
	}
	//如果表不存在
	if (rs == 0) {
		tb->isError = 1;
		return tb;
	}

	//申请buffer manager将这个表读入内存成为char [][][]形式
	//char ***data = buffer_manager.read(tableName);
	int rowIds[1000];
	m_string ** data = r.read_table(tb->table_name, tb->row_num, tb->column_num);
	int t = 0;
	m_string **newData;
	rs = 0;
	//检查约束条件
	if (opt != ' ') {
		int r = 0, c = 0, isInt = 0, isFloat = 0;;
		for (int i = 0; i < tb->column_num; i++) {
			if (column == tb->columns[i].column_name) {
				c = i;
				rs = 1;
				if (strcmp(tb->columns[i].data_type.str, "int") == 0)
					isInt = 1;
				if (strcmp(tb->columns[i].data_type.str, "float") == 0)
					isFloat = 1;
				break;
			}
		}
		if (rs == 0) {
			tb->isError = 2;
			return tb;
		}
		newData = new m_string*[tb->row_num];
		for (int i = 0; i < tb->row_num; i++) {
			newData[i] = new m_string[tb->column_num];
			string s;
			stringstream ss;
			int n, v;
			float f;
			switch (opt) {
			case '=':
				if (isInt || isFloat) {
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;
					if (isInt&&n == v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
					else if (isFloat&&f == v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				else {
					if (strcmp(value.str, data[i][c].str) == 0)
						newData[r] = data[i]; rowIds[r++] = i;
				}
				break;
			case'>':
				if (isInt || isFloat) {
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;

					if (isInt&&n > v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
					else if (isFloat&&f > v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				else {
					if (data[i][c] > value) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				break;
			case'<':
				if (isInt || isFloat) {
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;
					if (isInt&&n < v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
					else if (isFloat&&f < v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				else {
					if (data[i][c] < value) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				break;
			case'g':
				if (isInt || isFloat) {
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;
					if (isInt&&n >= v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
					else if (isFloat&&f >= v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				else {
					if (data[i][c] >= value) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				break;
			case'l':
				if (isInt || isFloat) {
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;
					if (isInt&&n <= v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
					else if (isFloat&&f <= v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				else {
					if (data[i][c] <= value) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				break;
			case'!':
				if (isInt || isFloat) {
					int n, v;
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;
					if (isInt&&n != v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
					else if (isFloat&&f != v) {
						newData[r] = data[i]; rowIds[r++] = i;
					}
				}
				else {
					if (strcmp(value.str, data[i][c].str) != 0)
						newData[r] = data[i]; rowIds[r++] = i;
				}
				break;
			}
		}
		tb->row_num = r;
	}
	else {
		newData = data;
	}

	// select *
	if (columnNum == 1 && strcmp(columns[0].str, "*") == 0) {
		for (int i = 0; i < tb->row_num; i++) {
			tb->rows[i] = new row(newData[i], tb->column_num);
			tb->rows[i]->id = rowIds[i];
		}
		//free(data);
		//delete data;
		return tb;
	}
	// select id name
	else {
		int colId[10];
		int t = 0;
		for (int i = 0; i < columnNum; i++) {
			for (int j = 0; j < tb->column_num; j++) {
				if (columns[i] == tb->columns[j].column_name) {
					colId[t++] = j;
					break;
				}
			}
		}

		for (int i = 0; i < tb->row_num; i++) {
			tb->rows[i] = new row(newData[i], tb->column_num, colId);
			tb->rows[i]->id = rowIds[i];
		}
		tb->column_num = columnNum;
		/*	free(data);
			free(newData);*/

			/*	for (int i = 0; i < tb->row_num; i++) {
					delete []newData[i];
				}*/
		return tb;
	}

}

int record_manager::add(m_string tableName, m_string* newRow)
{
	database *db;
	table *tb;
	real_buffer_manager r;
	int rs = 0;
	int t;
	try
	{
		db = this->dict.db;
		tb = new table();
		for (int i = 0; i < db->tableNum; i++) {
			if (tableName == db->tables[i]->table_name) {
				tb = db->tables[i];
				t = i;
				rs = 1;
				break;
			}
		}
	}
	catch (...)
	{
		cout << "错误3" << endl;
		return 4;
	}
	m_string **originData;
	int rnum;
	if (rs == 0)return -1;
	try
	{
		originData = r.read_table(tb->table_name, tb->row_num, tb->column_num);
		originData[tb->row_num] = new m_string[tb->column_num];
	}
	catch (...)
	{
		cout << "错误7" << endl;
		return 6;
	}
	//if(rs==0)throw error
	try
	{
		rnum = tb->row_num;
		data_dictionary d;
		for (int i = 0; i < rnum; i++) {
			for (int j = 0; j < tb->column_num; j++) {
				//主键或者唯一
				if (d.is_unique_or_pk(tb->table_name, j)) {
					if (newRow[j] == originData[i][j]) {
						delete tb;
						tb = NULL;
						return 1;
					}
				}
			}
		}
	}
	catch (...)
	{
		cout << "错误8" << endl;
	}
	try
	{

		for (int i = 0; i < tb->column_num; i++) {
			strcpy(originData[rnum][i].str, newRow[i].str);
		}


		//申请buffer manager将表更改后的内容存入硬盘
		int result = r.update_table(tb->table_name, originData, tb->row_num + 1, tb->column_num);
	}
	catch(...)
	{
		cout << "错误5" << endl;
		return 3;
	}

	try
	{
		//更新数据字典的行数
		this->dict.db->tables[t]->row_num++;
		this->dict.update_database();
		delete tb;
		tb = NULL;
		//delete originData;
		return 0;
	}
	catch (...)
	{
		cout << "错误7" << endl;
		return 2;
	}
	
}
int record_manager::drop_table(m_string tableName) {
	database *db = this->dict.db;
	table *tb = new table();
	int count = 0;
	int rs = 0;
	int t;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i]->table_name) {
			tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}
	if (rs == 0)return 1;
	string str(tableName.str);
	real_buffer_manager b;
	m_string mstr(str);
	b.delete_dbFile(mstr);
	b.create_dbFile(mstr);
	this->dict.delete_table(tableName);
	this->dict.update_database();
	return 0;
}
temp_row record_manager::select_row(m_string tableName, m_string column, m_string value, char opt)
{
	database *db = this->dict.db;
	int t = 0;
	table* tb = new table();
	int rs = 0;
	for (int i = 0; i < db->tableNum; i++) {
		if (db->tables[i]->table_name == tableName) {
			t = i;
			rs = 1;
			tb = db->tables[i];
			break;
		}
	}
	temp_row *tt = new temp_row();
	tt->num = -1;
	if (rs == 0) return *tt;
	real_buffer_manager r;
	m_string ** data = r.read_table(tb->table_name, tb->row_num, tb->column_num);

	int c = -1;
	int count = 0;
	int *res = new int[15000];
	if (opt == ' ') {
		for (int i = 0; i < tb->row_num; i++)
			res[i] = i;
		temp_row *tt = new temp_row();
		tt->row = res;
		tt->num = tb->row_num;
		return*tt;
	}

	for (int i = 0; i < tb->column_num; i++) {
		if (tb->columns[i].column_name == column) {
			c = i;
			break;
		}
	}
	tt->num = -2;
	if (c == -1)return *tt;

	int isFloat = 0, isInt = 0;
	if (strcmp("float", tb->columns[c].data_type.str) == 0) {
		isFloat = 1;
	}
	if (strcmp("int", tb->columns[c].data_type.str) == 0) {
		isInt = 1;
	}
	if (isFloat == 0&&isInt==0) {
		for (int i = 0; i < tb->row_num; i++) {
			switch (opt) {
			case '=':
				if (data[i][c] == value)
					res[count++] = i;
				break;
			case'>':
				if (data[i][c] > value)
					res[count++] = i;
				break;
			case '<':
				if (data[i][c] < value)
					res[count++] = i;
				break;
			case '!':
				if (data[i][c] != value)
					res[count++] = i;
				break;
			case 'g':
				if (data[i][c] >= value)
					res[count++] = i;
				break;
			case 'l':
				if (data[i][c] <= value)
					res[count++] = i;
				break;
			}
		}
	}
	else if (isInt == 1&&isFloat==0) {
		for (int i = 0; i < tb->row_num; i++) {
			int v1, v2;
			stringstream ss;
			ss << data[i][c];
			ss >> v1;
			ss.clear();
			ss << value;
			ss >> v2;
			switch (opt) {
			case '=':
				if (v1 == v2)
					res[count++] = i;
				break;
			case'>':
				if (v1 > v2)
					res[count++] = i;
				break;
			case '<':
				if (v1 < v2)
					res[count++] = i;
				break;
			case '!':
				if (v1 != v2)
					res[count++] = i;
				break;
			case 'g':
				if (v1 >= v2)
					res[count++] = i;
				break;
			case 'l':
				if (v1 <= v2)
					res[count++] = i;
				break;
			}
		}
	}
	else {
		for (int i = 0; i < tb->row_num; i++) {
			float v1, v2;
			stringstream ss;
			ss << data[i][c];
			ss >> v1;
			ss.clear();
			ss << value;
			ss >> v2;
			switch (opt) {
			case '=':
				if (v1 == v2)
					res[count++] = i;
				break;
			case'>':
				if (v1 > v2)
					res[count++] = i;
				break;
			case '<':
				if (v1 < v2)
					res[count++] = i;
				break;
			case '!':
				if (v1 != v2)
					res[count++] = i;
				break;
			case 'g':
				if (v1 >= v2)
					res[count++] = i;
				break;
			case 'l':
				if (v1 <= v2)
					res[count++] = i;
				break;
			}
		}
	}
	temp_row *tr = new temp_row();
	tr->num = count;
	tr->row = res;
	return *tr;
}
int record_manager::_delete(m_string tableName, m_string column, m_string value, char opt)
{
	database *db = this->dict.db;
	table *tb = new table();
	int count = 0;
	int rs = 0;
	int t;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i]->table_name) {
			tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}

	if (rs == 0) return -1;
	real_buffer_manager b;
	if (opt == ' ') {
		string str(tableName.str);
		m_string mstr(str);
		b.delete_dbFile(mstr);
		b.create_dbFile(mstr);
		int count = db->tables[t]->row_num;
		db->tables[t]->row_num = 0;
		//	this->dict.delete_table(tableName);
		this->dict.update_database();
		return count;
	}

	m_string **data = b.read_table(tb->table_name, tb->row_num, tb->column_num);

	/*
		检查约束列
	*/
	rs = 0;
	int r = 0, c = 0, isInt = 0, isFloat = 0;
	for (int i = 0; i < tb->column_num; i++) {
		if (column == tb->columns[i].column_name) {
			c = i;
			rs = 1;
			if (strcmp(tb->columns[i].data_type.str, "int") == 0)
				isInt = 1;
			if (strcmp(tb->columns[i].data_type.str, "float") == 0)
				isFloat = 1;
			break;
		}
	}
	if (rs == 0)return -2;
	//遍历搜索
	for (int i = 0; i < tb->row_num; i++) {
		m_string s;
		stringstream ss;
		int n, v;
		float f, f1;
		switch (opt) {
		case '=':
			if (isInt || isFloat) {
				s = data[i][c].str;
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				if (isInt == 1)
					ss >> v;
				if (isFloat == 1)
					ss >> f1;
				if (isInt&&n == v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
				else if (isFloat&&f == f1) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			else {
				if (strcmp(value.str, data[i][c].str) == 0) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			break;
		case'>':
			if (isInt || isFloat) {
				s = data[i][c].str;
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				if (isInt == 1)
					ss >> v;
				if (isFloat == 1)
					ss >> f1;
				if (isInt&&n > v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
				else if (isFloat&&f > f1) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			else {
				if (data[i][c] > value) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			break;
		case'<':
			if (isInt || isFloat) {
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				if (isInt == 1)
					ss >> v;
				if (isFloat == 1)
					ss >> f1;
				if (isInt&&n < v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
				else if (isFloat&&f < f1) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			else {
				if (data[i][c] < value) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			break;
		case'g':
			if (isInt || isFloat) {
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				if (isInt == 1)
					ss >> v;
				if (isFloat == 1)
					ss >> f1;
				if (isInt&&n >= v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
				else if (isFloat&&f >= f1) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			else {
				if (data[i][c] >= value) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			break;
		case'l':
			if (isInt || isFloat) {
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				if (isInt == 1)
					ss >> v;
				if (isFloat == 1)
					ss >> f1;
				if (isInt&&n <= v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			else if (isFloat&&f <= f1) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
				i = -1;
				count++;
			}
			else {
				if (data[i][c] <= value) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			break;
		case'!':
			if (isInt || isFloat) {
				int n, v;
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				if (isInt == 1)
					ss >> v;
				if (isFloat == 1)
					ss >> f1;
				if (isInt&&n != v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
				else if (isFloat&&f != f1) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			else {
				if (strcmp(value.str, data[i][c].str) != 0) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
					i = -1;
					count++;
				}
			}
			break;
		}
	}
	b.update_table(tb->table_name, data, tb->row_num, tb->column_num);
	/*
	data是新的文件
	更新数据词典
	*/
	this->dict.db->tables[t]->row_num = tb->row_num;
	this->dict.update_database();

	//申请buffer manager将表更改后的内容存入硬盘
	//将table重新变成char newData[][][]类型
	//int result = buffer_manager.update(tableName,newData);
	//free(data);
	delete tb;
	tb = NULL;
	return count;
}

temp_row mix(temp_row tr1, temp_row tr2) {
	int i = 0;
	int j = 0;
	int k = 0;
	int *mix = new int[1000];
	while (i < tr1.num && j < tr2.num)
	{
		if (tr1.row[i] == tr2.row[j])
		{
			mix[k++] = tr1.row[i];
			i++;
			j++;
		}
		else if (tr1.row[i] > tr2.row[j])
			j++;
		else if (tr1.row[i] < tr2.row[j])
			i++;
	}
	temp_row * tr = new temp_row();
	tr->num = k;
	tr->row = mix;
	return *tr;
}

int record_manager::_delete_2(m_string tableName, int opt_num, m_string column_name[], m_string value[], char opt[])
{

	database *db = this->dict.db;
	real_buffer_manager b;
	table *tb = new table();
	int rs = 0;
	int t;
	if (opt_num == 0) {
		string str(tableName.str);
		m_string mstr(str);
		b.delete_dbFile(mstr);
		b.create_dbFile(mstr);
		int count = db->tables[t]->row_num;
		db->tables[t]->row_num = 0;
		this->dict.update_database();
		return count;
	}
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i]->table_name) {
			tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}

	temp_row tr1 = this->select_row(tb->table_name, column_name[0], value[0], opt[0]);
	temp_row *tr = new temp_row();
	tr = &tr1;
	for (int i = 1; i < opt_num; i++) {
		temp_row tr2 = this->select_row(tb->table_name, column_name[i], value[i], opt[i]);
		*tr = mix(tr1, tr2);
		tr1.num = tr->num;
		tr1.row = tr->row;
	}

	int row_num = tr->num;
	int *rows = tr->row;


	m_string **data = b.read_table(tb->table_name, tb->row_num, tb->column_num);
	m_string **newData = new m_string*[tb->row_num - row_num];
	if (rs == 0)return 1;
	int p = 0;
	int j = 0;
	for (int i = 0; i < db->tables[t]->row_num; i++) {

		if (rows[j] == i) {
			j++;
		}
		else {
			newData[p++] = data[i];
		}
	}
	int pk = this->dict.db->tables[t]->row_num - p;
	b.update_table(tb->table_name, newData, p, tb->column_num);
	this->dict.db->tables[t]->row_num = p;
	this->dict.update_database();
	return pk;
}
table* record_manager::select_2(m_string tableName, int opt_num, m_string column_name[], m_string value[], char opt[], m_string res_name[], int col_num)
{

	database *db = this->dict.db;
	int rs = 0;
	table *tb = new table();
	int t;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i]->table_name) {
			tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}
	if (rs == 0) {
		tb->isError = 1;
		return tb;
	}
	real_buffer_manager b;
	m_string **data1 = b.read_table(tb->table_name, tb->row_num, tb->column_num);
	m_string **data = new m_string*[tb->row_num];
	for (int i = 0; i < tb->row_num; i++) data[i] = data1[i];
	temp_row *tr = new temp_row();
	if (opt_num != 0) {
		temp_row tr1 = this->select_row(tb->table_name, column_name[0], value[0], opt[0]);
		*tr = tr1;
		for (int i = 1; i < opt_num; i++) {
			temp_row tr2 = this->select_row(tb->table_name, column_name[i], value[i], opt[i]);
			*tr = mix(tr1, tr2);
			tr1.num = tr->num;
			tr1.row = tr->row;
		}
	}
	else {
		*tr = this->select_row(tb->table_name);
	}

	int row_num = tr->num;
	int *rows = tr->row;

	table *_table = new table();
	_table->row_num = row_num;
	_table->columns = tb->columns;
	_table->index_num = tb->index_num;
	_table->index_names = tb->index_names;
	if (col_num == -1)
		_table->column_num = tb->column_num;
	else
		_table->column_num == col_num;

	//select *
	if (col_num == -1) {
		int p = 0;
		for (int i = 0; i < tb->row_num; i++) {
			if (i == rows[p]) {
				_table->rows[p++] = new row(data[i], tb->column_num);
			}
		}
	}
	else {
		int colId[10];
		int t = 0;
		for (int i = 0; i < col_num; i++) {
			for (int j = 0; j < tb->column_num; j++) {
				if (res_name[i] == tb->columns[j].column_name) {
					colId[t] = j;
					_table->columns[t++] = tb->columns[j];
					break;
				}
			}
		}
		_table->column_num = t;
		int p = 0;
		for (int i = 0; i < tb->row_num; i++) {
			if (i == rows[p]) {
				_table->rows[p++] = new row(data[i], tb->column_num, colId);
			}
		}
	}
	delete data;
	delete tr;
	delete data1;
	delete tb;
	return _table;
}
int record_manager::update(m_string tableName,int opt_num ,m_string column1, m_string value1 , m_string *column2, m_string *value2,char *opt)
{
	database *db = new database();
	table *tb = new table();
	real_buffer_manager b1;
	int rs = 0;
	int t;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i]->table_name) {
			tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}

	// if(rs==0) throw error;

	/*
	检查更新列
	*/
	int t1 = -1;
	for (int i = 0; i < tb->column_num; i++) {
		if (column1 == tb->columns[i].column_name) {
			t1 = i;
			break;
		}
	}
	if (t1 == -1)return 1;
	

	real_buffer_manager b;
	m_string **data1 = b.read_table(tb->table_name, tb->row_num, tb->column_num);
	m_string **data = new m_string*[tb->row_num];
	for (int i = 0; i < tb->row_num; i++) data[i] = data1[i];
	temp_row *tr = new temp_row();
		temp_row tr1 = this->select_row(tb->table_name, column2[0], value2[0], opt[0]);
		*tr = tr1;
		for (int i = 1; i < opt_num; i++) {
			temp_row tr2 = this->select_row(tb->table_name, column2[i], value2[i], opt[i]);
			*tr = mix(tr1, tr2);
			tr1.num = tr->num;
			tr1.row = tr->row;
		}

		for (int i = 0; i < tr->num; i++) {
			data[tr->row[i]][t1] = value1;
		}

	/*
	data是新的文件
	更新数据词典*/
	m_string **d = new m_string*[tb->row_num];
	for (int i = 0; i < tb->row_num; i++) {
		d[i] = new m_string[tb->column_num];
		for (int j = 0; j < tb->column_num; j++) {
			d[i][j] = data[i][j];
		}
	}
	real_buffer_manager r;
	r.update_table(tb->table_name, d, tb->row_num, tb->column_num);
	delete d;
	delete data;
	delete tb;
	tb = NULL;
	return 0;
}





record_manager::record_manager()
{
}
/*

int main() {
	real_buffer_manager b;
	m_string str("table2");
	//b.create_dbFile(str);
	m_string s1("0004");
	m_string s2("asuka");
	m_string s3("math");
	m_string s4("2.7");
	m_string s5("saito asuka");
	m_string tbName("table2");
	m_string cl("name");
	m_string *s = new m_string[4]{ s1,s2,s3,s4 };
	record_manager rm;
	int rs = 0;
	//for(int i=0;i<5;i++)
	rs = rm.add(tbName, s);
	if (rs == 1)cout << "添加失败 不唯一" << endl;
	//rm._delete(tbName, cl, s2, '=');
	rm.update(tbName, cl, s5, cl, s2,'=');
	m_string tableName;
	strcpy(tableName.str, "table2");
	table *tb = rm.select(tableName);
	for (int i = 0; i < tb->row_num; i++) {
		for (int j = 0; j < tb->column_num; j++) {
			cout << tb->rows[i]->data[j] << " ";
		}
		cout << endl;
	}
	free(s);
	system("pause");
}
*/
