#include "stdafx.h"
#include "GamesManager.h"
#include "CommHandlerHelpers.h"
#include "CommMessageFactory.h"
#include "SQLiteDBManager.h"
#include "CommServer.h"

GamesManager::GamesManager(CommServer* pCommServer)
	: m_pServerQuiz(pCommServer)
{
	//reserve initial space for the hash tables
	m_mapPlayerIDToGameTuple.reserve(INITIAL_ALLOCATED_PLAYERS_NUMBER);
	CommObserverManager::GetInstance()->RegisterCommServerListener(this);

	// get the questions from DB
	SQLiteDBManager dbSqlite;
	dbSqlite.GetQuestions(m_mapQuestionIDToQuestionItem);

	srand((size_t)time(NULL));
}

GamesManager::~GamesManager()
{
	CommObserverManager::GetInstance()->UnregisterCommServerListener(this);
}

bool GamesManager::IsAnotherPlayerAvailable(const unsigned short& currPlayerId, unsigned short& foundPlayerId)
{
	foundPlayerId = INVALID_PLAYER_ID;

	std::lock_guard<std::recursive_mutex> lock(m_MapsMutex);
	for (auto it = m_mapPlayerIDToGameTuple.begin(); it != m_mapPlayerIDToGameTuple.end(); it++)
	{
		if (it->first != currPlayerId && (std::get<1>(it->second).get() == nullptr))
		{
			// found a player that's not in a game
			foundPlayerId = it->first;
			return true;
		}
	}

	return false;
}

void GamesManager::CheckPlayersWithoutGame(const unsigned short& currPlayerId)
{
	// check if there is one logged in client that does not have a Game running
	unsigned short nPlayerWithoutGame;
	if (IsAnotherPlayerAvailable(currPlayerId, nPlayerWithoutGame))
	{
		StartGame(currPlayerId, nPlayerWithoutGame);
	}
}

void GamesManager::StartGame(const unsigned short& playerFirst, const unsigned short& playerSecond)
{
	// send the SendInit command to both players 

	std::lock_guard<std::recursive_mutex> lock(m_MapsMutex);
	std::shared_ptr<Game> spNewGame(new Game(playerFirst, playerSecond));
	
	SetNewQuestion(spNewGame);

	auto it = m_mapPlayerIDToGameTuple.find(playerFirst);
	ASSERT(it != m_mapPlayerIDToGameTuple.end());

	auto tuplePlayerGame = std::make_tuple(std::get<0>(it->second), spNewGame);
	it->second = tuplePlayerGame;

	it = m_mapPlayerIDToGameTuple.find(playerSecond);
	ASSERT(it != m_mapPlayerIDToGameTuple.end());

	tuplePlayerGame = std::make_tuple(std::get<0>(it->second), spNewGame);
	it->second = tuplePlayerGame;

	auto itPlayerGameTuple = m_mapPlayerIDToGameTuple.find(playerSecond);
	ASSERT(itPlayerGameTuple != m_mapPlayerIDToGameTuple.end());

	std::unique_ptr<CommMessageBase> spStartInit = CommMessageFactory::CreateStartInitCommMessage(playerFirst, std::get<0>(itPlayerGameTuple->second), spNewGame->m_strQuestions);
	m_pServerQuiz->SendCommMessage(playerFirst, std::move(spStartInit));

	itPlayerGameTuple = m_mapPlayerIDToGameTuple.find(playerFirst);
	ASSERT(itPlayerGameTuple != m_mapPlayerIDToGameTuple.end());

	std::unique_ptr<CommMessageBase> spStartInitSecond = CommMessageFactory::CreateStartInitCommMessage(playerSecond, std::get<0>(itPlayerGameTuple->second), spNewGame->m_strQuestions);
	m_pServerQuiz->SendCommMessage(playerSecond, std::move(spStartInitSecond));

	SetNewQuestion(spNewGame);
}

void GamesManager::SetNewQuestion(std::shared_ptr<Game> spGame)
{
	std::lock_guard<std::recursive_mutex> lock(m_MapsMutex);

	size_t nQuestionsTotalNo = m_mapQuestionIDToQuestionItem.size();

	size_t pickedQuestionID = -1;

	if (spGame->m_historyAskedQuestionsIds.size() != 0)
	{
		std::vector<unsigned int>::iterator it;
		
		//make sure not to send the same question again
		do 
		{
			pickedQuestionID = rand() % nQuestionsTotalNo;
			it = std::find(spGame->m_historyAskedQuestionsIds.begin(), spGame->m_historyAskedQuestionsIds.end(), pickedQuestionID);
		} while (it != spGame->m_historyAskedQuestionsIds.end());
	}
	else
	{
		pickedQuestionID = rand() % nQuestionsTotalNo;
	}

	pickedQuestionID += 1; // due to the DB ID being 1 based

	spGame->m_historyAskedQuestionsIds.push_back(pickedQuestionID);

	auto questionIt = m_mapQuestionIDToQuestionItem.find(pickedQuestionID);
	ASSERT(questionIt != m_mapQuestionIDToQuestionItem.end());	

	std::string strSerializedQuestion;
	
	SerializeQuestionString(questionIt->second, spGame->m_strQuestions);
}

void GamesManager::SerializeQuestionString(const QuestionItem& questionItem, std::string& strSerializedQuestion)
{
	char szAnswerIDx[2];
	_itoa_s(questionItem.questionAnswerIdx, szAnswerIDx, sizeof(szAnswerIDx), 10);

	strSerializedQuestion += "1 | " + questionItem.questionText + " | " + questionItem.vecQuestionOptions[0] + " | "
		+ questionItem.vecQuestionOptions[1] + " | " + questionItem.vecQuestionOptions[2] + " | " + questionItem.vecQuestionOptions[3] + " | "
	    + szAnswerIDx;
}

void GamesManager::OnClientLoggedIn(const std::string& strPlayerName, unsigned short nServerClientPort)
{
	{
		std::lock_guard<std::recursive_mutex> lock(m_MapsMutex);
		auto tupleGame = std::make_tuple(strPlayerName, std::shared_ptr<Game>());

		m_mapPlayerIDToGameTuple.insert(std::make_pair(nServerClientPort, tupleGame));
	}
	
	CheckPlayersWithoutGame(nServerClientPort);
}

void GamesManager::OnSocketClosed(unsigned int nRemoteClientPort)
{
	// delete the client from the hash tables
	std::lock_guard<std::recursive_mutex> lock(m_MapsMutex);
	m_mapPlayerIDToGameTuple.erase(nRemoteClientPort);
}