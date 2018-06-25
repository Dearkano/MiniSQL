#pragma once
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace boost;




// 限制种类
enum RESTRICT { DEFAULT, PRKEY, FRKEY, ERR };
enum COD {BIG, SMALL, EQUAL, NOTSMALL, NOTBIG, NOTEQUAL};
// 合法类型


// 属性定义

struct attrInfo
{
	string attrName;
	string attrType;
	int length;        // 专供char, int和float该值为-1
	bool unique;       // 默认为false， 如果是主键，unique必为true
	RESTRICT resType;  // 默认为DEFAULT
	string reference;  // 专供foreign key，primary key 和 default 的该值为空串
};

// 表定义

struct InterTable
{
	string tableName;
	string prKey;    // 主键
	map<string, attrInfo> attrList;
};

struct condition
{
	string attr;
	COD cond;
	string value;
};

//条件定义

// 相关函数

string Interpreter(string statement = "");

string Input();

void RemoveSpace(string& s,  const string mark = " ");

void RemoveTabSpace(string &s);

string Create(string sql);

string Drop(string sql);

// 查询模块
string Select(string sql);
string Insert(string sql);
string Delete(string sql);
string ExecFile(string sql);
vector<string> ProcessValue(string info);

 // 建表模块

 string CreateTable(string sql);
 string CreateIndex(string sql);

 attrInfo ProcessInfo(string info); // info格式正确返回完整的attrInfo, 否则restrict被置为ERR.
 vector<condition> ProcessWhere(string sql);

string DropTable(string sql);
// 建索引模块

string DropIndex(string sql);

vector<string> ValueSplit(string input);

vector<condition> WhereSplit(string input);

bool IsLegalInt(string a);
bool IsLegalFloat(string a);


# endif
