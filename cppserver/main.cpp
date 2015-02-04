#include <iostream>
#include <list>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/thread/thread.hpp>

#include "StdAfx.h"
#include "App.h"

int main()
{
	std::srand(std::time(0)); // use current time as seed for random generator

	//getchar();
	if (!App::inst()->init())
	{
		std::cout << "app init error!!!!!!!!!!!!!!!!!!!" << std::endl;
		return 1;
	}

	App::inst()->loop();
	App::inst()->exit();
	
	return 0;
}