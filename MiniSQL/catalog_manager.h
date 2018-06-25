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
	//��ȡ���µ������ֵ䵽�ڴ�
	void update();
	//���������ֵ��ļ�
	void build_dictionary();
	//���������µ������ֵ��ļ�
	void update_database();
	//�����±� 0��:success 1������
	int create_table(table table);
	//ɾ���� 0���ɹ� 1��:������
	int delete_table(m_string tableName);
	//������ 0���ɹ� 1�������� 2 ���Ѿ�����
	int add_column(m_string tableName, column column);
	//ɾ���� 0���ɹ� 1�������� 2 �в�����
	int delete_column(m_string tableName, m_string columnName);
	//������� 0���ɹ� 1�������� 2�������Ѿ����� 3���в�����
	int add_index(m_string tableName, m_string indexName);
	//ɾ������  0���ɹ� 1�������� 2������������
	int delete_index(m_string tableName, m_string indexName);
	//��ӡ���еı���
	void listTable();
	//�����Ƿ�����������Ψһ
	int is_unique_or_pk(m_string tableName, int colNum);

};