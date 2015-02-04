#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "StdAfx.h"
#include "ServerConnection.h"

ServerConnection::ServerConnection(boost::asio::io_service& io_service)
	:Connection(io_service), 
	m_tickTimer(io_service)
{
}

ServerConnection::~ServerConnection()
{

}

void ServerConnection::init(std::string ip, int port)
{
	m_ip	= ip;
	m_port	= port;

	this->setErrHandler(boost::bind(&ServerConnection::errHandler, this, boost::asio::placeholders::error, _2, _3));
	tryConnect();
	/*
	m_tickTimer.expires_from_now(boost::posix_time::seconds(CONNECT_INTERVAL));	
	m_tickTimer.async_wait(boost::bind(&ServerConnection::tick, this, boost::asio::placeholders::error));*/
}

void ServerConnection::update()
{
	switch(m_status)
	{
	//case CONN_STATUS_INIT:		
	//	break;
	////case CONN_STATUS_CONNECTING:
	////	// wait
	////	break;
	//case CONN_STATUS_CONNECTED:
	//	// handle command
	//	handlePacket();
	//	break;
	//case CONN_STATUS_DISCONNECT:
	//	{
	//		tryConnect();
	//	}		
	//	break;
	}
}

void ServerConnection::connectHandler(const boost::system::error_code& e)
{
	if(!e)
	{
		LOGNORMAL() << "connect " << m_ip.c_str() << " " << m_port << " OK" << LOGEND();
		//m_status = CONN_STATUS_CONNECTED;
		start();
		// send handshake message		

		// trigger read
		//tryRead();
		
	}
	else
	{
		m_status = CONN_STATUS_DISCONNECT;
		LOGERROR() << "connect error " << m_ip.c_str() << " " << m_port << LOGEND();
		tryConnect();
	}
}

void ServerConnection::tick(const boost::system::error_code& e)
{
	//if(boost::asio::error::operation_aborted != e)
	//{
	//	if (CONN_STATUS_DISCONNECT == m_status)
	//	{
	//		tryConnect();
	//		m_tickTimer.expires_at(m_tickTimer.expires_at()+boost::posix_time::seconds(CONNECT_INTERVAL));
	//		m_tickTimer.async_wait(boost::bind(&ServerConnection::tick, this, boost::asio::placeholders::error));
	//	}
	//	else if (CONN_STATUS_CONNECTED == m_status)
	//	{
	//		//// send msg hear beat
	//		//OPSSHeartBeat msg;
	//		//sendMsg(&msg);

	//		//LOGNORMAL() << "send ss heartbeat " << m_ip << " " << m_port << LOGEND();
	//		m_tickTimer.expires_at(m_tickTimer.expires_at()+boost::posix_time::seconds(HEARTBEAT_INTERVAL));
	//		m_tickTimer.async_wait(boost::bind(&ServerConnection::tick, this, boost::asio::placeholders::error));
	//	}
	//	else
	//	{
	//		// just wait
	//	}
	//}
}

void ServerConnection::errHandler(const boost::system::error_code& e, Connection*, std::string dummy)
{
	LOGERROR() << "serverconnection error" 
				<<"ip"<<m_ip.c_str() 
				<<"port"<<m_port
				<<"dummy"<<dummy
				<<LOGEND();
	tryConnect();
}
// just trying to connect
void ServerConnection::tryConnect()
{
	this->clear();

	ip::tcp::endpoint point(ip::address_v4::from_string(m_ip.c_str()), m_port);
	m_sock.async_connect(point, boost::bind(&ServerConnection::connectHandler, this, boost::asio::placeholders::error));
	m_status = CONN_STATUS_CONNECTING;
	LOGNORMAL() << "trying to connect " << m_ip.c_str() << " " << m_port << LOGEND();
}
