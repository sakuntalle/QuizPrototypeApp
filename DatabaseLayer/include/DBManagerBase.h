#pragma once
#ifndef DBManagerBase_h__
#define DBManagerBase_h__
#include <unordered_map>
#include "CommonIncludes.h"

class DBManagerBase
{
public:
	DBManagerBase();
	virtual ~DBManagerBase();

	virtual bool CreateDB() = 0;
	virtual bool PopulateDB() = 0;
	virtual bool GetQuestions(QuestionsMap& questions) = 0;
};

#endif // DBManagerBase_h__


