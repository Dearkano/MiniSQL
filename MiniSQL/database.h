#pragma once
#include "table.h"
class database
{
public:
	database();
	~database();
	table **tables;
	int tableNum;
};

