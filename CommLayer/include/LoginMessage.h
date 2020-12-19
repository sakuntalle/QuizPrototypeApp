#ifndef LoginMessage_h__
#define LoginMessage_h__
#include "CommMessageBase.h"

class LoginMessage;
namespace boost {
	namespace serialization {
		template<class Archive>
		inline void save_construct_data(Archive & ar, const LoginMessage * t, const unsigned int file_version);
	}
}

class LoginMessage : public CommMessageBase
{
public:
	LoginMessage(const std::string& strPlayerName)
		: m_strName(strPlayerName)
	{
	}

	~LoginMessage()
	{

	}

	/*CommMessageBase override*/
	virtual bool HandleMessage(unsigned short nServerClientPort);

	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/);

private:
	friend class boost::serialization::access;
	template<class Archive> friend void boost::serialization::save_construct_data(Archive & ar, const LoginMessage * t, const unsigned int file_version);

	std::string		m_strName;
};

namespace boost {
	namespace serialization {
		template<class Archive>
		inline void save_construct_data(Archive & ar, const LoginMessage * t, const unsigned int file_version)
		{
			// save data required to construct instance
			ar << t->m_strName;
		}

		template<class Archive>
		inline void load_construct_data(Archive & ar, LoginMessage * t, const unsigned int file_version)
		{
			// retrieve data from archive required to construct new instance
			std::string strName;
			ar >> strName;
			// invoke inplace constructor to initialize instance of my_class
			::new(t)LoginMessage(strName);
		}
	}
} // namespace ...

BOOST_CLASS_EXPORT_KEY(LoginMessage)
#endif // LoginMessage_h__
