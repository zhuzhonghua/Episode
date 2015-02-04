#ifndef __CONNECTIONMANAGER_H__
#define __CONNECTIONMANAGER_H__

#include <boost/asio.hpp>
#include <list>

#include "Packet.h"
#include "Connection.h"

using namespace boost::asio;

class ConnectionManager {
public:
	ConnectionManager(boost::asio::io_service& io_service, int port);
	~ConnectionManager();
public:
	void handleAccept(Connection* new_session, const boost::system::error_code& error);
	void errHandler(const boost::system::error_code& e, Connection*, std::string dummy);
	void update();
public:
	Connection*		getConn(int id);
protected:
	boost::asio::io_service&	m_io_service;
	tcp::acceptor acceptor_;

	int				m_index;
	std::map<int, Connection*>	m_allConns;
	int				m_port;

	double			m_lastUpdateTime;
protected:
	void startAccept();
};

#endif