#ifndef CommServer_h__
#define CommServer_h__

#include <string.h>
#include <list>
#include "CommSession.h"
#include "CommObserver.h"
#include "GamesManager.h"
#include "CommBase.h"

typedef std::shared_ptr<CommSession> CommSessionSP;

/*
The classes ownership :
CQuizServerDlg has one CommServer
CommServer allocates a CommSession for every connection opened by a client
CommSession has a CommHandler. 
CommSession is destroyed when the socket is closed
CommServer has one GamesManager
*/
class CommServer 
	: public CommBase
	, public ICommServerObserver
{

public:
	CommServer();
	~CommServer();

	bool SendCommMessage(unsigned int nRemoteClientPort, std::unique_ptr<CommMessageBase> spCommMessage);

private:
	void StartAccept();
	bool GetSessionByClientRemotePort(const unsigned short nClientRemotePort, CommSessionSP& spSession);
	bool RemoveSessionByClientRemotePort(const unsigned short nClientRemotePort);

	/*ICommServerObserver implementation*/
	virtual void OnConnectionAccepted();
	virtual void OnClientLoggedIn(const std::string& strPlayerName, unsigned short nServerClientPort) { };
	virtual void OnSocketClosed(unsigned int nRemoteClientPort);
	
private:

	std::shared_ptr<tcp::acceptor>	m_ptrAcceptor;

	std::list<CommSessionSP>		m_listSessions;
	std::unique_ptr<GamesManager>	m_ptrGamesManager;				// Manager that keeps track of various infos related to the connected clientss
};
#endif // CommServer_h__
