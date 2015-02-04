#include "StdAfx.h"

#include "MsgHandler.h"
#include "Packet.h"

#include "Util.h"

using namespace com::magetower::op;
std::map<int, MsgHandlerType> MsgHandler::_s_handler;

void MsgHandler::init()
{
	
}

MsgHandlerType MsgHandler::getHandler(int opCode)
{
	std::map<int, MsgHandlerType>::iterator iter = MsgHandler::_s_handler.find(opCode);
	if (iter != MsgHandler::_s_handler.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}
