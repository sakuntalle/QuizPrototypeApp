#include "stdafx.h"
#include <iostream>
#include "SQLiteDBManager.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/algorithm/string/split.hpp>                                      
#include <boost/algorithm/string.hpp>
#include <vector>

SQLiteDBManager::SQLiteDBManager()
{
	// should be moved on a server parameter 
	CreateDB();	
}


SQLiteDBManager::~SQLiteDBManager()
{
}

bool SQLiteDBManager::CreateDB()
{
	try
	{
		mSQLiteDB.open(SQLITE_DB_PATH);

		mSQLiteDB.execDML("begin transaction;");
		mSQLiteDB.execDML("drop table Questions");
		mSQLiteDB.execDML("create table Questions(ID integer primary key, \
				   							Text char(40), \
											Option0 char(20), \
											Option1 char(20), \
											Option2 char(20), \
											Option3 char(20), \
											AnswerIdx int \
											); ");
		
		mSQLiteDB.execDML("commit transaction;");

		PopulateDB();

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		std::cerr << e.errorCode() << ":" << e.errorMessage() << std::endl;
		ASSERT(false);
		return false;
	}
}

bool SQLiteDBManager::PopulateDB()
{
	try
	{
		std::ifstream qFile(QUESTIONS_PATH);

		mSQLiteDB.execDML("begin transaction;");
		std::string line;

		while (std::getline(qFile, line))
		{
			std::vector<std::string> strQuestionItems;
			boost::algorithm::split(strQuestionItems, line, boost::is_any_of(","));

			ASSERT(strQuestionItems.size() == COLUMN_NO);

			CppSQLite3Buffer bufSQL;
			bufSQL.format("insert into Questions (Text, Option0, Option1, Option2, Option3, AnswerIdx) values (%Q, %Q, %Q, %Q, %Q, %d);", strQuestionItems[0].c_str(), strQuestionItems[1].c_str(), strQuestionItems[2].c_str(), strQuestionItems[3].c_str(), strQuestionItems[4].c_str(), atoi(strQuestionItems[5].c_str()));

			int nRows = mSQLiteDB.execDML(bufSQL);
			ASSERT(nRows == 1);
		}

		mSQLiteDB.execDML("commit transaction;");
		return true;

	}
	catch (CppSQLite3Exception& e)
	{
		std::cerr << e.errorCode() << ":" << e.errorMessage() << std::endl;
		ASSERT(false);
		return false;
	}

	return true;
}

bool SQLiteDBManager::GetQuestions(QuestionsMap& questions)
{
	try
	{
		mSQLiteDB.open(SQLITE_DB_PATH);

		size_t queryResSize = mSQLiteDB.execScalar("select count(*) from Questions;");

		CppSQLite3Query query = mSQLiteDB.execQuery("select * from Questions;");
		
		questions.clear();
		questions.reserve(queryResSize);

		for (size_t idx = 0; idx < queryResSize; idx++)
		{
			int i = query.getIntField(0);
			questions.emplace(query.getIntField(0), QuestionItem(query.fieldValue(1), query.fieldValue(2), query.fieldValue(3), query.fieldValue(4), query.fieldValue(5), query.getIntField(6)));
			query.nextRow();
		}
	
		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		std::cerr << e.errorCode() << ":" << e.errorMessage() << std::endl;
		ASSERT(false);
		return false;
	}
}