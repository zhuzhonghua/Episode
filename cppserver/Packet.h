#ifndef __FIGHTSERVER_PACKET_H__
#define __FIGHTSERVER_PACKET_H__

#include <string>

class Packet
{
public:
	Packet(int opCode, std::string &buf)
	{
		m_opCode = opCode;
		m_strBuf = buf;
	}
	int getOP()
	{
		return m_opCode;
	}
	std::string* getData()
	{
		return &m_strBuf;
	}
private:
	int			m_opCode;
	std::string m_strBuf;
};

class SendPacket
{
public:
	virtual short		getOPCode() = 0;
	virtual std::string getData()	= 0;
};

#define MSG_BEGIN(name, msgName, op) \
	class name : public msgName, public SendPacket \
	{ \
	public: \
		short getOPCode(){ return op; } \
		std::string getData(){ return this->SerializeAsString(); }

#define MSG_END() \
	};

#define GETMSG(name, p) \
	name msg; \
	bool okFlag = msg.ParseFromString(*p->getData()); \
	if(okFlag) \
	{\
	LOGNET() << "OK! parse msg " << p->getOP() << " [" << #name << "]" << LOGEND(); \
	}\
	else\
	{\
	LOGERROR() << "Error! parse msg " << p->getOP() << " ["<< #name << "]" << LOGEND(); \
	}

#endif