#pragma once
#include"database.h"
#include"table.h"
#include"column.h"
#include"catalog_manager.h"
class record_manager {
public:
	record_manager();
	data_dictionary dict;
	/*
	select(tableName,columnNames,columnNum,columnName?,operator?,value?)
	数字允许 '=' '>' '<' '!' 'g' 'l' 但value统一使用字符串
	字符串允许 '=' '!'
	后三项可选

	select * from table1 where id=1
	=>
	select("table1",["*"],4,"id",'=',"1")
	/
	select (id,name) from table1 
	=>
	select("table1",["id","name"],2)
	*/
	table* select(m_string tableName, m_string *columns=&m_string("*"),int columnNum=1, m_string column=m_string("") , m_string value=m_string(""), char opt = ' ') ;

	/*
	  insert (id,name,major,gpa) vailues("001","andy","computer","3") into table1
	  =>
	  add("table1",["001","andy","computer","3"]);
	*/
	int add(m_string tableName, m_string *data);
	/*
	  delete from table1 where id=1
	  =>
	  _delete("table1","id",'=',"1");
	*/
	int _delete(m_string tableName, m_string column , m_string value ,char opt = ' ');
	/*
	  update table1 set name = 'andy' where major = 'biology'
	  =>
	  update("table1","name","andy","major",'=',"biology")
	*/
	int update(m_string tableName, m_string column1, m_string value1, m_string column2  ,m_string value2 , char opt = ' ');
};