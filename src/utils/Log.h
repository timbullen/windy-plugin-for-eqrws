/*
 * Log.h
 *
 *  Created on: 25/02/2022
 *      Author: Tim Bullen
 */

#ifndef LOG_H_
#define LOG_H_


#include <string>


typedef enum LogVerbosityLevel
{
    LOG_NONE        = 0,
    LOG_ERRORS      = 1,
    LOG_ACTIVITY    = 2,
    LOG_DETAILED    = 3,
    MAX_LOG_LEVEL
} LogVerbosityLevel_t;


class Log
{
public:
    /*
     * Sets the logging verbosity level of the system. All logs at a level equal to or
     * lower than the selected level will be logged.
     */
    static void setLogLevel(LogVerbosityLevel_t level);

    /*
     * Returns the logging level that the system has been set to.
     */
    static LogVerbosityLevel_t logLevel();

    /*
     * Adds a log message with the level LOG_ERROR
     */
    static void error(const std::string& msg);

    /*
     * Adds a log message with the level LOG_ACTIVITY
     */
    static void activity(const std::string& msg);

    /*
     * Adds a log message with the level LOG_DETAILED
     */
    static void detailed(const std::string& msg);

private:
    static void printSystemLog(const std::string& msg);
};

#endif /* LOG_H_ */
