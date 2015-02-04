#ifndef __CONNECTIONMANAGER_SERVER_CONNECTION_H__
#define __CONNECTIONMANAGER_SERVER_CONNECTION_H__

#include <boost/asio.hpp>
#include <list>

#include "Packet.h"
#include "Connection.h"

using namespace boost::asio;

/*
1. 连接某地址 ip port
2. 断开连接后，不停尝试继续连接
3. 连接成功后，发送handshake，后续定时发送心跳
*/
class ServerConnection:public Connection
{
public:
	ServerConnection(boost::asio::io_service& io_service);
	~ServerConnection();
	void init(std::string ip, int port);
	void update();
protected:
	void connectHandler(const boost::system::error_code& e);
	void tick(const boost::system::error_code& e);	
	
	
	void tryConnect();
	void errHandler(const boost::system::error_code& e, Connection*, std::string dummy);
protected:
	std::string			m_ip;
	int					m_port;
	

	boost::asio::deadline_timer m_tickTimer;
};

#endif