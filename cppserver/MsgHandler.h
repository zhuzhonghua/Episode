#ifndef __FIGHTSERVER_MSGHANDLER_H__
#define __FIGHTSERVER_MSGHANDLER_H__

#include <map>
#include "Connection.h"

class MsgHandler;
class Packet;

typedef bool (*MsgHandlerType)(Connection*, Packet* p);

class MsgHandler
{
public:
	static void				init();
	static MsgHandlerType	getHandler(int opCode);
public:
private:
	static std::map<int, MsgHandlerType> _s_handler;
};

#endif