#ifndef __FIGHTSERVER_UTIL_H__
#define __FIGHTSERVER_UTIL_H__

#include <boost/date_time/gregorian/gregorian.hpp>

#include "BaseDefine.h"

typedef boost::posix_time::ptime PTime;

class Util
{
public:
	static int		getUnixTime();
	static int64	getUnixMsTime();
	static PTime	getPTime();
	// return milisec p1-p2
	static int64	getDuration(PTime p1, PTime p2);
	static float	getDurationMicro(PTime p1, PTime p2);
	static double	getMSTime();

	static double	getRand(double start, double end);
	static int		getRand(int start, int end);
};
#endif