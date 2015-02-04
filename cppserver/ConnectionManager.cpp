#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "StdAfx.h"
#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(boost::asio::io_service& io_service, int port)
	:m_io_service(io_service),
	acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
	m_port(port)
{
	m_index=1;
	startAccept();
	LOGNORMAL() <<" listen on port"<<port<<LOGEND();
}

ConnectionManager::~ConnectionManager()
{
	std::map<int, Connection*>::iterator iter = m_allConns.begin();
	for (; iter != m_allConns.end(); ++iter)
	{
		delete iter->second;
	}
	m_allConns.clear();
}

void ConnectionManager::startAccept()
{
	Connection* new_session = new Connection(m_io_service, m_index++);
	acceptor_.async_accept(
		new_session->socket(),
		boost::bind(&ConnectionManager::handleAccept, this, new_session,boost::asio::placeholders::error)
		);
	m_allConns.insert(std::make_pair(new_session->id(), new_session));
}

void ConnectionManager::handleAccept(Connection* new_session, const boost::system::error_code& error)
{
	if (!error)
	{
		LOGNORMAL() <<" handle accept success id"<<new_session->id() <<LOGEND();
		new_session->setErrHandler(boost::bind(&ConnectionManager::errHandler, this, boost::asio::placeholders::error, _2, _3));
		new_session->start();
	}
	else
	{
		LOGERROR() <<" handle accept id"<<new_session->id()
					<<"error"<<error
					<<LOGEND();
		std::map<int, Connection*>::iterator iter = m_allConns.find(new_session->id());
		m_allConns.erase(iter);
		delete new_session;
	}

	startAccept();
}

void ConnectionManager::errHandler(const boost::system::error_code& e, Connection* conn, std::string dummy)
{
	LOGNORMAL() <<dummy<<"conn disconnect id"<<conn->id()<<"e"<<e<<LOGEND();
	std::map<int, Connection*>::iterator iter = m_allConns.find(conn->id());
	m_allConns.erase(iter);
	
	delete conn;
}

Connection* ConnectionManager::getConn(int id)
{
	std::map<int, Connection*>::iterator iter=m_allConns.find(id);
	if (iter != m_allConns.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

void ConnectionManager::update()
{
	if (Util::getMSTime()-m_lastUpdateTime < 60)
	{
		return;
	}
	m_lastUpdateTime=Util::getMSTime();
	// ¼ì²â¿ÕÁ¬½Ó
	std::list<Connection*>	deadConn;
	std::map<int, Connection*>::iterator iter=m_allConns.begin();
	for (; iter != m_allConns.end(); iter++)
	{
		if (iter->second->getStatus() != CONN_STATUS_INIT)
		{
			if (Util::getMSTime() - iter->second->getConnSuccTime() > 60)
			{
				deadConn.push_back(iter->second);
			}
		}		
	}
	for (std::list<Connection*>::iterator iterDead=deadConn.begin();
		iterDead != deadConn.end(); ++iterDead)
	{
		LOGERROR()<<" forceclose"<<(*iterDead)->id()<<LOGEND();
		(*iterDead)->forceClose();
	}
}