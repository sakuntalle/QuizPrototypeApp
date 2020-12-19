#include "stdafx.h"
#include "StartInitMessage.h"
#include "CommMessageBase.h"
#include "CommObserver.h"
#include "CommMessageFactory.h"

/*CommMessageBase override*/
bool StartInitMessage::HandleMessage(unsigned short nServerClientPort)
{
	// copy data from the Boost serialize allocated buffer

	// TODO!!! Use shared_from_this to avoid the copy
	std::shared_ptr<CommMessageBase> spStartInit = CommMessageFactory::CreateStartInitCommMessage(this);
	CommObserverManager::GetInstance()->NotifyStartInit(spStartInit);

	return true;
}

template<class Archive>
void StartInitMessage::serialize(Archive &ar, const unsigned int /*version*/)
{
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CommMessageBase);
	ar & m_nPlayerId;
	ar & m_strOpponentName;
	ar & m_strQuestions;
}

BOOST_CLASS_EXPORT_IMPLEMENT(StartInitMessage)