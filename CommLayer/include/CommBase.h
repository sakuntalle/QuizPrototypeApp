#ifndef CommBase_h__
#define CommBase_h__

#include <boost/asio.hpp>
#include <thread>

using boost::asio::ip::tcp;

class CommBase
{
public:
	CommBase();
	virtual ~CommBase() = 0;

protected:

	boost::asio::io_service			m_ioService;
	std::unique_ptr<std::thread>	m_ptrCommThread;			// boost ASIO thread
	std::unique_ptr<boost::asio::io_service::work> m_ptrWork;	// used for keeping the ioService thread busy
};

#endif // CommBase_h__