#include "stdafx.h"
#include "LoginMessage.h"
#include "CommMessageBase.h"
#include "CommObserver.h"

/*CommMessageBase override*/
bool LoginMessage::HandleMessage(unsigned short nServerClientPort)
{
	CommObserverManager::GetInstance()->NotifyClientLoggedIn(m_strName, nServerClientPort);
	return true;
}

template<class Archive>
void LoginMessage::serialize(Archive &ar, const unsigned int /*version*/)
{
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CommMessageBase);
	ar & m_strName;
}

BOOST_CLASS_EXPORT_IMPLEMENT(LoginMessage)