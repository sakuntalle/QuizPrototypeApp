#ifndef StartInitMessage_h__
#define StartInitMessage_h__
#include "CommMessageBase.h"
#include "CommHandlerHelpers.h"
#include <memory>

class StartInitMessage;
namespace boost {
	namespace serialization {
		template<class Archive>
		inline void save_construct_data(Archive & ar, const StartInitMessage * t, const unsigned int file_version);
	}
}

class StartInitMessage 
	: public CommMessageBase
{

public:
	StartInitMessage(unsigned short nPlayerId, const std::string& strOpponentName, const std::string& strQuestions)
		: m_nPlayerId(nPlayerId)
		, m_strOpponentName(std::move(strOpponentName))
		, m_strQuestions(std::move(strQuestions))
	{
	}

	/*CommMessageBase override*/
	virtual bool HandleMessage(unsigned short nServerClientPort);

	unsigned int	   GetPlayerId()	 const { return m_nPlayerId; }
	const std::string& GetOpponentName() const { return m_strOpponentName; }
	const std::string& GetQuestions()	 const { return m_strQuestions; }

	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/);

private:
	friend class boost::serialization::access;
	template<class Archive> friend void boost::serialization::save_construct_data(Archive & ar, const StartInitMessage * t, const unsigned int file_version);

	unsigned short		m_nPlayerId; 
	std::string			m_strOpponentName; 
	std::string			m_strQuestions;
};


namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive & ar, const StartInitMessage * t, const unsigned int file_version)
{
	// save data required to construct instance
	ar << t->m_nPlayerId;
	ar << t->m_strOpponentName;
	ar << t->m_strQuestions;
}

template<class Archive>
inline void load_construct_data(Archive & ar, StartInitMessage * t, const unsigned int file_version)
{
	// retrieve data from archive required to construct new instance
	unsigned short nPlayerId;
	ar >> nPlayerId;
	std::string strOpponentName;
	ar >> strOpponentName;
	std::string strQuestion;
	ar >> strQuestion;
	// invoke inplace constructor to initialize instance of my_class
	::new(t)StartInitMessage(nPlayerId, std::move(strOpponentName), std::move(strQuestion));
}
}} // namespace ...

BOOST_CLASS_EXPORT_KEY(StartInitMessage)

#endif // StartInitMessage_h__
