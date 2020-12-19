#ifndef CommObserver_h__
#define CommObserver_h__
#include <vector>
#include <mutex>
#include <memory>
#include "CommMessageBase.h"

// abstract class for the Observer pattern
class ICommClientObserver
{
public:
	virtual void OnClientConnected	(bool bSuccess)	= 0;
	virtual void OnStartInitReceived(std::shared_ptr<CommMessageBase> spStartInit) = 0;
	virtual void OnQuestionReceived(std::shared_ptr<CommMessageBase> spQuestion) = 0;
	virtual void OnSocketClosed		() = 0;
};

// abstract class for the Observer pattern
class ICommServerObserver
{
public:
	virtual void OnConnectionAccepted	() = 0;
	virtual void OnClientLoggedIn		(const std::string& strPlayerName, unsigned short nServerClientPort) = 0;
	virtual void OnSocketClosed			(unsigned int nRemoteClientPort) = 0;
};

// Observer manager that handles the listeners and fires the events
// Implemented as a Singleton
class CommObserverManager
{
public:
	// server notifications
	void NotifyConnectionAccepted();
	void NotifyClientLoggedIn(const std::string& strPlayerName, unsigned short nServerClientPort);
	
	// client notifications
	void NotifyClientConnected	(bool bSuccess);
	void NotifyStartInit		(std::shared_ptr<CommMessageBase> spStartInit);
	void NotifyDisplayQuestion  (std::shared_ptr<CommMessageBase> spQuestion);

	// client + server notifications
	void NotifySocketClosed(unsigned int nRemoteClientPort);

	void RegisterCommClientListener(ICommClientObserver* listener);
	void UnregisterCommClientListener(ICommClientObserver* listener);

	void RegisterCommServerListener(ICommServerObserver* listener);
	void UnregisterCommServerListener(ICommServerObserver* listener);

	static CommObserverManager* GetInstance()
	{
		static CommObserverManager instance;	// save for multi threading with C++ 11
		return &instance;
	}

private:
	enum 
	{
		INITIAL_VECTOR_ALLOCATED_SPACE = 10
	};

	CommObserverManager();
	CommObserverManager(const CommObserverManager &);
	CommObserverManager &operator=(const CommObserverManager &);

	// mutex for protecting access to the listeners vector
	std::recursive_mutex mListenersMutex;					
	
	std::vector<ICommClientObserver*> m_vecClientListeners;				// for now we are adding and deleting rarely but we iterate frequently so list is not optimum
	std::vector<ICommServerObserver*> m_vecServerListeners;				// for now we are adding and deleting rarely but we iterate frequently so list is not optimum
};
#endif // CommObserver_h__
