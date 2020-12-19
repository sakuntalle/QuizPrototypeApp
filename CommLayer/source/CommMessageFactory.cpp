#include "stdafx.h"
#include "CommMessageFactory.h"
#include "LoginMessage.h"
#include "CommonIncludes.h"

std::unique_ptr<CommMessageBase> CommMessageFactory::CreateLoginMessage(const std::string& strPlayerName)
{
	return std::unique_ptr<CommMessageBase>(new LoginMessage(strPlayerName));
}

std::unique_ptr<CommMessageBase> CommMessageFactory::CreateStartInitCommMessage(unsigned short nPlayerId, const std::string& strOpponentName, const std::string& strQuestions)
{
	return std::unique_ptr<CommMessageBase>(new StartInitMessage(nPlayerId, strOpponentName, strQuestions));
}

std::shared_ptr<CommMessageBase> CommMessageFactory::CreateStartInitCommMessage(StartInitMessage* pStartInit)
{
	// shared_ptr because this message will be given to different observers so the ownership needs to be shared between them
	return std::shared_ptr<CommMessageBase>(new StartInitMessage(pStartInit->GetPlayerId(), pStartInit->GetOpponentName(), pStartInit->GetQuestions()));
}

std::unique_ptr<CommMessageBase> CommMessageFactory::CreateQuestionCommMessage(QuestionItem* pQuestion)
{
	return std::unique_ptr<CommMessageBase>(new SendQuestionMessage(pQuestion->questionText, pQuestion->vecQuestionOptions));
}

std::shared_ptr<CommMessageBase> CommMessageFactory::CreateQuestionCommMessage(SendQuestionMessage* pQuestionMsg)
{
	std::vector<std::string> strQuestionOptions;
	pQuestionMsg->GetQuestionOptions(strQuestionOptions);

	// shared_ptr because this message will be given to different observers so the ownership needs to be shared between them
	return std::unique_ptr<CommMessageBase>(new SendQuestionMessage(pQuestionMsg->GetQuestionText(), strQuestionOptions));
}