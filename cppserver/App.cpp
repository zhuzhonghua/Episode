#include "StdAfx.h"

#include "App.h"
#include "MsgHandler.h"
#include "RoomManager.h"

#include "ConnectionManager.h"

App* App::s_inst;
App* App::inst()
{
	return s_inst?s_inst:(s_inst = new App());
}

App::App():m_tick(m_io, boost::posix_time::microsec(TICK_MS)),m_pFSMaster(NULL)
{

}

bool App::init()
{
	// *log thread
	if(!LOGTHREAD_P()->init())
	{
		SLOWLOG() << "log thread init false!!!!!!!!!!!!!!!!" << LOGEND();
		return false;
	}	
	SLOWLOG() << "log init OK!" << LOGEND();

	// *init connection to master
	m_pFSMaster = new ServerConnection(m_io);
	BaseConfig* bc = Config::get()->getBaseConfig();
	m_pFSMaster->init("127.0.0.1", 1234);
	SLOWLOG() << "connection to master init OK!" << LOGEND();

	m_pFS = new ConnectionManager(m_io, 5678);

	// init msg handler
	MsgHandler::init();
	SLOWLOG() << "msg handler init OK!" << LOGEND();

	return true;
}

void App::loop()
{
	m_tick.async_wait(boost::bind(&App::tick, this, boost::asio::placeholders::error));

	m_io.run();
}

void App::tick(const boost::system::error_code& e)
{
	if (boost::asio::error::operation_aborted != e)
	{
		PTime pStart = Util::getPTime();

		m_pFSMaster->update();
		m_pFS->update();

		PTime pEnd = Util::getPTime();
		int duration = Util::getDuration(pEnd, pStart);

		if ( duration <= TICK_MS)
		{
			m_tick.expires_at(m_tick.expires_at()+boost::posix_time::millisec(TICK_MS-duration));
		}
		
		m_tick.async_wait(boost::bind(&App::tick, this, boost::asio::placeholders::error));
	}
}

void App::exit()
{

}

ServerConnection* App::getFSMasterConn()
{
	return m_pFSMaster;
}

ConnectionManager* App::getFS()
{
	return m_pFS;
}