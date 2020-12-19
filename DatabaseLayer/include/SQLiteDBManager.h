#pragma once
#ifndef SQLiteDBManager_h__
#define SQLiteDBManager_h__

#include "DBManagerBase.h"
#include "CommonIncludes.h"
#include "CppSQLite3.h"

class SQLiteDBManager : public DBManagerBase
{
	const char* SQLITE_DB_PATH = "..\\QuizDB\\Quiz.db";
	const char* QUESTIONS_PATH = "..\\QuizDB\\Questions.txt";
	const char QUESTION_DELIM = '|';
	const unsigned short COLUMN_NO = 6;

public:
	SQLiteDBManager();
	virtual ~SQLiteDBManager();

	//DBManagerBase Override
	virtual bool CreateDB();
	virtual bool PopulateDB();
	virtual bool GetQuestions(QuestionsMap& questions);

	// Members
private:
	CppSQLite3DB mSQLiteDB;
};

#endif // SQLiteDBManager_h__


