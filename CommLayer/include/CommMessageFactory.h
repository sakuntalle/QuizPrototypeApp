#ifndef CommMessageFactory_h__
#define CommMessageFactory_h__

#include "CommMessageBase.h"
#include "CommHandlerHelpers.h"
#include "StartInitMessage.h"
#include "SendQuestionMessage.h"

//Forward declaration
struct QuestionItem;

// Factory for creating instances of all the messages that will be serialized
class CommMessageFactory
{
private:
	CommMessageFactory() { };
	CommMessageFactory(const CommMessageFactory &);
	CommMessageFactory &operator=(const CommMessageFactory &);

	~CommMessageFactory() {}

public:
	
	static std::unique_ptr<CommMessageBase> CreateLoginMessage			(const std::string& strPlayerName);

	static std::unique_ptr<CommMessageBase> CreateStartInitCommMessage	(unsigned short nPlayerId, const std::string& strOpponentName, const std::string& strQuestions);
	static std::shared_ptr<CommMessageBase> CreateStartInitCommMessage	(StartInitMessage* pStartInit);
	
	static std::unique_ptr<CommMessageBase> CreateQuestionCommMessage	(QuestionItem* pQuestion);
	static std::shared_ptr<CommMessageBase> CreateQuestionCommMessage	(SendQuestionMessage* pQuestion);
};

#endif // CommMessageFactory_h__
