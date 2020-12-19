#ifndef CommHandler_h__
#define CommHandler_h__

#include <boost/asio.hpp>
#include "CommMessageBase.h"
#include "CommHandlerHelpers.h"

/*************************
The message communication protocol is described below

----------------------------------
CLIENT						SERVER
-----------------------------------

LOGIN -------------------->  When two clients without a game are logged on
									|
									|
									|
Question<-------------------- START_INIT
is displayed

***************************/
using boost::asio::ip::tcp;

class CommHandler
{
public:

	CommHandler(boost::asio::io_service& ioService);
	~CommHandler()
	{
	}

	void ReadData			();
	void AcceptNewConnection(std::shared_ptr<tcp::acceptor> acceptorConnection);
	bool ConnectToServer	(const std::string& strServerName);

	bool SendCommMessage(std::unique_ptr<CommMessageBase> message);

	void CloseSocket		();
	unsigned short GetSocketRemotePort() const { return m_Socket.remote_endpoint().port(); }

private:

	void HandleWriteMessage	(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleReadHeader	(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleReadBody		(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleAccept		(const boost::system::error_code& error);
	void HandleConnect		(const boost::system::error_code& error);
	void DoCloseSocket		();

	tcp::socket					m_Socket;
	CommMessage					mCommMessage;	// buffer used for sending and receiving data
	boost::asio::io_service&	m_ioService;
};
#endif // CommHandler_h__
