#include "stdafx.h"
#include "CommServer.h"
#include "CommHandlerHelpers.h"
#include "SQLiteDBManager.h"

CommServer::CommServer()
{
	tcp::endpoint endPoint(boost::asio::ip::address::from_string(QUIZ_LISTEN_IP), QUIZ_SERVER_PORT);
	m_ptrAcceptor.reset(new tcp::acceptor(m_ioService, endPoint));

	CommObserverManager::GetInstance()->RegisterCommServerListener(this);
	StartAccept();

	m_ptrGamesManager.reset(new GamesManager(this));
}

CommServer::~CommServer()
{
	CommObserverManager::GetInstance()->UnregisterCommServerListener(this);
}

bool CommServer::SendCommMessage(unsigned int nRemoteClientPort, std::unique_ptr<CommMessageBase> spCommMessage)
{
	// find the session where to send the message to
	CommSessionSP spSession;
	bool bFound = GetSessionByClientRemotePort(nRemoteClientPort, spSession);
	ASSERT(bFound == true);
	
	return spSession->SendCommMessage(std::move(spCommMessage));
}

void CommServer::StartAccept()
{
	CommSessionSP newSession = CommSessionSP(new CommSession(m_ioService));

	m_listSessions.push_back(newSession);

	newSession->AcceptNewConnection(m_ptrAcceptor);
}

bool CommServer::GetSessionByClientRemotePort(const unsigned short nClientRemotePort, CommSessionSP& spSession)
{
	auto it = std::find_if(m_listSessions.begin(), m_listSessions.end(),
		[&nClientRemotePort](CommSessionSP spSession)
	{
		return nClientRemotePort == spSession->GetClientRemotePort();
	});

	if (it != m_listSessions.end())
	{
		spSession = *it;
		return true;
	}

	return false;
}

bool CommServer::RemoveSessionByClientRemotePort(const unsigned short nClientRemotePort)
{
	auto it = std::remove_if(m_listSessions.begin(), m_listSessions.end(), 
		[&nClientRemotePort](CommSessionSP spSession)
	{
		return nClientRemotePort == spSession->GetClientRemotePort();
	});

	m_listSessions.erase(it, m_listSessions.end());

	return true;
}

/*ICommServerObserver implementation*/
void CommServer::OnConnectionAccepted()
{
	StartAccept();
}

void CommServer::OnSocketClosed(unsigned int nRemoteClientPort)
{
	RemoveSessionByClientRemotePort(nRemoteClientPort);
}