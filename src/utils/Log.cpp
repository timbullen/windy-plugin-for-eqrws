/*
 * Log.cpp
 *
 *  Created on: 25/02/2022
 *      Author: Tim Bullen
 */

#include <fstream>
#include <iostream>

#include "Log.h"


#ifdef UNIT_TEST
LogVerbosityLevel_t logging_level = LOG_ACTIVITY;
#else
// Default to no logging
LogVerbosityLevel_t logging_level = LOG_NONE;
#endif


void Log::setLogLevel(LogVerbosityLevel_t level)
{
    logging_level = level;
}


LogVerbosityLevel_t Log::logLevel()
{
    return logging_level;
}


void Log::error(const std::string& msg)
{
    if (logging_level >= LOG_ERRORS) {
        printSystemLog(msg);
    }
}


void Log::activity(const std::string& msg)
{
    if (logging_level >= LOG_ACTIVITY) {
        printSystemLog(msg);
    }
}


void Log::detailed(const std::string& msg)
{
    if (logging_level >= LOG_DETAILED) {
        printSystemLog(msg);
    }
}


/**
 * Private Methods
 */

void Log::printSystemLog(const std::string& msg)
{
    std::cout << msg << std::endl;
}
