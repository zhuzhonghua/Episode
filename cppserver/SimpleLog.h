#ifndef __FIGHTSERVER_SIMPLE_LOG_H__
#define __FIGHTSERVER_SIMPLE_LOG_H__

#include <iostream>
#include <string>
#include <sstream>
#include <list>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "BaseDefine.h"

class SimpleLog
{
public:
	template<class T>
	SimpleLog& operator << (T &s)
	{
		this->log<T>(s);
		return *this;
	}
	SimpleLog& operator << (const char* s)
	{
		if(s)
		{
			this->log<const char*>(s);
		}		
		return *this;
	}
	SimpleLog& operator << (const int64 s)
	{
		this->log<const int64>(s);
		return *this;
	}
	/*SimpleLog& operator << (const int s)
	{
		this->log<const int>(s);
		return *this;
	}	
	SimpleLog& operator << (const unsigned int s)
	{
		this->log<const unsigned int>(s);
		return *this;
	}
	SimpleLog& operator << (const short s)
	{
		this->log<const short>(s);
		return *this;
	}	
	SimpleLog& operator << (const unsigned short s)
	{
		this->log<const unsigned short>(s);
		return *this;
	}
	SimpleLog& operator << (const double s)
	{
		this->log<const double>(s);
		return *this;
	}*/

public:
				void		getLogContent(std::stringstream* ss);
	virtual		bool		flushLog();
	virtual		bool		init();
				SimpleLog&	readyToLog();
protected:
	virtual		void		afterLog(const std::string &str)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_bufSS.push_back(str);
	}
	template<class T>
	void log(T & inst)
	{
		std::stringstream ssBuf;
		ssBuf <<" "<< inst;

		if(m_bDisplay)
			std::cout << ssBuf.str();

		afterLog(ssBuf.str());
	}
	std::string getFileName();
	SimpleLog(const char* fileName);
protected:
	std::list<std::string>	m_bufSS;
	std::string				m_fileName;

	boost::mutex			m_mutex;
	bool					m_bDisplay;
};

class SimpleLogTest: public SimpleLog
{
public:
	static SimpleLogTest* get();
private:
	SimpleLogTest();
	static SimpleLogTest*	m_instance;
};

class SimpleLogNormal: public SimpleLog
{
public:
	static SimpleLogNormal* get();
private:
	SimpleLogNormal();
	static SimpleLogNormal*	m_instance;
};

class SimpleLogNet: public SimpleLog
{
public:
	static SimpleLogNet* get();
private:
	SimpleLogNet();
	static SimpleLogNet*	m_instance;
};

class SimpleLogError: public SimpleLog
{
public:
	static SimpleLogError*	get();
private:
	SimpleLogError();
	static SimpleLogError*	m_instance;
};

class SlowLog: public SimpleLog
{
public:
	static SlowLog* get();
protected:
	void afterLog(const std::string &str);
private:
	SlowLog();
	static SlowLog*	m_instance;
};

class LogThread
{
public:
	bool init();
	void run();
	void start();
	void exit();

	static LogThread* get();
private:
	LogThread();
private:
	boost::thread*		m_thread;

	THREAD_STATUS		m_status;

	static LogThread*	s_inst;
};
#endif