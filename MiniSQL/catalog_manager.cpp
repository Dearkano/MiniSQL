#include"database.h"
#include"table.h"
#include"column.h"
#include "catalog_manager.h"
#include<iostream>
#include<fstream>
#include<string>
#include<string.h>

using namespace std;
int data_dictionary::create_table(table table) {

	//--------------预检查
	database *db = this->db;
	for (int i = 0; i < db->tableNum; i++) {
		if (table.table_name== db->tables[i]->table_name)
			return 1;
	}

	//--------------预检结束
	if (db->tableNum < 0)db->tableNum = 0;
	db->tables[db->tableNum] = &table;
	db->tableNum++;
	this->update_database();
	return 0;
}
int data_dictionary::delete_table(m_string tableName) {

	database *db = this->db;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName==db->tables[i]->table_name) {
			for (int j = i; j < db->tableNum-1; j++) {
				db->tables[j] = db->tables[j + 1];
			}
			db->tableNum--;
			this->update_database();
			return 0;
		}
	}
	return 1;

}
int data_dictionary::add_column(m_string tableName,column column) {
	database *db = this->db;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName==db->tables[i]->table_name) {
			//-----预检查
			for (int j = 0; j < db->tables[i]->column_num; j++) {
				if (db->tables[i]->columns[j].column_name==column.column_name)
					return 2;
			}
			//-----预检结束
			
			db->tables[i]->columns[db->tables[i]->column_num] = column;
			db->tables[i]->column_num++;
			this->update_database();
			return 0;
		}
	}
	return 1;
}
int data_dictionary::delete_column(m_string tableName,m_string columnName) {
	database *db = this->db;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName==db->tables[i]->table_name) {
			table tb = *db->tables[i];
			for (int j = 0; j < tb.column_num; j++) {
				if (columnName== tb.columns[j].column_name) {
					for (int k = j; k < tb.column_num - 1; k++) {
						tb.columns[k] = tb.columns[k + 1];
					}
					tb.column_num--;
					db->tables[i] = &tb;
					this->update_database();
					return 0;
				}
			}
			return 2;
		}
	}
	return 1;
}
int data_dictionary::add_index(m_string tableName, m_string indexName)
{
	database *db = this->db;
	int rs = 0; int t = 0;
	table tb;
	//检查表是否存在
	for (int i = 0; i < db->tableNum; i++) {
		tb = *db->tables[i];
		if (tb.table_name == tableName) {
			rs = 1;
			t = i;
			break;
		}
	}
	if (rs == 0) return 1;
	//检查索引是否存在
	rs = 0;
	for (int i = 0; i < tb.index_num; i++) {
		if (tb.index_names[i] == indexName) {
			rs = 1;
			break;
		}
	}
	if (rs == 1)return 2;

	rs = 0;
	//检查列是否存在
	for (int i = 0; i < tb.column_num; i++) {
		if (tb.columns[i].column_name == indexName) {
			rs = 1;
			break;
		}
	}
	if (rs == 0)return 3;
	//插入新索引

	m_string *indexes = new m_string[tb.index_num + 1];
	for (int i = 0; i < tb.index_num ; i++) {
		indexes[i] = tb.index_names[i];
	}
	indexes[tb.index_num] = indexName;
	tb.index_names = indexes;
	tb.index_num++;
	db->tables[t] = &tb;
	this->update_database();
	this->db = new database();
	return 0;
}
int data_dictionary::delete_index(m_string tableName, m_string indexName)
{
	database *db = this->db;
	int rs = 0; int t = 0;
	table tb;
	//检查表是否存在
	for (int i = 0; i < db->tableNum; i++) {
		tb = *db->tables[i];
		if (tb.table_name == tableName) {
			rs = 1;
			t = i;
			break;
		}
	}
	if (rs == 0) return 1;
	//检查索引是否存在
	rs = 0;
	int ix = 0;
	for (int i = 0; i < tb.index_num; i++) {
		if (tb.index_names[i] == indexName) {
			rs = 1;
			ix = i;
			break;
		}
	}
	if (rs == 0)return 2;

	
	//删除索引

	m_string *indexes = new m_string[tb.index_num - 1];
	for (int i = 0; i < ix; i++) {
		indexes[i] = tb.index_names[i];
	}
	for (int i = ix; i <tb.index_num-1 ; i++) {
		indexes[i] = tb.index_names[i+1];
	}
	tb.index_names = indexes;
	tb.index_num--;
	db->tables[t] =&tb;
	this->update_database();
	this->db = new database();
	return 0;
}
void data_dictionary::build_dictionary() {
	ofstream outf;
	outf.open("./data_dictionary.txt");
	outf << 0 << " ";
	outf.close();
}
void data_dictionary::listTable() {
	database *db = this->db;
	for (int i = 0; i < db->tableNum; i++) {
		table tb = *db->tables[i];
		cout.setf(std::ios::left);
		cout << "表名:" << tb.table_name << endl << endl;
		cout.width(10);
		cout << "列名";
		cout.width(10);
		cout << "类型";
		cout.width(10);
		cout << "大小";
		cout.width(10);
		cout << "索引数";
		for (int i = 1; i <= tb.index_num; i++) {
			cout.width(4);
			cout << "索引";
			cout.width(6);
			cout << i;
		}
	
		cout.width(10);
		cout << "唯一";
		cout.width(10);
		cout << "主键"<<endl;
		for (int j = 0; j <tb.column_num; j++) {
			column col = tb.columns[j];
			char s3[6],s4[6];			
			if (col.unique == 0) {
				strcpy(s3, "false");
			}
			else {
				strcpy(s3, "true");
			}
			if (col.isPrimaryKey == 0) {
				strcpy(s4, "false");
			}
			else {
				strcpy(s4, "true");
			}
			cout.width(10);
			cout << col.column_name;
			cout.width(10);
			cout << col.data_type;
			cout.width(10);
			cout << col.data_size;
			cout.width(10);
			cout << tb.index_num;
			for (int i = 0; i < tb.index_num; i++) {
				cout.width(10);
				cout << tb.index_names[i];
			}		
			cout.width(10);
			cout << s3;
			cout.width(10);
			cout << s4<<endl;
		}
		cout << endl;
	}
}
int data_dictionary::is_unique_or_pk(m_string tableName, int colNum)
{
	database *db = this->db;
	for (int i = 0; i < db->tableNum; i++) {
		if (db->tables[i]->table_name == tableName) {
			column col = db->tables[i]->columns[colNum];
			if (col.isPrimaryKey || col.unique)return 1;
			return 0;
		}
	}

	return 0;
}
void data_dictionary::update() {
	//delete this->db;
	this->db = new database();
}
data_dictionary::data_dictionary() {
	this->db = new database();
}

void data_dictionary::update_database() {
	database *db = this->db;
	remove("./data_dictionary.dat");
	FILE *fp;
	try {
		fp=fopen("./data_dictionary.dat","wb");
		fwrite(&this->db->tableNum, sizeof(int), 1, fp);
	}
	catch (exception e) {
		cout << "error" << endl;
	}

	for (int i = 0; i < db->tableNum; i++) {
		table table = *db->tables[i];
		//写入表名
		fwrite(table.table_name.str, sizeof(char),256, fp);

		//写入列数 行数
		fwrite(&table.column_num, sizeof(int), 1, fp);
		fwrite(&table.row_num, sizeof(int), 1, fp);
		//写入索引数，索引
		fwrite(&table.index_num, sizeof(int), 1, fp);
		for (int i = 0; i < table.index_num; i++) {
			fwrite(table.index_names[i].str, sizeof(char), 256, fp);
		}
		//写入列属性
		for (int i = 0; i < table.column_num; i++) {

			//写入列名
			fwrite(table.columns[i].column_name.str, sizeof(char), 256, fp);

			//写入数据类型
			fwrite(table.columns[i].data_type.str, sizeof(char), 256, fp);

			//写入数据长度
			fwrite(&table.columns[i].data_size, sizeof(int), 1, fp);

			//写入是否唯一
			fwrite(&table.columns[i].unique, sizeof(int), 1, fp);

			//写入是否主键
			fwrite(&table.columns[i].isPrimaryKey, sizeof(int), 1, fp);

		}
	}
	fclose(fp);
}

table build_table_demo(m_string tbName) {
	m_string name,type;
	strcpy(name.str , "id");
	strcpy(type.str, "int");
	int size = 32;
	column column1(name,type,size);
	m_string name1, type1;
	strcpy(name1.str, "name");
	strcpy(type1.str, "varchar");
	column column2(name1, type1, size);
	m_string name2, type2;
	strcpy(name2.str, "major");
	strcpy(type2.str, "varchar");
	column column3(name2, type2, size);

	m_string name3, type3;
	strcpy(name3.str, "gpa");
	strcpy(type3.str, "float");
	column column4(name3, type3, size);
	column columns[4] = {column1,column2,column3,column4};
	table t(tbName, 4, 0, columns);
	return t;
}
column build_column_demo(m_string column_name) {
	m_string data_type;
	strcpy(data_type.str, "int");
	int data_size = 32;
	bool unique = true;
	bool isPrimaryKey = false;
	column col (column_name,data_type,data_size, unique, isPrimaryKey);
	return col;
}
void test_add_column() {
	data_dictionary d;
	m_string colName,tbName;
	strcpy(colName.str, "col1");
	column col = build_column_demo(colName);
	strcpy(tbName.str, "table1");
	int rs = d.add_column(tbName, col);
	if (rs == 0)cout << "添加新列成功" << endl;
	else if(rs==1)cout << "添加失败，表不存在！" << endl;
	else cout << "添加失败，列已存在！" << endl;
	d.update();
	d.listTable();
}
void test_delete_column() {
	data_dictionary d;
	m_string t, n;
	strcpy(t.str, "table1");
	strcpy(n.str, "col1");
	int rs = d.delete_column(t, n);
	if (rs == 0)cout << "删除列成功" << endl;
	else if(rs==1) cout << "删除失败，表不存在！" << endl;
	else cout << "删除失败，列不存在！" << endl;
	d.update();
	d.listTable();
}
void test_create_table() {
	data_dictionary d;
	m_string tbName;
	strcpy(tbName.str, "table2");
	table t = build_table_demo(tbName);
	int rs = d.create_table(t);
	if (rs == 0)cout << "建表成功" << endl;
	else cout << "建表失败，表名重复！" << endl;
	d.update();
	d.listTable();
}
void test_delete_table() {
	data_dictionary d;
	m_string t;
	strcpy(t.str, "table1");
	int rs = d.delete_table(t);
	if (rs == 0)cout << "删除成功" << endl;
	else cout << "删除失败，表不存在！" << endl;
	d.update();
	d.listTable();
}
void test_add_index() {
	m_string tableName, indexName;
	strcpy(tableName.str, "table1");
	strcpy(indexName.str, "name");
	data_dictionary d;
	int rs = d.add_index(tableName, indexName);
	if (rs == 0) {
		cout << "添加索引成功" << endl;
	}
	else if (rs == 1) {
		cout << "添加失败 表不存在" << endl;
	}
	else if(rs == 2){
		cout << "添加失败 索引已经存在" << endl;
	}
	else {
		cout << "添加失败 列不存在" << endl;
	}
	d.update();
	d.listTable();
}

void test_delete_index() {
	m_string tableName, indexName;
	strcpy(tableName.str, "table1");
	strcpy(indexName.str, "name");
	data_dictionary d;
	int rs = d.delete_index(tableName, indexName);
	if (rs == 0) {
		cout << "删除索引成功" << endl;
	}
	else if (rs == 1) {
		cout << "删除失败 表不存在" << endl;
	}
	else if (rs == 2) {
		cout << "删除失败 索引不存在" << endl;
	}
	d.update();
	d.listTable();
}
void show_database() {
	data_dictionary d;
	d.listTable();
}
void init() {
	test_create_table();
	//test_delete_table();
	//test_add_column();
	//test_delete_column();
	//test_add_index();
	//test_delete_index();
	//show_database();
	
}

//int main() {
//	init();	
//	system("pause");
//}