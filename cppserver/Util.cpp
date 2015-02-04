#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

#include "Util.h"

int Util::getUnixTime()
{
	return time(NULL);		
}

int64 Util::getUnixMsTime()
{
	return Util::getPTime().time_of_day().total_milliseconds();
}

PTime Util::getPTime()
{
	return boost::posix_time::microsec_clock::local_time();
}

int64 Util::getDuration(PTime p1, PTime p2)
{
	boost::posix_time::time_duration p = p1-p2;
	return p.total_milliseconds();
}

float Util::getDurationMicro(PTime p1, PTime p2)
{
	boost::posix_time::time_duration p = p1-p2;
	return p.total_microseconds()/(float)1000000;
}

std::string ts("2000-01-01 00:00:00.001");
PTime begin(boost::posix_time::time_from_string(ts));

double Util::getMSTime()
{
	PTime now = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration p = now-begin;
	return p.total_microseconds()/1000000.0;
}

boost::minstd_rand fRnd(time(0));
double Util::getRand(double start, double end)
{	
	boost::uniform_real<> uni_dist(start,end);
	boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > uni(fRnd, uni_dist);
	return uni();
}

boost::minstd_rand intRnd(time(0));
int Util::getRand(int start, int end)
{
	if (start == end)
	{
		return start;
	}
	int random_variable = std::rand();
	return random_variable % (end-start+1) + start;
}