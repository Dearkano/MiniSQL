#include "interpreter.h"
#include "table.h"
#include "record_manager.h"
#include "api.h"
#include <fstream>

bool isLegalName(string a)
{
	trim(a);
	if (a == "")
	{
		return false;
	}
	 // string illgal = "{}[]/<>~`\"\\|:\?";
	string illgalForName = "{}[]/<>~`\"\\|:\';+-=!@#$%%^&*\?() "; // 取名不能使用的字符
	for (int i = 0; i < a.length(); i++)
	{
		if (illgalForName.find(a[i]) != -1)
		{
			return false;
		}
	}
	return true;
}

bool IsLegalInt(string a)
{
	trim(a);
	if (a == "")
	{
		return false;
	}
	for (int i = 0; i < a.length(); i++)
	{
		if (a[i] == '-')
		{
			if (i == 0)
				continue;
			else
				return false;
		}
		else if (!(a[i] >= '0' && a[i] <= '9'))
		{
			return false;
		}
	}
	return true;
}

bool IsLegalFloat(string a)
{
	trim(a);
	int count = 0;
	if (a == "")
	{
		return false;
	}
	for (int i = 0; i < a.length(); i++)
	{
		if (a[i] == '-')
		{
			if (i == 0)
				continue;
			else
				return false;
		}
		if (!(a[i] >= '0' && a[i] <= '9') && (a[i] != '.'))
		{
			return false;
		}
		if (a[i] == '.')
		{
			count++;
			if (count > 1)
				return false;
			if (i == 0)
				return false;
		}

	}
	return true;
}



void RemoveEmpty(vector<string> &s)
{
	vector<string>::iterator it;
	for (it = s.begin(); it != s.end();)
	{
		trim(*it);
		if (*it == "")
		{
			it = s.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void RemoveSpace(string &s, const string mark)
{
	size_t nSize = mark.size();
	while (1)
	{
		size_t pos = s.find(mark);    //  尤其是这里  
		if (pos == string::npos)
		{
			return;
		}

		s.erase(pos, nSize);
	}
}

void RemoveTabSpace(string &s)
{
	size_t nSize = 1;
	while (1)
	{
		size_t pos = s.find(" ");    //  尤其是这里  
		if (pos == string::npos)
		{
			return;
		}

		s.erase(pos, nSize);
	}
	while (1)
	{
		size_t pos = s.find("\t");    //  尤其是这里  
		if (pos == string::npos)
		{
			return;
		}
		s.erase(pos, nSize);
	}
}
bool Space(string str)
{
	int find1 = str.find(" ");
	int find2 = str.find("\t");
	if (find1 == string::npos && find2 == string::npos)
	{
		return false;
	}
	return true;
}
string Input()
{
	string sql = "";
	string temp;
	bool finish = false;
	cout << "sql> ";
	while (!finish)
	{
		getline(cin, temp);
		trim(temp);
		if (temp.length() == 0)
			continue;
		if (temp[temp.length() - 1] == ';')
		{
			temp[temp.length() - 1] = ' ';
			temp += ";";
			sql += temp;
			finish = true;
		}
		else
		{
			sql += (temp + " ");
			cout << "---> ";
		}
	}
	return sql;
}

string ExecFile(string path)
{
	int find = path.find("execfile");
	find += 8;
	path = path.substr(find);
	cout << path << endl;
	if (path.empty())
	{
		cerr << "语法错误：空路径" << endl;
		return "99";
	}
	string fileName = path;
	trim(fileName);
	while (fileName[fileName.length() - 1] == ';')
	{
		fileName.erase(fileName.end() - 1);
	}
	trim(fileName);
	ifstream inf(".\\sql\\" + fileName);
	// 表征文件行数
	int line = 1;
	if (!inf.is_open())
	{
		cout << "打开文件时发生错误!" << endl;
		return "99";
	}
	string sql = "";
	string temp;
	while (!inf.eof())
	{
		getline(inf, temp);
		trim(temp);
		if (temp.length() == 0)
			continue;
		if (temp[temp.length() - 1] == ';')
		{
			temp[temp.length() - 1] = ' ';
			temp += ";";
			sql += temp;
			string result = Interpreter(sql);
			if (result == "80")
			{
				// 清空当前sql语句
				sql = "";
			}
			else if (result == "99")
			{
				cerr << "脚本执行失败!问题语句结束于第" << line << "行" << endl;
				return "99";
			}
		}
		else
			sql += (temp + " ");
		line++;
	}
	cout << "脚本执行成功" << endl;
	return "80";
}
// 分类阐释器
string Interpreter(string statement)
{
	trim(statement);
	string sql;
	string temp;
	int start = 0, end;
	sql = statement;
	if (sql == "")
	{
		return "99";
	}
	end = sql.find(' ', start);
	temp = sql.substr(start, end - start);
	start = end + 1;
	// 如果没有输入，打印出错信息
	if (temp.empty())
	{
		cout << "语法错误：语句为空" << endl;
		sql = "99";
	}
	// 若为正常语句
	else if (temp == "create")
		sql = Create(sql);
	else if (temp == "drop")
		sql = Drop(sql);
	else if (temp == "select")
		sql = Select(sql);
	else if (temp == "insert")
		sql = Insert(sql);
	else if (temp == "delete")
		sql = Delete(sql);
	else if (temp == "execfile")
		sql = ExecFile(sql);
	else if (temp == "update")
		sql = UpdateTable(sql);
	// 如果是非法语句
	else
	{
		cout << "语法错误: ‘" << temp << "’---无效的的关键字或方法" << endl;
		sql = "99";
		return sql;
	}
	// 返回语句的内部形式
	return sql;
}

string  Drop(string sql)
{
	string nowSql = sql.substr(5);
	int position = nowSql.find(' ', 0);
	if (position == -1)
	{
		cout << "语法错误：语句为空" << endl;
		sql = "99";
		return sql;
	}
	string keyWord = nowSql.substr(0, position);
	trim(keyWord);
	if (keyWord == "table")
	{
		sql = DropTable(sql);
	}
	else if (keyWord == "index")
	{
		sql = DropIndex(sql);
	}
	else
	{
		cout << "语法错误: ‘" << keyWord << "’---无效的关键字或方法";
		sql = "99";
	}
	return sql;
}



// 确定是创建什么
string Create(string sql)
{
	string nowSql = sql.substr(7);
	int position = nowSql.find(' ', 0);
	if (position == -1)
	{
		cout << "语法错误：语句为空" << endl;
		sql = "99";
		return sql;
	}
	string keyWord = nowSql.substr(0, position);
	trim(keyWord);
	if (keyWord == "table")
	{
		sql = CreateTable(sql);
	}
	else if (keyWord == "index")
	{
		sql = CreateIndex(sql);
	}
	else
	{
		cout << "语法错误: ‘" << keyWord << "’---无效的关键字或方法" << endl;
		sql = "99";
		return sql;
	}
	return sql;
}

// 测试建立表
// CreateTable: 建表的核心函数

string CreateTable(string sql)
{
	InterTable newTable;
	vector<string> innerItem;
	vector<string> orderAttr;
	map<string, attrInfo> attrList; // 属性名--存储属性信息的结构体
	string nowSql = sql.substr(12);
	int position = nowSql.find('(', 0);
	if (position == -1)
	{
		cout << "语法错误：缺少‘(’" << endl;
		sql = "99"; // 抛出错误代码，返回
		return sql;
	}
	string tableName = nowSql.substr(0, position); // 获得表名
	trim(tableName);
	if (isLegalName(tableName) == false)
	{
		cout << "语法错误：表名含有非法字符" << endl;
		sql = "99"; // 抛出错误代码，返回
		return sql;
	}
	// 处理没有右括号的情况
	char last = nowSql[nowSql.length() - 1];
	while (last == ';')
	{
		nowSql.erase(nowSql.end() - 1);
		trim(nowSql);
		last = nowSql[nowSql.length() - 1];
	}
	if (last != ')')
	{
		cout << "语法错误：缺少‘)’或)外有异常字符" << endl;
		sql = "99";
		return sql;
	}
	// 处理没有右括号的情况结束
	// “包含括号”获取括号中的内容
	int position1 = nowSql.find("(");
	int position2 = nowSql.length() - 1; // 找到最后一个
	while (nowSql[position2] != ')')
		position2--;

	nowSql = nowSql.substr(position1, position2 - position1 + 1);
	// 将括号去除
	nowSql.erase(nowSql.begin());
	nowSql.erase(nowSql.end() - 1);
	// 已经提取括号内的内容
	// 将括号内的内容按照逗号分割
	// 使用boost库进行分割
	boost::split(innerItem, nowSql, is_any_of(","), token_compress_on);
	// 对逗号分隔的元素进行一次处理
	// 元素应当形如 “attrname varchar(15)”这个样子
	RemoveEmpty(innerItem);
	if (innerItem.empty())
	{
		cout << "语法错误：表定义为空" << endl;
		sql = "99"; // 返回错误代码；
		return sql;
	}
	bool Primary = false; // 表征表的主键被定义了没有
	string primaryName;
	for (int i = 0; i < innerItem.size(); i++)
	{
		trim(innerItem[i]);
		if (innerItem[i] == "")
		{
			cout << "语法错误：表定义存在格式错误" << endl;
			sql = "99"; // 返回错误代码；
			return sql;
		}
		// 处理每一条属性语句
		attrInfo tempInfo = ProcessInfo(innerItem[i]);
		if (tempInfo.resType == ERR)
		{
			cout << "语法错误：属性" << tempInfo.attrName << "的定义存在格式错误" << endl;
			sql = "99"; // 返回错误代码；
			return sql;
		}
		// 如果成功
		//两种情况，属性定义和约束添加
		// 约束添加的话，ProcessInfo函数只返回表名、和新的约束，以及是否unique
		// 为了表征两种语句的不同，如果是单纯的约束添加语句，length被会被置为-2
		// 如果是处理unique的语句，length会被置为-3
		if (tempInfo.length == -2) // 如果是约束添加语句
		{
			string getName = tempInfo.attrName;
			if (isLegalName(getName) == false)
			{
				cout << "语法错误：属性名中有非法字符" << endl;
				sql = "99"; // 抛出错误代码，返回
				return sql;
			}
			RESTRICT newRestrict = tempInfo.resType;
			switch (newRestrict)
			{
			case PRKEY:
			{
				if (Primary == true) // 如果主键已经被定义过了
				{
					cout << "约束声明错误：主键的重定义" << endl;
					sql = "99";
					return sql;
				}
				else if (Primary == false)
				{
					map<string, attrInfo>::iterator it;
					if ((it = attrList.find(getName)) != attrList.end())
					{
						// 修改约束
						it->second.resType = PRKEY;
						Primary = true;
						primaryName = getName;

					}
					else
					{
						cout << "主键约束声明错误：列名不存在" << endl;
						sql = "99";
						return sql;
					}
				}
				break;
			}
			case FRKEY:
			{
				map<string, attrInfo>::iterator it;
				if ((it = attrList.find(getName)) != attrList.end())
				{
					//修改约束
					it->second.resType = FRKEY;
					//修改引用的表
					it->second.reference = tempInfo.reference;
				}
				// 如果列名不存在
				else
				{
					cout << "外键约束声明错误：列名不存在" << endl;
					sql = "99";
					return sql;
				}
				break;
			}
			default:
			{
				cout << "约束声明错误：不存在的声明" << endl;
				sql = "99";
				return sql;
			}
			}
		}
		// 如果是unique添加语句
		else if (tempInfo.length == -3)
		{
			string getName = tempInfo.attrName;
			bool newUnique = tempInfo.unique;
			map<string, attrInfo>::iterator it;
			if ((it = attrList.find(getName)) != attrList.end())
			{
				// 修改unique
				it->second.unique = newUnique;
			}
			else
			{
				cout << "unique约束声明错误：列名不存在" << endl;
				sql = "99";
				return sql;
			}
		}
		// 如果没有其他的问题
		else
		{
			// 将属性插入表中
			try
			{
				map<string, attrInfo>::iterator it;
				if ((it = attrList.find(tempInfo.attrName)) != attrList.end())
					throw "建立表格时发生错误：属性名冲突";
				else
				{
					attrList.insert(pair<string, attrInfo>(tempInfo.attrName, tempInfo));
					orderAttr.push_back(tempInfo.attrName);
				}
			}
			catch (const char* Msg)
			{
				cerr << Msg << endl;
				sql = "99";
				return sql;
			}
		}
	}
	// 属性列表已经建好
	newTable.attrList = attrList;
	newTable.tableName = tableName;
	newTable.prKey = primaryName;
	newTable.orderAttr = orderAttr;
	/**************************************/
	/*construct the table class and insert*/
	/**************************************/
	cout << newTable.tableName << endl;
	map<string, attrInfo>::iterator iter;
	iter = newTable.attrList.begin();
	/*while (iter != newTable.attrList.end()) {
		cout << iter->first << " : " << iter->second.attrType << " " << iter->second.length<< " " << \
			iter->second.resType << " " << iter->second.unique <<endl;
		iter++;
	}*/
	int result = create_table_api(newTable);
	if (result == 1)
	{
		cerr << "数据库错误：表名冲突！" << endl;
		sql = "99";
		return sql;
	}
	else if (result == 0)
	{
		sql = "80";
		return sql;
	}
	return sql;
}
// 传入的参数应当是形如 cool char(20) not unique;

attrInfo ProcessInfo(string info)
{
	// 正常处理：如果该行是属性定义
	attrInfo newInfo;
	newInfo.unique = false;
	newInfo.resType = DEFAULT;
	newInfo.length = -1;
	newInfo.attrType = "unknown";
	RESTRICT res;
	trim(info);
	vector<string> strGroup;
	split(strGroup, info, is_any_of("\n \t()"), token_compress_on);
	RemoveEmpty(strGroup);
	if (strGroup.empty() || strGroup.size() <= 1)
	{
		cerr << "属性错误：无法识别的属性名" << endl;
		// 报错
		newInfo.resType = ERR;
		return newInfo;
	}
	int ptr = 0;
	string attrName = strGroup[ptr++];
	trim(attrName);
	// 如果是添加约束的
	if (attrName == "primary" || attrName == "unique")
	{
		// 就要读下面的括号了
		if (attrName == "primary")
		{
			trim(strGroup[ptr]);
			if (strGroup[ptr] != "key")
			{
				cerr << "约束错误：未定义的约束类型" << strGroup[ptr] << endl;
				// 报错
				newInfo.resType = ERR;
				return newInfo;
			}
			else
			{
				try
				{
					string attrName = strGroup.at(++ptr);
					trim(attrName);
					newInfo.attrName = attrName;
					newInfo.length = -2;
					newInfo.resType = PRKEY;

				}
				catch (std::out_of_range)
				{
					cerr << "约束错误(primary key)：未定义的属性名" << endl;
					// 报错
					newInfo.resType = ERR;
				}
				return newInfo;
			}
		}
		else
		{
			try
			{
				string attrName = strGroup.at(++ptr);
				trim(attrName);
				newInfo.attrName = attrName;
				newInfo.length = -3;
				newInfo.unique = true;

			}
			catch (std::out_of_range)
			{
				cerr << "约束错误(unique)：未定义的属性名" << endl;
				// 报错
				newInfo.resType = ERR;
			}
			return newInfo;
		}
	}
	else
	{
		string type = strGroup[ptr++];
		int length = -1;
		trim(type);
		if (type == "char")
		{
			if (ptr >= strGroup.size())
			{
				cerr << "类型错误：未指明char类型的长度" << endl;
				// 报错
				newInfo.resType = ERR;
				return newInfo;
			}
			//获取长度
			int tempLen = atoi(strGroup[ptr].c_str());
			if (tempLen <= 0)
			{
				cerr << "类型错误：无效的char类型长度值" << endl;
				// 报错
				newInfo.resType = ERR;
				return newInfo;
			}
			length = tempLen;
			ptr++;
		}
		else if (type == "int")
		{
		}
		else if (type == "float")
		{
		}
		else
		{
			cerr << "类型错误：未定义的数据类型" << endl;
			// 报错
			newInfo.resType = ERR;
			return newInfo;
		}
		// 处理约束
		if (ptr >= strGroup.size())
		{
			res = DEFAULT;
		}
		else
		{
			string resInfo = strGroup[ptr++];
			if (resInfo == "primary")
			{
				if (ptr >= strGroup.size() || strGroup[ptr] != "key")
				{
					cerr << "约束错误：未定义的约束类型" << endl;
					// 报错
					newInfo.resType = ERR;
					return newInfo;
				}
				newInfo.resType = PRKEY;
			}
			else if (resInfo == "unique")
			{
				newInfo.unique = true;
			}
			else
			{
				cerr << "约束错误：未定义的约束类型" << endl;
				// 报错
				newInfo.resType = ERR;
				return newInfo;
			}
		}
		newInfo.attrName = attrName;
		newInfo.attrType = type;
		newInfo.length = length;
		newInfo.reference = "";
		return newInfo;
	}
}




string DropTable(string sql)
{
	string posSql = sql;
	int find = sql.find("table");
	if (find != string::npos)
	{
		// 将table 去除
		sql = sql.substr(find + 4);
		if (sql.length() == 1) return "80";
		sql = sql.substr(1);
		if (sql[0] != ' ' && sql[0] != '\t')
		{
			cerr << "语法错误：drop table:无法确定的表名" << endl;
			return "99";
		}
		trim(sql);
		find = sql.find(";");
		if (find == string::npos)
		{
			find = sql.length();
		}
		// 获得表名
		string tableName = sql.substr(0, find);
		trim(tableName);
		if (isLegalName(tableName) == false)
		{
			cout << "语法错误：表名含有空格或非法字符" << endl;
			sql = "99"; // 抛出错误代码，返回
			return sql;
		}
		if (tableName.length() == 0)
		{
			cerr << "语法错误：drop table:无法确定的表名" << endl;
			return "99";
		}
		trim(tableName);
		int result = drop_table_api(tableName);
		if (result == 1)
		{
			cerr << "drop table错误：查无此表" << endl;
			return "99";
		}
		cout << "删除表" << tableName << "成功" << endl;
		sql = "80";
	}
	else
	{
		cerr << "语法错误：drop table:无法确定的表名" << endl;
		return "99";
	}
	return sql;
}


string DropIndex(string sql)
{
	string posSql = sql;
	int find = sql.find("index");
	if (find != string::npos)
	{
		// 将table 去除
		sql = sql.substr(find + 4);
		if (sql.length() == 1) return "80";
		sql = sql.substr(1);
		if (sql[0] != ' ' && sql[0] != '\t')
		{
			cerr << "语法错误：drop index:无法确定的索引名" << endl;
			return "99";
		}
		trim(sql);
		find = sql.find(";");
		if (find == string::npos)
		{
			find = sql.length();
		}
		// 获得表名
		string tableName = sql.substr(0, find);
		trim(tableName);
		if (isLegalName(tableName) == false)
		{
			cout << "语法错误：索引名含有空格或非法字符" << endl;
			sql = "99"; // 抛出错误代码，返回
			return sql;
		}
		if (tableName.length() == 0)
		{
			cerr << "语法错误：drop index:无法确定的索引名" << endl;
			return "99";
		}
		trim(tableName);
		string result = drop_index_api(tableName);
		return result;
		cout << "delete from " << tableName << " success" << endl;
	}
	else
	{
		cerr << "语法错误：drop index:无法确定的索引名" << endl;
		return "99";
	}
	return sql;
}

string Delete(string sql)
{
	string preSql = sql;
	int find = sql.find("from");
	if (find == string::npos)
	{
		cerr << "语法错误：缺少“from”" << endl;
		return "99";
	}
	sql = sql.substr(find + 4);
	find = sql.find("where");
	vector<condition> cod;
	string tableName;
	//有where的情况
	if (find != string::npos)
	{
		tableName = sql.substr(0, find);
		RemoveSpace(tableName, ";");
		trim(tableName);
		if (tableName.length() == 0)
		{
			cerr << "语法错误：未指定需要查找的表" << endl;
			return "99";
		}
		if (isLegalName(tableName) == false)
		{
			cout << "语法错误：表名含有空格或非法字符" << endl;
			return "99"; // 抛出错误代码，返回
		}
		// 获得表名并继续截取
		sql = sql.substr(find + 5);
		while (sql[sql.length() - 1] == ';')
			sql.pop_back();
		trim(sql);
		try
		{
			cod = WhereSplit(sql);
		}
		catch (...)
		{
			return "99";
		}
		for (int i = 0; i < cod.size(); i++)
		{
			cout << "condition: " << cod[i].attr << " " << cod[i].value << endl;
		}
	}
	else
	{
		tableName = sql.substr(0, sql.length());
		RemoveSpace(tableName, ";");
		trim(tableName);
		if (tableName.length() == 0)
		{
			cerr << "语法错误：未指定需要查找的表" << endl;
			return "99";
		}
		if (isLegalName(tableName) == false)
		{
			cout << "语法错误：表名含有空格或非法字符" << endl;
			return "99"; // 抛出错误代码，返回
		}
		cod.clear();
		cout << "无条件" << endl;
	}
	if (cod.size() <= 1)
	{
		int result = multi_delete(tableName, cod);
		if (result == -1)
		{
			cerr << "delete错误：查无此表" << endl;
			return "99";
		}
		else if (result == -2)
		{
			cerr << "delete错误:where条件提供的属性不存在" << endl;
			return "99";
		}
		else if (result < 0)
		{
			// 其他错误
			return "99";
		}
		cout << result << "行受影响" << endl;
		return "80";
	}
	else
	{
		int result = multi_delete(tableName, cod);
		if (result == -1)
		{
			cerr << "delete错误：查无此表" << endl;
			return "99";
		}
		else if (result == -2)
		{
			cerr << "delete错误:where条件提供的属性不存在" << endl;
			return "99";
		}
		else if (result < 0)
		{
			// 其他错误
			return "99";
		}
		cout << result << "行受影响" << endl;
		return "80";
	}
}

string Insert(string sql)
{
	string preSql = sql;
	int find = sql.find("into");
	if (find == string::npos)
	{
		cerr << "语法错误：缺少“into”" << endl;
		return "99";
	}
	sql = sql.substr(find + 4);
	if (sql[0] != ' ' && sql[0] != '\t')
	{
		cerr << "语法错误：无法辨识的目标名" << endl;
		return "99";
	}
	trim(sql);
	find = sql.find("values");
	if (find == string::npos)
	{
		cerr << "语法错误：缺少“values”" << endl;
		return "99";
	}
	string tableName = sql.substr(0, find);
	trim(tableName);
	if (tableName.length() == 0)
	{
		cerr << "语法错误：无效的表名" << endl;
		return "99";
	}
	sql = sql.substr(find + 5);
	// 处理括号闭合的情况
	int findLeft = sql.find("(");
	int findRight = sql.length() - 1;
	while (findRight != -1 && sql[findRight] != ')')
		--findRight;
	if (findLeft == string::npos || findRight == -1 || findLeft >= findRight)
	{
		cerr << "语法错误：括号不闭合" << endl;
		return "99";
	}
	//截取有括号的部分
	string value = sql.substr(findLeft, findRight - findLeft + 1);
	// 去除括号
	trim(value);
	value.erase(value.begin());
	value.erase(value.end() - 1);
	// 有逗号的数据进入valueList
	vector<string> valueList;
	try
	{
		valueList = ValueSplit(value);
	}
	catch(...)
	{
		return "99";
	}
	try
	{
		int result = insert_into_api(valueList, tableName);
		if (result == 0)
		{
			return "80";
		}
		else
		{
			return "99";
		}
	}
	catch(...)
	{
		return "99";
	}
}


// 进入信息处理模块的应当是由逗号分隔的数据




string Select(string sql)
{
	if (sql.empty()) return "80";
	string preSql = sql;
	// 去除头部的select
	string mask1 = "select";
	string mask2 = "";
	vector<string> vec2;
	vec2.push_back(mask1);
	vec2.push_back(mask2);
	replace_first(sql, mask1, mask2);
	// 如果select和表名之间没有空格
	if (sql[0] != ' ' && sql[0] != '\t')
	{
		cerr << "语法错误：无法识别的sql命令" << endl;
		return "99";
	}
	// 如果没有问题，获取需要查找的属性名
	trim(sql);
	// 方法是找到第一个from，截取前面的字符串
	int find = sql.find("from");
	if (find != string::npos)
	{
		// 读取列名
		string targetList = sql.substr(0, find);
		sql = sql.substr(find); // sql 读取之后的串
								//将串中所有的空格全部去除
		RemoveSpace(targetList);
		if (targetList.length() == 0)
		{
			cerr << "语法错误：未指定需要查找的属性" << endl;
			return "99";
		}
		// 处理字符串
		vector<string> attr;
		bool all = false;
		split(attr, targetList, is_any_of(","), token_compress_on);
		vector<string> newAttr;
		// 如果是所有列。"*"
		for (int i = 0; i < attr.size(); i++)
		{
			// 如果不为空
			if (!attr[i].empty())
			{
				trim(attr[i]);
				if (attr[i] == "*")
				{
					all = true;
					newAttr.clear();
					break;
				}
				else
				{
					all = false;
					newAttr.push_back(attr[i]);
				}
			}
		}
		// 然后开始处理条件
		trim(sql);
		// 去除 from
		sql = sql.substr(4);
		find = sql.find("where");
		if (find != string::npos)
		{
			string tableName = sql.substr(0, find);
			trim(tableName);
			if (tableName.length() == 0)
			{
				cerr << "语法错误：未指定需要查找的表" << endl;
				return "99";
			}
			
			sql = sql.substr(find + 5);
			trim(sql);
			while (sql[sql.length() - 1] == ';')
				sql.erase(sql.end() - 1);
			trim(sql);
			vector<condition> cod = WhereSplit(sql);
			// 获得条件列表
			for (int i = 0; i < cod.size(); i++)
			{
				cout << "condition: " << cod[i].attr << " " << cod[i].value << endl;
			}
			/*----------------------------------------*/
			string a = new_select_api(tableName, newAttr, cod); // 如果查全部，newAttr是空的
			return a;
		}
		else
		{
			string tableName = sql.substr(0, sql.length());
			trim(tableName);
			RemoveSpace(tableName, ";");
			trim(tableName);
			if (tableName.length() == 0)
			{
				cerr << "语法错误：未指定需要查找的表" << endl;
				return "99";
			}
			vector<condition> cod;
			cod.clear();
			string a = new_select_api(tableName, newAttr, cod); // 如果查全部，newAttr是空的
			return a;
		}
	}
	else
	{
		cerr << "语法错误：缺少from" << endl;
		return "99";
	}
	return sql;
}


vector<string> ValueSplit(string input)
{
	vector<string> result;
	string stack = "";
	string number = "";
	bool close = true;
	bool submit = true;
	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == '\'' && close == true)
		{
			stack.push_back(input[i]);
			close = false; // 可以向栈内压入字符串
		}
		else if (input[i] == '\'' && close == false)
		{
			stack.push_back(input[i]);
			close = true;
			int k = i + 1;
			while (k < input.length() - 1 && (!input[k] == ','))
			{
				if (input[k] != ' ' && input[k] != '\t')
				{
					throw "value错误："+stack+"后有未定义的标识符";
				}
			}
			i = k - 1;
			submit = false;
		}
		else
		{
			if (close == false)
			{
				stack.push_back(input[i]);
			}
			else if (close == true)
			{
				if (input[i] == ' ' || input[i] == '\t')
				{
					continue;
				}
				if (input[i] == ',' && submit == false)
				{
					result.push_back(stack);
					stack = "";
					submit = true;
				}
				else
				{
					int nextCut = input.find(",", i);
					if (nextCut == string::npos)
					{
						nextCut = input.length();
					}
					
					number = input.substr(i, nextCut - i);
					trim(number);
					i = nextCut;
					const char* temp = number.c_str();
					int t = number.find(".");
					if (t != string::npos)
					{
						int d = atof(temp);
						if (d == 0)
						{
							string error = "value错误：错误的浮点数字" + number;
							cerr << error << endl;
							throw error.c_str();
						}
						else
						{
							result.push_back(number);
							number = "";
						}
					}
					else
					{
						int d = atoi(temp);
						if (d == 0 && number != "0")
						{
							string error = "value错误：错误的整型数字" + number;
							cerr << error << endl;
							throw error.c_str();
						}
						else
						{
							result.push_back(number);
							number = "";
						}
					}
				}
			}
		}
	}
	if (close == true && submit == false)
	{
		result.push_back(stack);
		stack = "";
		submit = true;
	}
	else if (close == false)
	{
		string error = "value错误：未闭合的单引号";
		cerr << error << endl;
		throw error.c_str();
	}
	return result;
}


vector<condition> WhereSplit(string input)
{
	string save = input;
	// step 1
	vector<string> codList;
	string stack1 = "";
	string stack2 = "";
	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == '\t') input[i] = ' ';
		if (input[i] == '\'')
		{
			int nextCut = input.find("\'",i + 1); // 找到闭合的下一个
			if (nextCut == string::npos)
			{
				string error = "where错误：未闭合的\'";
				cerr << error << endl;
				throw error.c_str();
			}
			stack1 = input.substr(i, nextCut - i + 1);
			i = nextCut; //i 移动到下一位置
			input[i] = ' ';
			/*list.push_back(stack2 + " " + stack1);
			stack2 = "";
			stack1 = "";*/
		}
		else
		{
			stack2.push_back(input[i]);
			int find = stack2.find(" and ");
			if (find != string::npos)
			{
				stack2 = stack2.substr(0, find);
				if (stack1 != "")
				{
					codList.push_back(stack2 + " " + stack1);
					stack2 = "";
					stack1 = "";
				}
				else
				{
					codList.push_back(stack2);
					stack2 = "";
				}
			}
		}
	}
	if (stack1 != "")
	{
		codList.push_back(stack2 + " " + stack1);
		stack2 = "";
		stack1 = "";
	}
	else
	{
		codList.push_back(stack2);
		stack2 = "";
	}
	vector<condition> temp;
	condition cod;
	for (int i = 0; i < codList.size(); i++)
	{
		trim(codList[i]);
		int find1 = codList[i].find("<");
		int find2 = codList[i].find(">");
		int find3 = codList[i].find("=");
		int find4 = codList[i].find(">=");
		int find5 = codList[i].find("<=");
		int find6 = codList[i].find("<>");
		// 单纯小于
		if (find1 != string::npos && find5 == string::npos && find6 == string::npos)
		{
			string attrName = codList[i].substr(0, find1);
			string condition = codList[i].substr(find1);
			condition.erase(condition.begin()); //去除第一个字符
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = SMALL;
			cod.value = condition;
			temp.push_back(cod);
			// 
		}
		// 单纯大于
		else if (find2 != string::npos && find4 == string::npos && find6 == string::npos)
		{
			string attrName = codList[i].substr(0, find2);
			string condition = codList[i].substr(find2);
			condition.erase(condition.begin()); //去除第一个字符
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = BIG;
			cod.value = condition;
			temp.push_back(cod);
		}
		// 单纯等于
		else if (find3 != string::npos && find4 == string::npos && find5 == string::npos)
		{
			string attrName = codList[i].substr(0, find3);
			string condition = codList[i].substr(find3);
			condition.erase(condition.begin()); //去除第一个字符
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = EQUAL;
			cod.value = condition;
			temp.push_back(cod);
		}
		// 不大于
		else if (find5 != string::npos)
		{
			string attrName = codList[i].substr(0, find5);
			string condition = codList[i].substr(find5 + 1);
			condition.erase(condition.begin()); //去除第一个字符
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = NOTBIG;
			cod.value = condition;
			temp.push_back(cod);
		}
		// 不小于
		else if (find4 != string::npos)
		{
			string attrName = codList[i].substr(0, find4);
			string condition = codList[i].substr(find4 + 1);
			condition.erase(condition.begin()); //去除第一个字符
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = NOTSMALL;
			cod.value = condition;
			temp.push_back(cod);
		}
		// 不等于
		else if (find6 != string::npos)
		{
			string attrName = codList[i].substr(0, find6);
			string condition = codList[i].substr(find6 + 1);
			condition.erase(condition.begin()); //去除第一个字符
			trim(attrName);
			trim(condition);
			cod.attr = attrName;
			cod.cond = NOTEQUAL;
			cod.value = condition;
			temp.push_back(cod);
		}
		else
		{
			string error = "where错误：错误的条件" + codList[i];
			cerr << error << endl;
			throw error.c_str();
		}
	}
	return temp;
}

string CreateIndex(string sql)
{
	// 进入函数的语句应当是create index
	string preSql = sql;
	int find = sql.find("on");
	if (find == string::npos)
	{
		cerr << "语法错误：缺少“on”" << endl;
		return "99";
	}
	int findPre = sql.find("index");
	findPre += 5;
	string indexName = sql.substr(findPre, find - findPre);
	trim(indexName);
	if (indexName.length() == 0)
	{
		cerr << "语法错误:未指明索引名" << endl;
		return "99";
	}
	if (!isLegalName(indexName))
	{
		cerr << "语法错误：非法的索引名" << endl;
		return "99";
	}
	sql = sql.substr(find + 2);
	trim(sql);
	// 将 on 去掉；
	find = sql.find(";");
	string tableAttr = sql.substr(0, find);
	trim(tableAttr);
	if (tableAttr.length() == 0)
	{
		cerr << "语法错误:未指明索引目标" << endl;
		return "99";
	}
	// 处理表名和列
	int find1 = tableAttr.find("(");
	int find2 = tableAttr.find(")");
	if (find1 == string::npos || find2 == string::npos || find1 > find2)
	{
		cerr << "语法错误：括号不闭合" << endl;
		return "99";
	}
	string table = tableAttr.substr(0, find1);
	string attr = tableAttr.substr(find1, find2 - find1);
	attr.erase(attr.begin());
	trim(table);
	trim(attr);
	if (!(isLegalName(table)) || (!isLegalName(attr)))
	{
		cerr << "语法错误：非法的表名或列名" << endl;
		return "99";
	}
	cout << indexName << " " << table << " " << attr << endl;
	string result = create_index_api(table, attr, indexName);
	sql = result;
	return sql;
}


void clear()
{

}

string UpdateTable(string sql)
{
	trim(sql);								//	将字符串两端空格去除
	string formSql = sql;					//	储存原字符串
	sql = sql.substr(6);					//	去除update，其为6位
	if (sql[0] != ' ' || sql[0] != '\t')	//	如果update的下字符非空
	{
		cerr << "Syntax Error: Invalid table name in update expression" << endl;
		return "99";
	}
	// 此时应当暴露table
	trim(sql);
	int find = sql.find("set");
	if (find == string::npos)
	{
		cerr << "Syntax Error: keyword \'set'\ is missed" << endl;
		return "99";
	}
	string tableName = sql.substr(0, find);
	trim(tableName);
	if (!isLegalName(tableName))
	{
		cerr << "Syntax Error: illegal table name" << endl;
		return "99";
	}
	sql = sql.substr(find + 3);
	if (sql[0] != ' ' || sql[0] != '\t')	//	如果update的下字符非空
	{
		cerr << "Syntax Error: Invalid attribute name in update expression" << endl;
		return "99";
	}
	trim(sql);
	string setStr;
	// 下面提取set条件，为此我们需要先找到where
	int find2 = sql.find("where");
	// 没有
	if (find2 == string::npos)
	{
		int find3 = sql.find(";");
		setStr = sql.substr(0, find3);
		trim(setStr);
	}
	else
	{
		setStr = sql.substr(0, find2);
		trim(setStr);
	}
	// 处理setstr
	vector<condition> condList;
	condList.clear(); // 一开始清空
	if (find2 != string::npos)
	{
		int find4 = find2 + 5;
		if (find4 >= sql.length() || (sql[find4] != ' ' && sql[find4] != '\t'))
		{
			cerr << "Syntax Error: Invalid expression after \'where\'" << endl;
			return "99";
		}
		int find5 = sql.find(";");
		if (find5 < find4)
		{
			cerr << "Syntax Error: Unexpected \';\'" << endl;
			return "99";
		}
		string whereStr = sql.substr(find4, find5 - find4);
		trim(whereStr);
		condList = WhereSplit(whereStr); // 获取条件表
	}
	vector<string> container;
	boost::split(container, setStr, is_any_of("="), token_compress_off);
	if (container.size() != 2)
	{
		cerr << "Syntax Error: Unexpected \'=\'" << endl;
		return "99";
	}
	trim(container[0]);
	trim(container[1]);
	if (!isLegalName(container[0]))
	{
		cerr << "Syntax Error: illgal attribute name" << endl;
		return "99";
	}
	//  开始调用接口


	return "80";
}

