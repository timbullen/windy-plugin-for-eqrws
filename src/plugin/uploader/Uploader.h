/*
 * Uploader.h
 *
 *  Created on: 31/03/2023
 *      Author: Tim Bullen
 */

#ifndef UPLOADER_H_
#define UPLOADER_H_

#include "StationConfiguration.h"

class Uploader {
public:
    Uploader(const StationConfiguration& stationConfig);

    /*
     * Checks that the curl library is available on the system, as this is required to perform the upload.
     *
     * If curl is not present, an exception is thrown.
     */
    static void checkCurl();

    /*
     * Uploads the data to the Windy servers using the HTTPS GET request API.
     */
    void performUpload(time_t timestamp,
                        float temperature,
                        float wind_speed,
                        float wind_dir,
                        float wind_gust,
                        float pressure,
                        float humidity) const;

private:
    const StationConfiguration& stationConfig;
};

#endif /* UPLOADER_H_ */
