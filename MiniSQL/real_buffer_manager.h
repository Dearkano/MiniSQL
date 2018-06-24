#pragma once
#include"m_string.h"
class real_buffer_manager {
public:
	int create_dbFile(m_string tableName);
	int delete_dbFile(m_string tableName);
	m_string** read_table(m_string tableName, int rowNum,int colNum);
	int update_table(m_string tableName, m_string **data, int rowNum, int colNum);
};
