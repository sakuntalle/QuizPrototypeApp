#include "stdafx.h"
#include "CommHandler.h"
#include <boost/bind.hpp>
#include "CommObserver.h"
#include "CommMessageFactory.h"
#include <boost/serialization/export.hpp>
#include "LoginMessage.h"
#include "StartInitMessage.h"

CommHandler::CommHandler(boost::asio::io_service& ioService)
	: m_Socket(ioService)
	, m_ioService(ioService)
{
}

void CommHandler::ReadData()
{
	boost::asio::async_read(m_Socket,
		boost::asio::buffer(mCommMessage.GetData(), CommMessage::HEADER_LENGTH),
		boost::bind(&CommHandler::HandleReadHeader, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void CommHandler::AcceptNewConnection(std::shared_ptr<tcp::acceptor> acceptorConnection)
{
	acceptorConnection->async_accept(m_Socket,
		boost::bind(&CommHandler::HandleAccept, this, boost::asio::placeholders::error));
}

bool CommHandler::ConnectToServer(const std::string& strServerName)
{
	try
	{
		tcp::resolver resolver(m_ioService);
		tcp::resolver::query query(strServerName, QUIZ_SERVER_PORT_STRING);
		tcp::resolver::iterator endpointIterator = resolver.resolve(query);

		boost::asio::async_connect(m_Socket, endpointIterator,
			boost::bind(&CommHandler::HandleConnect, this,
			boost::asio::placeholders::error));

		return true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		return false;
	}
}



bool CommHandler::SendCommMessage(std::unique_ptr<CommMessageBase> message)
{
	try{
		// serialize command ( serialized buffer size | serialized buffer ) 
		boost::asio::streambuf commBuffer;
		std::ostream osBuffer(&commBuffer);
		boost::archive::text_oarchive outArchive(osBuffer);

		// serialize message using Boost.Serialize
		outArchive << message.get();

		std::stringstream ssSend;
		ssSend << osBuffer.rdbuf();

		mCommMessage.SetBodyMessage(ssSend.str());

		boost::asio::async_write(m_Socket, boost::asio::buffer(mCommMessage.GetData(), mCommMessage.GetSendMessageLength()),
			boost::bind(&CommHandler::HandleWriteMessage, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		
		return true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		DoCloseSocket();
		return false;
	}
}

void CommHandler::CloseSocket()
{
	m_ioService.post(boost::bind(&CommHandler::DoCloseSocket, this));
}

void CommHandler::HandleWriteMessage(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (!error)
	{
		boost::asio::async_read(m_Socket,
			boost::asio::buffer(mCommMessage.GetData(), CommMessage::HEADER_LENGTH),
			boost::bind(&CommHandler::HandleReadHeader, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		DoCloseSocket();
	}
}

void CommHandler::HandleReadHeader(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (!error && mCommMessage.ParseHeader())
	{
		boost::asio::async_read(m_Socket,
			boost::asio::buffer(mCommMessage.GetData(), mCommMessage.GetBodyLength()),
			boost::bind(&CommHandler::HandleReadBody, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		DoCloseSocket();
	}
}

void CommHandler::HandleReadBody(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	try
	{
		if (!error)
		{
			std::istringstream ss(std::string(mCommMessage.GetData(), mCommMessage.GetBodyLength()));
			boost::archive::text_iarchive inArchive(ss);

			// deserialize message using Boost.Serialize
			CommMessageBase* pRecvMsg = nullptr;
			inArchive >> pRecvMsg;

			mCommMessage.PurgeBuffer();

			std::unique_ptr<CommMessageBase> spRecvMsg(pRecvMsg);

			unsigned short nServerClientPort = m_Socket.remote_endpoint().port();
			spRecvMsg->HandleMessage(nServerClientPort);
		}
		else
		{
			DoCloseSocket();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		DoCloseSocket();
	}
}

void CommHandler::HandleAccept(const boost::system::error_code& error)
{
	if (!error)
	{
		ReadData();
		CommObserverManager::GetInstance()->NotifyConnectionAccepted();
	}
	else
	{
		DoCloseSocket();
		std::cerr << "Error accepting new connections" << "\n";
	}
}

void CommHandler::HandleConnect(const boost::system::error_code& error)
{
	if (!error)
	{
		CommObserverManager::GetInstance()->NotifyClientConnected(true);
	}	
	else
	{
		CommObserverManager::GetInstance()->NotifyClientConnected(false);
	}
}

void CommHandler::DoCloseSocket()
{
	unsigned short nServerClientPort = 0;
	try
	{
		nServerClientPort = m_Socket.remote_endpoint().port();

		m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		m_Socket.close();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		try
		{
			m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			m_Socket.close();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}

	CommObserverManager::GetInstance()->NotifySocketClosed(nServerClientPort);
}