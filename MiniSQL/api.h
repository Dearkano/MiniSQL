#pragma once
#ifndef _API_H_
#include "interpreter.h"
#include "catalog_manager.h"
#include "record_manager.h" 

int create_table_api(InterTable newTable);
int insert_into_api(vector<string> valueList, string tableName);
int delete_from_api(string tableName, vector<condition> option);
//table* select_from_api(string tableName, vector<string> attrList, condition opt);
//table* select_from_api(string tableName, vector<string> attrList);
// 以上两个函数接口不予暴露
string select_api(string tableName, vector<string> attrList, vector<condition> options);
int drop_table_api(string tableName);
string create_index_api(string tableName, string attr, string indexName);
string drop_index_api(string indexName);

#endif // !_API_H_
