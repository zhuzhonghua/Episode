#ifndef __FIGHTSERVER_APP_H__
#define __FIGHTSERVER_APP_H__

#include <boost/asio.hpp>

#include "ServerConnection.h"

class ConnectionManager;

class App
{
public:
	static App* inst();
	bool init();
	void loop();
	void exit();
public:
	ServerConnection*			getFSMasterConn();
	ConnectionManager*				getFS();
protected:
	void tick(const boost::system::error_code& e);
	App();
	static App*					s_inst;
protected:
	boost::asio::io_service		m_io;
	boost::asio::deadline_timer m_tick;

	ServerConnection*			m_pFSMaster;
	ConnectionManager*				m_pFS;
};


#endif
