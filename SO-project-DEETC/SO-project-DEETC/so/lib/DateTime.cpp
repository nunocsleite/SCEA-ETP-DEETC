#ifndef DATETIME_CPP
#define DATETIME_CPP

#include <string>
#include <stdio.h>
#include <time.h>
#include "DateTime.h"

// Using boost accumulators framework for computing the variance
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

// I'm importing the accumulators namespace as "a" to make clear which
// elements belong to it
namespace a = boost::accumulators;

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


#endif // DATETIME_CPP
