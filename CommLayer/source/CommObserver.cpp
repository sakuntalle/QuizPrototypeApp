#include "stdafx.h"
#include "CommObserver.h"
#include <algorithm>

CommObserverManager::CommObserverManager()
{
	m_vecClientListeners.reserve(INITIAL_VECTOR_ALLOCATED_SPACE);
	m_vecServerListeners.reserve(INITIAL_VECTOR_ALLOCATED_SPACE);
}

// server notifications
void CommObserverManager::NotifyConnectionAccepted()
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	std::for_each(m_vecServerListeners.begin(), m_vecServerListeners.end(), [](ICommServerObserver* observer) { observer->OnConnectionAccepted(); });
}

void CommObserverManager::NotifyClientLoggedIn(const std::string& strPlayerName, unsigned short nServerClientPort)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	std::for_each(m_vecServerListeners.begin(), m_vecServerListeners.end(), [&](ICommServerObserver* observer) { observer->OnClientLoggedIn(strPlayerName, nServerClientPort); });
}

// client notifications
void CommObserverManager::NotifyClientConnected(bool bSuccess)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	std::for_each(m_vecClientListeners.begin(), m_vecClientListeners.end(), [&](ICommClientObserver* observer) { observer->OnClientConnected(bSuccess); });
}

void CommObserverManager::NotifyStartInit(std::shared_ptr<CommMessageBase> spStartInit)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	std::for_each(m_vecClientListeners.begin(), m_vecClientListeners.end(), [=](ICommClientObserver* observer) { observer->OnStartInitReceived(spStartInit); });
}

void CommObserverManager::NotifyDisplayQuestion(std::shared_ptr<CommMessageBase> spQuestion)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	std::for_each(m_vecClientListeners.begin(), m_vecClientListeners.end(), [=](ICommClientObserver* observer) { observer->OnQuestionReceived(spQuestion); });
}

void CommObserverManager::NotifySocketClosed(unsigned int nRemoteClientPort)
{
	{
		std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
		
		std::for_each(m_vecServerListeners.begin(), m_vecServerListeners.end(), [&nRemoteClientPort](ICommServerObserver* observer) { observer->OnSocketClosed(nRemoteClientPort); });
		
		std::for_each(m_vecClientListeners.begin(), m_vecClientListeners.end(), [](ICommClientObserver* observer) { observer->OnSocketClosed(); });
	}
}

void CommObserverManager::RegisterCommClientListener(ICommClientObserver* listener)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	m_vecClientListeners.push_back(listener);
}

void CommObserverManager::UnregisterCommClientListener(ICommClientObserver* listener)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	auto it = std::remove_if(m_vecClientListeners.begin(), m_vecClientListeners.end(), [listener](ICommClientObserver* observer) { return (observer == listener); });
	m_vecClientListeners.erase(it, m_vecClientListeners.end());
}

void CommObserverManager::RegisterCommServerListener(ICommServerObserver* listener)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	m_vecServerListeners.push_back(listener);
}

void CommObserverManager::UnregisterCommServerListener(ICommServerObserver* listener)
{
	std::lock_guard<std::recursive_mutex> lock(mListenersMutex);
	auto it = std::remove_if(m_vecServerListeners.begin(), m_vecServerListeners.end(), [listener](ICommServerObserver* observer) { return (observer == listener); });
	m_vecServerListeners.erase(it, m_vecServerListeners.end());
}

//CommObserverManager* CommObserverManager::GetInstance()
//{
//	static CommObserverManager instance;	// save for multi threading with C++ 11
//	return &instance;
//}

