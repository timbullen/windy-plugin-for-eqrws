/*
 * Datalogger.h
 *
 *  Created on: 31/03/2023
 *      Author: Tim Bullen
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#include <vector>
#include <mutex>

#include "PluginDefs.h"
#include "StationConfiguration.h"


class Datalogger {
public:
    Datalogger(const StationConfiguration& stationConfig);

    typedef std::vector<Sample_t> SampleBuffer_t;

    /**
     * Adds a received sample to the buffers.
     *
     * @param channel_index     The zero-indexed channel number that the sample belongs to
     * @param sample            The timestamped data sample
     */
    void addSample(const uint8_t channel_index, const Sample_t& sample);

    /**
     * Polls the system clock to see if its time to perform a data upload to the
     * Windy servers. If it is time, then it gathers all the samples from the data window
     * and averages them, before uploading the results to the servers. This should be called
     * periodically in the main loop.
     */
    void pollDatalogger();

private:
    const StationConfiguration& stationConfig;
    std::array<SampleBuffer_t, EQRWS_CHANNELS::NUMBER_OF_CHANNELS> sampleBuffers;
    time_t nextUploadTime;
    std::mutex dataMutex;

    bool hasUploadPeriodCompleted() const;
    void sendDataUpload();

    static float caluclateAverage(const SampleBuffer_t& samples);
    static float caluclatePeakValue(const SampleBuffer_t& samples);
};

#endif /* DATALOGGER_H_ */
