#ifndef __FIGHTSERVER_CONNECTION_H__
#define __FIGHTSERVER_CONNECTION_H__

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include "Packet.h"
using namespace boost::asio;
using boost::asio::ip::tcp;

class Connection;
typedef boost::function<void (const boost::system::error_code& error, Connection*, std::string dummy) > ErrHandler;

class Connection
{
public:
	Connection(boost::asio::io_service& io_service, int id=-1);
	~Connection();
	tcp::socket&	socket()	{ return m_sock; }
	int				id()		{ return m_id; }

	void			start();
	void			clear();
	virtual void sendMsg(SendPacket* p);
	virtual void handleRead(const boost::system::error_code& error, size_t len);
	virtual void handleWrite(const boost::system::error_code& error, size_t len);
	virtual void handlePacket();
	//
	virtual void tryRead();
	virtual void tryWrite();
	//
	int  getBufSize() { return m_msgBuffer.size(); }
	bool getMsgSize(std::string* buf, int size);
	void skipMsgSize(int size);
	//
	void setErrHandler(ErrHandler handler) { m_errHandler = handler;}
	//
	double				getConnSuccTime() { return m_connSuccTime; }
	CONNECTION_STATUS	getStatus() { return m_status; }
	void				forceClose();
protected:
	CONNECTION_STATUS	m_status;	
	double				m_connSuccTime;
	ip::tcp::socket				m_sock;
	int							m_id;

	std::list<std::string>	m_msgBuffer;
	char				m_buffer[INBUF_MAX_SIZE];	// temporary
	int					m_offSet;

	std::string					m_tempBuf;
	std::list<std::string>		m_outData;
	SOCKET_WRITE_STATUS			m_writeStatus;

	ErrHandler					m_errHandler;
};
#endif