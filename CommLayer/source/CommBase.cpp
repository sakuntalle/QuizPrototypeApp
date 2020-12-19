#include "stdafx.h"
#include "CommBase.h"
#include <boost/bind.hpp>

CommBase::CommBase()
{
	m_ptrWork.reset(new boost::asio::io_service::work(m_ioService));
	m_ptrCommThread.reset(new std::thread(boost::bind(&boost::asio::io_service::run, &m_ioService)));
}

CommBase::~CommBase()
{
	m_ioService.stop();
	m_ptrWork.reset();
	m_ptrCommThread->join();
}