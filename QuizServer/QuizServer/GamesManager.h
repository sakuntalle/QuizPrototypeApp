#ifndef GameManager_h__
#define GameManager_h__

#include "CommObserver.h"
#include <unordered_map>
#include <memory>
#include <tuple>
#include "CommonIncludes.h"

//forward declaration
class CommServer;

struct Game;

typedef std::tuple<std::string, std::shared_ptr<Game>> GameTuple;

class GamesManager : public ICommServerObserver
{
public:
	GamesManager(CommServer* pCommServer);

	~GamesManager();

	static const short QUESTIONS_MAX_NO = 5;
	const short INITIAL_ALLOCATED_PLAYERS_NUMBER = 20;

private:

	//Helpers
	bool IsAnotherPlayerAvailable(const unsigned short& currPlayerId, unsigned short& foundPlayerId);
	void CheckPlayersWithoutGame(const unsigned short& currPlayerId);
	void StartGame(const unsigned short& playerFirst, const unsigned short& playerSecond);
	void SetNewQuestion(std::shared_ptr<Game> spGame);

	static void SerializeQuestionString(const QuestionItem& questionItem, std::string& strSerializedQuestion);

	/*ICommServerObserver implementation*/
	virtual void OnConnectionAccepted() { };
	virtual void OnClientLoggedIn(const std::string& strPlayerName, unsigned short nServerClientPort);
	virtual void OnSocketClosed(unsigned int nRemoteClientPort);

private:
	CommServer*													m_pServerQuiz;
	
	// synchronization object for the game manager hash tables. Used for fast lookup and delete
	std::recursive_mutex m_MapsMutex;
	
	std::unordered_map<unsigned short, GameTuple>				m_mapPlayerIDToGameTuple;
	QuestionsMap												m_mapQuestionIDToQuestionItem;
};

struct Game
{
	Game(unsigned short nPlayerId, unsigned short nOpponentID)
		: m_nPlayerId(nPlayerId)
		, m_nOpponentID(nOpponentID)
		, m_strQuestions("")
	{
		m_historyAskedQuestionsIds.reserve(GamesManager::QUESTIONS_MAX_NO);
	}

	// these represent actually the client remote port from the server side socket
	unsigned short			  m_nPlayerId;
	unsigned short			  m_nOpponentID;

	// on the hurry way to add a question in the game. 
	// Ideally GameManager should send 5 specific command each of them containinig the Question  
	std::string				  m_strQuestions;
	std::vector<unsigned int> m_historyAskedQuestionsIds;
};
#endif // GameManager_h__
