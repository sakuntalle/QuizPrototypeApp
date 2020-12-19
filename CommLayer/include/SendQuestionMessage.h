#ifndef SendQuestionMessage_h__
#define SendQuestionMessage_h__

#include "CommMessageBase.h"

class SendQuestionMessage;
namespace boost {
	namespace serialization {
		template<class Archive>
		inline void save_construct_data(Archive & ar, const SendQuestionMessage * t, const unsigned int file_version);
	}
}

class SendQuestionMessage : public CommMessageBase
{
public:

	static const short QUESTION_OPTIONS_NO = 4;

	SendQuestionMessage(const std::string& strQuestionText, const std::vector<std::string>& vecQuestionOptions)
		: m_strQuestionText(std::move(strQuestionText)),
		m_strQuestionOption0(std::move(vecQuestionOptions[0])),
		m_strQuestionOption1(std::move(vecQuestionOptions[1])),
		m_strQuestionOption2(std::move(vecQuestionOptions[2])),
		m_strQuestionOption3(std::move(vecQuestionOptions[3]))
	{
	}

	~SendQuestionMessage()
	{

	}

	//Access
	const std::string&	GetQuestionText	  ()										     const { return m_strQuestionText; }
	void				GetQuestionOptions(std::vector<std::string>& vecQuestionOptions) const;

	/*CommMessageBase override*/
	virtual bool HandleMessage(unsigned short nServerClientPort);

	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/);

private:

	friend class boost::serialization::access;
	template<class Archive> friend void boost::serialization::save_construct_data(Archive & ar, const SendQuestionMessage * t, const unsigned int file_version);

	std::string					m_strQuestionText;
	std::string					m_strQuestionOption0;
	std::string					m_strQuestionOption1;
	std::string					m_strQuestionOption2;
	std::string					m_strQuestionOption3;
};

namespace boost {
	namespace serialization {
		template<class Archive>
		inline void save_construct_data(Archive & ar, const SendQuestionMessage * t, const unsigned int file_version)
		{
			// save data required to construct instance
			ar << t->m_strQuestionText;
			ar << t->m_strQuestionOption0;
			ar << t->m_strQuestionOption1;
			ar << t->m_strQuestionOption2;
			ar << t->m_strQuestionOption3;
		}

		template<class Archive>
		inline void load_construct_data(Archive & ar, SendQuestionMessage * t, const unsigned int file_version)
		{
			// retrieve data from archive required to construct new instance
			std::string strQuestionText;
			ar >> strQuestionText;
			std::vector<std::string> vecQuestionOptions(QuestionItem::QUESTION_OPTIONS_NO);
			ar >> vecQuestionOptions[0];
			ar >> vecQuestionOptions[1];
			ar >> vecQuestionOptions[2];
			ar >> vecQuestionOptions[3];

			// invoke inplace constructor to initialize instance of my_class
			::new(t)SendQuestionMessage(std::move(strQuestionText), std::move(vecQuestionOptions));
		}
	}
} // namespace ...

BOOST_CLASS_EXPORT_KEY(SendQuestionMessage)

#endif // SendQuestionMessage_h__