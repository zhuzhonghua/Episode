#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>

using namespace boost::gregorian;
namespace fs = boost::filesystem;

#include "StdAfx.h"
#include "SimpleLog.h"
#include <fstream>

SimpleLogNormal* SimpleLogNormal::m_instance = NULL;
SimpleLogNet* SimpleLogNet::m_instance = NULL;
SimpleLogTest* SimpleLogTest::m_instance = NULL;
SimpleLogError*	SimpleLogError::m_instance = NULL;
SlowLog* SlowLog::m_instance = NULL;


SimpleLog::SimpleLog(const char* fileName)
	:m_fileName(fileName),
	m_bDisplay(true)
{

}

// log/logname-2014-7-23.log
std::string SimpleLog::getFileName()
{
	date theDay = day_clock::local_day();

	std::stringstream fileName;
	fileName << "log/" << m_fileName 
		<< "-" 
		<< theDay.year() 
		<< "-" 
		<< theDay.month().as_number() 
		<< "-"
		<< theDay.day() 
		<< ".log";

	return fileName.str();
}

void SimpleLog::getLogContent(std::stringstream* ss)
{
	boost::mutex::scoped_lock lock(m_mutex);

	int count = 0;
	while(count++ < 10 && m_bufSS.size() > 0)
	{
		*ss << m_bufSS.front();
		m_bufSS.pop_front();			
	}	
}

bool SimpleLog::flushLog()
{	
	std::stringstream ss;
	// no item , just return
	if(m_bufSS.size() <= 0)
	{
		return false;
	}
	else
	{
		getLogContent(&ss);
	}
	
	
	FILE* p = fopen(getFileName().c_str(), "a");
	fwrite(ss.str().c_str(), ss.str().length(), 1, p);
	fclose(p);

	return true;
}

// check directory log
// check if can append file
bool SimpleLog::init()
{
	if (!fs::exists("log"))
	{
		fs::create_directory("log");
	}
	if (!fs::exists("log"))
	{
		SLOWLOG() << "can't create log directory!!!!!!!" << LOGEND();
		return false;
	}

	std::ofstream ofs(getFileName().c_str(), std::ofstream::app);	
	if (!ofs.is_open())
	{
		SLOWLOG() << "can't append " << getFileName().c_str() << "!!!!!!!" << LOGEND();
		return false;
	}
	
	return true;
}

SimpleLog& SimpleLog::readyToLog()
{
	std::stringstream ssBuf;
	boost::posix_time::ptime p = boost::posix_time::microsec_clock::local_time();
	ssBuf << "(" << p.time_of_day() << ")    ";
	*this << ssBuf.str().c_str();
	return *this;
}

SimpleLogNormal::SimpleLogNormal()
	:SimpleLog("normal")
{

}

SimpleLogNormal* SimpleLogNormal::get()
{
	if(SimpleLogNormal::m_instance)
	{
		return SimpleLogNormal::m_instance;	
	}
	else
	{
		return SimpleLogNormal::m_instance = new SimpleLogNormal();
	}
}

SimpleLogTest::SimpleLogTest()
	:SimpleLog("test")
{
	//m_bDisplay = false;
}

SimpleLogTest* SimpleLogTest::get()
{
	if(SimpleLogTest::m_instance)
	{
		return SimpleLogTest::m_instance;	
	}
	else
	{
		return SimpleLogTest::m_instance = new SimpleLogTest();
	}
}

SimpleLogNet::SimpleLogNet()
	:SimpleLog("net")
{
	m_bDisplay = false;
}

SimpleLogNet* SimpleLogNet::get()
{
	if(SimpleLogNet::m_instance)
	{
		return SimpleLogNet::m_instance;	
	}
	else
	{
		return SimpleLogNet::m_instance = new SimpleLogNet();
	}
}

SimpleLogError::SimpleLogError()
	:SimpleLog("error")
{

}

SimpleLogError* SimpleLogError::get()
{
	if(SimpleLogError::m_instance)
	{
		return SimpleLogError::m_instance;	
	}
	else
	{
		return SimpleLogError::m_instance = new SimpleLogError();
	}
}

SlowLog::SlowLog()
	:SimpleLog("slow-log")
{

}

SlowLog* SlowLog::get()
{
	if(SlowLog::m_instance)
	{
		return SlowLog::m_instance;	
	}
	else
	{
		return SlowLog::m_instance = new SlowLog();
	}
}

// flush
void SlowLog::afterLog(const std::string &str)
{
	std::ofstream ofs(getFileName().c_str(), std::ofstream::app);
	ofs << str;
}

LogThread* LogThread::s_inst = NULL;

LogThread::LogThread()
{
	m_status = THREAD_STATUS_INIT;
}

bool LogThread::init()
{
	if (!LOGNORMAL_P()->init())
	{
		return false;
	}
	SLOWLOG() << "lognormal init OK!" << LOGEND();
	if (!LOGERROR_P()->init())
	{
		return false;
	}
	SLOWLOG() << "logerror init OK!" << LOGEND();

	start();
	return true;
}

void LogThread::start()
{
	if(m_status == THREAD_STATUS_RUNNING)
	{
		return;
	}

	m_thread = new boost::thread(boost::bind(&LogThread::run, this));
	m_status = THREAD_STATUS_RUNNING;

	SLOWLOG() << "logthread init OK!" << LOGEND();
}

void LogThread::run()
{
	while(m_status != THREAD_STATUS_EXIT)
	{
		std::string bufNormal, bufError;
		bool flagNormal = LOGNORMAL_P()->flushLog();
		bool flagError = LOGERROR_P()->flushLog();
		bool flagNet = LOGNET_P()->flushLog();
		bool flagTest = LOGTEST_P()->flushLog();
		if(!flagNormal && !flagError && !flagNet && !flagTest)
		{
			boost::this_thread::sleep_for(boost::chrono::seconds(1));
			continue;
		}
	}
}

LogThread* LogThread::get()
{
	if(LogThread::s_inst)
	{
		return LogThread::s_inst;
	}
	else
	{
		return LogThread::s_inst = new LogThread();
	}
}

void LogThread::exit()
{
	m_status = THREAD_STATUS_EXIT;
}