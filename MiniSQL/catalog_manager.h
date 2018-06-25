#pragma once
#include"database.h"
#include"table.h"
#include"column.h"
#include"m_string.h"
using namespace std;
class data_dictionary {
public:
	data_dictionary();
	~data_dictionary() {}
	database *db;
	//读取最新的数据字典到内存
	void update();
	//生成数据字典文件
	void build_dictionary();
	//重新生成新的数据字典文件
	void update_database();
	//创建新表 0：:success 1：重名
	int create_table(table table);
	//删除表 0：成功 1：:表不存在
	int delete_table(m_string tableName);
	//增加列 0：成功 1：表不存在 2 列已经存在
	int add_column(m_string tableName, column column);
	//删除列 0：成功 1：表不存在 2 列不存在
	int delete_column(m_string tableName, m_string columnName);
	//添加索引 0：成功 1：表不存在 2：索引已经存在 3：列不存在
	int add_index(m_string tableName, m_string indexName);
	//删除索引  0：成功 1：表不存在 2：索引不存在
	int delete_index(m_string tableName, m_string indexName);
	//打印所有的表定义
	void listTable();
	//返回是否是主键或者唯一
	int is_unique_or_pk(m_string tableName, int colNum);

};