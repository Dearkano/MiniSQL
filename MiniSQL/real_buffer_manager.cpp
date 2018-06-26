#include "real_buffer_manager.h"
#include<iostream>
#include<fstream>
#include<string.h>
#include<sstream>
#include "record_manager.h"
using namespace std;

int real_buffer_manager::create_dbFile(m_string tableName)
{
	ofstream outf;
	string str(tableName.str);
	string filepath = "./dbfile/" + str + ".dat";
	outf.open(filepath);
	outf.close();
	return 0;
}

int real_buffer_manager::delete_dbFile(m_string tableName)
{
	string str(tableName.str);
	string filepath = "./dbfile/" + str + ".dat";
	const char* c = filepath.data();
	remove(c);
	return 0;
}

m_string** real_buffer_manager::read_table(m_string tableName, int rowNum, int colNum)
{
	string str(tableName.str);
	string filepath = "./dbfile/" + str + ".dat";
	FILE *fp = fopen(filepath.data(), "rb");
	m_string **data = new m_string *[rowNum];
	for (int i = 0; i < rowNum; i++) {
		data[i] = new m_string[colNum];
		for (int j = 0; j < colNum; j++) {
			fread(data[i][j].str, sizeof(char), 256, fp);
		}
	}
	fclose(fp);
	return data;
}

int real_buffer_manager::update_table(m_string tableName, m_string ** data, int rowNum, int colNum)
{
	string str(tableName.str);
	string filepath = "./dbfile/" + str + ".dat";
	FILE *fp = fopen(filepath.data(),"wb");

	for (int i = 0; i < rowNum; i++) {
		for (int j = 0; j < colNum; j++) {
			fwrite(data[i][j].str, sizeof(char), 256, fp);
		}		
	}

	fclose(fp);
	return 0;
}
//int main() {
//
//	//m_string **d = new m_string*[3];
//	//for (int i = 0; i < 3; i++) {
//	//	d[i] = new m_string[4];
//	//	for (int j = 0; j < 4; j++) {
//	//		string s1, s2;
//	//		stringstream ss;  //创建一个格式化输出流
//	//		ss << i;
//	//		ss >> s1;
//	//		ss.clear();
//	//		ss << j;
//	//		ss >> s2;
//	//		string s = "row" + s1 + " col" + s2;			
//	//		//m_string m;
//	//		const char *c = s.c_str();
//	//		//strcpy(m.str, c);
//	//		strcpy(d[i][j].str ,c);
//	//	}
//	//}
//	real_buffer_manager r;
//	//m_string str;
//	//strcpy(str.str, "table");
//	//r.create_dbFile(str);
//	//r.update_table(str,d,3,4);
//
//
//	//record_manager r;
//	//m_string s1("0001");
//	//m_string s2("andy");
//	//m_string s3("computer science");
//	//m_string s4("c4");
//	//m_string tbName("table2");
//	//m_string *s = new m_string[4]{ s1,s2,s3,s4 };
//	//r.add(tbName, s);
//	m_string s("table2");
//	r.read_table(s,1,4);
//}