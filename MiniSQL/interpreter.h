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




// ��������
enum RESTRICT { DEFAULT, PRKEY, FRKEY, ERR };
enum COD {BIG, SMALL, EQUAL, NOTSMALL, NOTBIG, NOTEQUAL};
// �Ϸ�����


// ���Զ���

struct attrInfo
{
	string attrName;
	string attrType;
	int length;        // ר��char, int��float��ֵΪ-1
	bool unique;       // Ĭ��Ϊfalse�� �����������unique��Ϊtrue
	RESTRICT resType;  // Ĭ��ΪDEFAULT
	string reference;  // ר��foreign key��primary key �� default �ĸ�ֵΪ�մ�
};

// ����

struct InterTable
{
	string tableName;
	string prKey;    // ����
	map<string, attrInfo> attrList;
};

struct condition
{
	string attr;
	COD cond;
	string value;
};

//��������

// ��غ���

string Interpreter(string statement = "");

string Input();

void RemoveSpace(string& s,  const string mark = " ");

void RemoveTabSpace(string &s);

string Create(string sql);

string Drop(string sql);

// ��ѯģ��
string Select(string sql);
string Insert(string sql);
string Delete(string sql);
string ExecFile(string sql);
vector<string> ProcessValue(string info);

 // ����ģ��

 string CreateTable(string sql);
 string CreateIndex(string sql);

 attrInfo ProcessInfo(string info); // info��ʽ��ȷ����������attrInfo, ����restrict����ΪERR.
 vector<condition> ProcessWhere(string sql);

string DropTable(string sql);
// ������ģ��

string DropIndex(string sql);

vector<string> ValueSplit(string input);

vector<condition> WhereSplit(string input);

bool IsLegalInt(string a);
bool IsLegalFloat(string a);


# endif
