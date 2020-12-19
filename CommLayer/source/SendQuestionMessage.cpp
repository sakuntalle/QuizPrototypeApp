#include "stdafx.h"
#include "SendQuestionMessage.h"
#include "CommMessageBase.h"
#include "CommObserver.h"
#include "CommMessageFactory.h"
#include "CommonIncludes.h"

/*CommMessageBase override*/
bool SendQuestionMessage::HandleMessage(unsigned short nServerClientPort)
{
	// TODO!!! Use shared_from_this to avoid the copy
	std::shared_ptr<CommMessageBase> spQuestion = CommMessageFactory::CreateQuestionCommMessage(this);
	CommObserverManager::GetInstance()->NotifyDisplayQuestion(spQuestion);
	return true;
}
void SendQuestionMessage::GetQuestionOptions(std::vector<std::string>& vecQuestionOptions) const
{
	vecQuestionOptions.reserve(QuestionItem::QUESTION_OPTIONS_NO);

	vecQuestionOptions.push_back(m_strQuestionOption0);
	vecQuestionOptions.push_back(m_strQuestionOption1);
	vecQuestionOptions.push_back(m_strQuestionOption2);
	vecQuestionOptions.push_back(m_strQuestionOption3);
}

template<class Archive>
void SendQuestionMessage::serialize(Archive &ar, const unsigned int /*version*/)
{
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CommMessageBase);
	ar & m_strQuestionText;
	ar & m_strQuestionOption0;
	ar & m_strQuestionOption1;
	ar & m_strQuestionOption2;
	ar & m_strQuestionOption3;
}

BOOST_CLASS_EXPORT_IMPLEMENT(SendQuestionMessage)