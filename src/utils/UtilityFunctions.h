/*
 * UtilityFunctions.h
 *
 *  Created on: 3/04/2023
 *      Author: Tim Bullen
 */

#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include "PluginDefs.h"

class UtilityFunctions {
public:

    /*
     * Formats the timestamp into the format:
     *
     * YYYY-MM-DD HH:MM:ss
     */
    static std::string formatTimestampToStr(const std::time_t& timestamp)
    {
        int buf_len = 50;
        char buffer[buf_len] = "";

        tm utc_datetime;
        gmtime_r(&timestamp, &utc_datetime);

        snprintf(buffer, buf_len, "%04d/%02d/%02d %02d:%02d:%02d",
                utc_datetime.tm_year + 1900,    // years are years since 1900
                utc_datetime.tm_mon + 1,        // months are zero-indexed
                utc_datetime.tm_mday,
                utc_datetime.tm_hour,
                utc_datetime.tm_min,
                utc_datetime.tm_sec);

        return std::string(buffer);
    }
};

#endif /* UTILITYFUNCTIONS_H_ */
