#ifndef CommonIncludes_h__
#define CommonIncludes_h__

#include <vector>
#include <unordered_map>

struct QuestionItem
{
	static const short QUESTION_OPTIONS_NO = 4;

	QuestionItem(const char* Text, const char* Option0, const char* Option1, const char* Option2, const char* Option3, const int AnswerIdx)
	{
		questionText = Text;
		vecQuestionOptions.reserve(QUESTION_OPTIONS_NO);
		vecQuestionOptions.push_back(Option0);
		vecQuestionOptions.push_back(Option1);
		vecQuestionOptions.push_back(Option2);
		vecQuestionOptions.push_back(Option3);
		questionAnswerIdx = AnswerIdx;
	}

	std::string questionText;
	std::vector<std::string> vecQuestionOptions;
	short questionAnswerIdx;
};

typedef std::unordered_map<unsigned int, QuestionItem> QuestionsMap;

#endif // CommonIncludes_h__
