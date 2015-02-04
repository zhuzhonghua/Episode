#include "StdAfx.h"

#include "Connection.h"
#include "MsgHandler.h"

Connection::Connection(boost::asio::io_service& io_service, int id)
	:m_sock(io_service),
	m_id(id)
{
	m_status		= CONN_STATUS_INIT;
	m_writeStatus	= SW_STATUS_INIT;

	m_errHandler	= NULL;

	this->clear();
}

void Connection::sendMsg(SendPacket* p)
{
	if (m_status != CONN_STATUS_CONNECTED)
	{
		LOGERROR() << "sendmsg status not connected"
			<<"id"<<m_id
			<<LOGEND();
		return;
	}
	short opCode = p->getOPCode();
	std::string body = p->getData();

	unsigned int pack = (13 << 24) + (body.size() & 0xFFFFFF);

	char header[MSG_HEADER_SIZE] = {0};
	memmove(header,					(char*)&opCode, sizeof(opCode));
	memmove(header+sizeof(opCode),	(char*)&pack,	sizeof(pack));

	std::stringstream ss;
	ss << std::string(header, 6) << body;
	m_outData.push_back(ss.str());
	LOGNET() << "try send msg " << opCode << " pack size " << body.size() << LOGEND();

	tryWrite();
}

void Connection::start()
{
	m_connSuccTime = Util::getMSTime();
	m_status = CONN_STATUS_CONNECTED;
	tryRead();
}

void Connection::clear()
{
	m_outData.clear();
	m_msgBuffer.clear();

	memset(m_buffer, 0, sizeof(m_buffer));
	m_offSet = 0;

	m_sock.close();
}

Connection::~Connection()
{
	clear();
}

// return false if not enough buf
bool Connection::getMsgSize(std::string* buf, int size)
{
	int tempSize = 0;
	std::list<std::string>::iterator iter = m_msgBuffer.begin();
	for (; iter != m_msgBuffer.end(); ++iter)
	{
		std::string &temp = *iter;
		if (temp.size() <= size - tempSize)
		{
			tempSize += temp.size();
			buf->append(temp.begin(), temp.end());
		}
		else
		{
			buf->append(temp, 0, (size - tempSize));
			tempSize = size;
			break;
		}
	}

	return tempSize >= size;
}

void Connection::skipMsgSize(int size)
{
	std::list<std::string>::iterator iter = m_msgBuffer.begin();
	while(size > 0 && iter != m_msgBuffer.end())
	{
		std::string &temp = *iter;
		if (temp.size() <= size)
		{
			size -= temp.size();
			iter = m_msgBuffer.erase(iter);
		}
		else
		{
			temp.erase(0, size);
			size = 0;
			break;
		}
	}
}


// trigger read event
void Connection::tryRead()
{
	// read
	m_sock.async_read_some
		(
		boost::asio::buffer(m_buffer+m_offSet, INBUF_MAX_SIZE-m_offSet),
		boost::bind
		(
		&Connection::handleRead,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
		)
		);
}

// trigger
void Connection::tryWrite()
{
	if (m_status != CONN_STATUS_CONNECTED)
	{
		LOGERROR() << "write status not connected "
					<<"id"<<m_id
					<<LOGEND();
		return;
	}
	if (m_outData.size() > 0 && m_writeStatus != SW_STATUS_WRITTING)
	{
		m_writeStatus = SW_STATUS_WRITTING;
		m_tempBuf = m_outData.front();
		m_sock.async_write_some
			(
			boost::asio::buffer(m_tempBuf, m_tempBuf.size()),
			boost::bind
			(
			&Connection::handleWrite,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
			)
			);
	}
}


// get some data
void Connection::handleRead(const boost::system::error_code& error, size_t len)
{
	if(!error)
	{
		LOGNET() << ">>> got " << len << " data from " 
				<<"id"<<m_id
				<<LOGEND();
		m_offSet += len;
		m_msgBuffer.push_back(std::string(m_buffer, m_buffer+m_offSet));
		m_offSet = 0;
		// continue to read
		tryRead();
		handlePacket();
	}
	else
	{
		if (m_status == CONN_STATUS_CONNECTED)
		{
			m_status = CONN_STATUS_DISCONNECT;
		}
		else
		{
			// triggered by other event
		}
		
		if (m_writeStatus != SW_STATUS_WRITTING)
		{
			m_errHandler(error, this, "handleread");
		}
		else
		{
			// write callback will call immediately
		}
	}
}

void Connection::handleWrite(const boost::system::error_code& error, size_t len)
{
	if(!error)
	{
		m_writeStatus = SW_STATUS_WRITEDONE;

		assert(m_outData.size() > 0);
		// ok write some chars
		//std::string& buf = m_outData.front();
		// first buffer all flushed
		if (len == m_tempBuf.length())
		{
			LOGNET() << "<<< write full " << len << " " 
				<<"id"<<m_id
				<< LOGEND();
			m_outData.pop_front();
			tryWrite();
		}
		else
		{
			//LOGNET() << "<<< write partial " << len << " should " << buf.length() << " " 
			LOGNET() << "<<< write partial " << len << " should " << m_tempBuf.length() << " " 
				<<"id"<<m_id
				<< LOGEND();
			assert(len < m_tempBuf.length());
			//buf = buf.substr(len);
			m_tempBuf = m_tempBuf.substr(len);
			tryWrite();
		}
	}
	else
	{
		if (m_status == CONN_STATUS_CONNECTED)
		{
			m_status = CONN_STATUS_DISCONNECT;
		}
		else
		{
			// triggered by other event
		}

		if (m_errHandler)
		{
			m_errHandler(error, this, "handlewrite");
		}
	}
}


void Connection::handlePacket()
{
	int count = 0;
	while(count++ < 20)
	{
		if(this->getBufSize() <= 0)
		{
			break;
		}
		std::string header;
		bool flag = this->getMsgSize(&header, MSG_HEADER_SIZE);
		if (!flag)
		{
			LOGERROR() << "not enough header" << LOGEND();
			break;
		}
		else
		{
			char* buf = &header[0];
			// check msg header
			int offset = 0;
			short msgID = 0;
			memcpy(&msgID, buf, sizeof(msgID));

			unsigned int pack = 0;
			memcpy(&pack, buf+sizeof(msgID), sizeof(pack));

			int msgSize = pack & 0xFFFFFF;
			unsigned int chaos	= pack >> 24;
			if (chaos != 13)
			{
				LOGERROR() << "wrong pack chaos " << chaos << " msg " << msgID << " from " 
							<<"id"<<m_id
							<< LOGEND();
				return;
			}
			// todo check valid msgid
			// body
			std::string body;
			flag = getMsgSize(&body, MSG_HEADER_SIZE+msgSize);
			if(flag)
			{
				this->skipMsgSize(msgSize+MSG_HEADER_SIZE);

				body.erase(0, MSG_HEADER_SIZE);
				LOGNET() << ">>> got msg " << msgID << " size " << msgSize << LOGEND();
				Packet p(msgID, body);

				MsgHandlerType handler = MsgHandler::getHandler(p.getOP());
				if (handler)
				{
					bool flag = handler(this, &p);
					if (!flag)
					{
						LOGERROR() << "return false handle " << p.getOP() << LOGEND();
					}
				}
				else
				{
					LOGERROR() << "didn't find handler for " << p.getOP() << LOGEND();
				}
			}
			else
			{
				LOGNET() << "not enough body need " << msgSize << 
					" but have " << m_offSet - MSG_HEADER_SIZE << 
					" msgID=" << msgID << LOGEND();
			}
		}
	}
}

void Connection::forceClose()
{
	m_sock.close();
	//if (m_writeStatus != SW_STATUS_WRITTING)
	//{		
	//	m_errHandler(boost::system::error_code(), this, "forceClose");
	//}
	//else
	//{
	//	// write callback will call immediately
	//}
}