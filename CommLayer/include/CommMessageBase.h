#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <memory>

#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <sstream>

class CommMessage
{
public:
	enum { HEADER_LENGTH = 4 };
	enum { MAX_BODY_LENGTH = 512 };

	CommMessage()
		:mMsgBodyLength(0)
	{
	}

	char* GetData()
	{
		return mMsgBodyData;
	}

	size_t GetSendMessageLength() const
	{
		return HEADER_LENGTH + mMsgBodyLength;
	}

	size_t GetBodyLength() const
	{
		return mMsgBodyLength;
	}

	bool ParseHeader()
	{
		char szHeader[HEADER_LENGTH + 1] = "";
		strncat_s(szHeader, sizeof(szHeader), mMsgBodyData, HEADER_LENGTH);
		mMsgBodyLength = std::atoi(szHeader);

		if (mMsgBodyLength > MAX_BODY_LENGTH)
		{
			mMsgBodyLength = 0;
			return false;
		}

		return true;
	}

	void SetBodyMessage(const std::string& bodyMessage)
	{
		char szHeader[HEADER_LENGTH + 1] = "";
		sprintf_s(szHeader, sizeof(szHeader), "%4d", static_cast<int>(bodyMessage.size()));

		// format the message that will be sent 4 bytes of message size followed by the actual message
		memcpy(mMsgBodyData, szHeader, HEADER_LENGTH);
		memcpy(mMsgBodyData + HEADER_LENGTH, bodyMessage.c_str(), bodyMessage.size());

		mMsgBodyLength = bodyMessage.size();
	}

	void PurgeBuffer()
	{
		mMsgBodyData[0] = 0;
	}

private:
	char mMsgBodyData[HEADER_LENGTH + MAX_BODY_LENGTH];
	size_t mMsgBodyLength;
};

// base class for all the send and received messages
// serialized with Boost Serialize
class CommMessageBase
{

public:

	virtual ~CommMessageBase() {}

	virtual bool HandleMessage(unsigned short nServerClientPort) { return true; }

	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/);

private:
	// for Boost serialize
	friend class boost::serialization::access;
};

BOOST_CLASS_EXPORT_KEY(CommMessageBase)

#endif // MESSAGE_H
