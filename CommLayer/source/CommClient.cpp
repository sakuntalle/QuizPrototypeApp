#include "stdafx.h"
#include "CommClient.h"
#include "CommMessageFactory.h"
#include "LoginMessage.h"
#include <boost/archive/text_oarchive.hpp>

CommClient::CommClient()
{
	CommObserverManager::GetInstance()->RegisterCommClientListener(this);
}

CommClient::~CommClient()
{
	CommObserverManager::GetInstance()->UnregisterCommClientListener(this);
	m_ptrCommHandler.reset();
}

bool CommClient::Connect(const std::string& strServerName, const std::string& strPlayerName)
{
	try
	{
		// create a socket and communication objects
		m_ptrCommHandler.reset(new CommHandler(m_ioService));

		bool bResult = m_ptrCommHandler->ConnectToServer(strServerName);
		if (bResult)
		{
			m_strPlayerName = strPlayerName;
		}

		return bResult;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";

		return false;
	}
}

/*ICommClientObserver implementation*/

void CommClient::OnClientConnected(bool bSuccess)
{
	if (bSuccess)
	{
		std::unique_ptr<CommMessageBase> spLogin = CommMessageFactory::CreateLoginMessage(m_strPlayerName);
		m_ptrCommHandler->SendCommMessage(std::move(spLogin));
	}
}

void CommClient::OnStartInitReceived(std::shared_ptr<CommMessageBase> spStartInit)
{

}

void CommClient::OnSocketClosed()
{
	m_ptrCommHandler.reset();
}

