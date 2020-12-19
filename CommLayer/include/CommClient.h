#ifndef CommClient_h__
#define CommClient_h__

#include <iostream>
#include "CommMessageBase.h"
#include "CommHandler.h"
#include "CommObserver.h"
#include "CommBase.h"

/*
The classes ownership:
CQuizClientDlg has one CommClient
CommClient allocates one CommHandler
CommHandler is destroyed when the socket is closed
Another CommHandler is created when connecting again to the server
*/

class CommClient 
	: public CommBase
	, public ICommClientObserver
{
public:
	CommClient();
	~CommClient();

	bool Connect(const std::string& strServerName, const std::string& strPlayerName);

	/*ICommClientObserver implementation*/
	virtual void OnClientConnected(bool bSuccess);
	virtual void OnStartInitReceived(std::shared_ptr<CommMessageBase> spStartInit);
	virtual void OnQuestionReceived(std::shared_ptr<CommMessageBase> spQuestion) { };
	virtual void OnSocketClosed();

private:
	std::string						m_strPlayerName;

	std::unique_ptr<CommHandler>	m_ptrCommHandler;		// object that handles the actual asynchronous socket communication
};
#endif // CommClient_h__