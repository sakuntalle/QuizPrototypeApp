#include "stdafx.h"

#include <string.h>

#include "CommSession.h"
#include "CommServer.h"
#include "CommObserver.h"

CommSession::CommSession(boost::asio::io_service& ioService)
	: m_commHandler(ioService)

{
	CommObserverManager::GetInstance()->RegisterCommServerListener(this);
	m_nRemotePort = INVALID_PLAYER_ID;
}

CommSession::~CommSession()
{
	CommObserverManager::GetInstance()->UnregisterCommServerListener(this);
}

void CommSession::AcceptNewConnection(std::shared_ptr<tcp::acceptor> acceptorConnection)
{
	m_commHandler.AcceptNewConnection(acceptorConnection);
}

bool CommSession::SendCommMessage(std::unique_ptr<CommMessageBase> spCommMessage)
{
	return m_commHandler.SendCommMessage(std::move(spCommMessage));
}

/*ICommServerObserver implementation*/
void CommSession::OnConnectionAccepted()
{
	m_nRemotePort = m_commHandler.GetSocketRemotePort();
}
