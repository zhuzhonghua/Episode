#ifndef __FIGHTSERVER_STDAFX_H__
#define __FIGHTSERVER_STDAFX_H__

#include "SimpleLog.h"
#include "Util.h"

#define LOGNORMAL()		((SimpleLogNormal::get()->readyToLog()))
#define LOGTEST()		((SimpleLogTest::get()->readyToLog()))
#define LOGNET()		((SimpleLogNet::get()->readyToLog()))
#define LOGERROR()		((SimpleLogError::get()->readyToLog()))
#define SLOWLOG()		((SlowLog::get()->readyToLog()))
#define LOGEND()		("\n\n");

#define LOGNORMAL_P()	SimpleLogNormal::get()
#define LOGERROR_P()	SimpleLogError::get()
#define LOGNET_P()		SimpleLogNet::get()
#define LOGTEST_P()		SimpleLogTest::get()

#define LOGTHREAD_P()	LogThread::get()

#define ASSERT(a)	do	\
{						\
	if (!(a))			\
	{					\
		*(int*)0=1;		\
	}					\
} while (false); 

#endif
