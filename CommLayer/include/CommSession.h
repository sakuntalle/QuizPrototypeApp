#ifndef CommSession_h__
#define CommSession_h__
#include <boost/asio.hpp>
#include <string.h>
#include <memory>
#include "CommMessageBase.h"
#include "CommHandler.h"
#include "CommObserver.h"

class CommServer;
using boost::asio::ip::tcp;

class CommSession : public ICommServerObserver
{
public:
	CommSession(boost::asio::io_service& ioService);
	~CommSession();

	void AcceptNewConnection(std::shared_ptr<tcp::acceptor> acceptorConnection);
	
	unsigned short GetClientRemotePort() const { return m_nRemotePort; }

	//bool SendStartInitMessage(std::unique_ptr<StartInitInfo> spStartInit);
	bool SendCommMessage(std::unique_ptr<CommMessageBase> spCommMessage);

	/*ICommObserver implementation*/
	virtual void OnConnectionAccepted();
	virtual void OnClientLoggedIn(const std::string& strPlayerName, unsigned short nServerClientPort) { };
	virtual void OnSocketClosed(unsigned int nRemoteClientPort) { };

private:

	CommHandler	m_commHandler;
	unsigned short m_nRemotePort;
};
#endif // CommSession_h__
