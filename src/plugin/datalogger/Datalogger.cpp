/*
 * Datalogger.cpp
 *
 *  Created on: 31/03/2023
 *      Author: Tim Bullen
 */

#include <ctime>

#include "Datalogger.h"
#include "Log.h"
#include "Uploader.h"


// How long to wait past the completion of the data window to allow data to be filtered through from the SeedLink server.
const uint32_t UPLOAD_PERIOD_BUFFER_s =                 20;


Datalogger::Datalogger(const StationConfiguration& stationConfig)
    : stationConfig(stationConfig)
{
    // Validate the existence of curl before continuing
    Uploader::checkCurl();

    // Set the first upload time to be the next even interval of the upload period
    const uint32_t upload_period_s = stationConfig.getConfig().uploadPeriod_s;
    const std::time_t now = std::time(nullptr);
    nextUploadTime = ((now / upload_period_s) * upload_period_s) + upload_period_s;

    Log::detailed("Set first data upload time to: " + std::to_string(nextUploadTime));
}


void Datalogger::addSample(const uint8_t channel_index, const Sample_t& sample)
{
    // This is called from the seedlink stream thread, so lock the mutex
    std::lock_guard lock(dataMutex);

    if (channel_index >= sampleBuffers.size()) {
        return;
    }

    // Append the new sample to the channel's sample buffer
    sampleBuffers.at(channel_index).push_back(sample);
}


void Datalogger::pollDatalogger()
{
    // Check for a completed upload period.
    if (hasUploadPeriodCompleted()) {
        Log::detailed("Data upload period completed.");

        std::lock_guard lock(dataMutex);

        sendDataUpload();

        // Shift the upload time
        nextUploadTime += stationConfig.getConfig().uploadPeriod_s;
    }
}


/*
 * Private Methods
 */

bool Datalogger::hasUploadPeriodCompleted() const
{
    const std::time_t now = std::time(nullptr);

    // Add a buffer period to allow the data from the end of the window to be filtered through
    return (now >= (nextUploadTime + UPLOAD_PERIOD_BUFFER_s));
}


void Datalogger::sendDataUpload()
{
    /*
     * Performs some basic processing on the data within the data upload window,
     * and then uploads the data to the Windy servers.
     */

    DataArray_t channel_averages;
    float wind_gust = 0.0;

    for (uint8_t channel_index = 0; channel_index < sampleBuffers.size(); channel_index++)
    {
        SampleBuffer_t& buffer = sampleBuffers.at(channel_index);

        // Transfer the samples over the period window to a new buffer
        std::vector<Sample_t> samples;

        // Iterate backwards so we don't screw up the indexing
        for (int32_t i = buffer.size() - 1; i >= 0; i--) {
            const Sample_t& sample = buffer.at(i);

            if (sample.timestamp <= nextUploadTime) {
                if (sample.timestamp >= (nextUploadTime - stationConfig.getConfig().uploadPeriod_s)) {
                    samples.push_back(sample);
                }
                else {
                    Log::error("Rejecting sample with timestamp " + std::to_string(sample.timestamp) + " outside data upload window.");
                }

                buffer.erase(buffer.begin() + i);
            }
        }

        if (samples.size() == 0) {
            Log::error("No samples found for data upload window");
            return;
        }

        // Calculate the average value for the channel over the window
        channel_averages.at(channel_index) = caluclateAverage(samples);

        // If this is the wind speed channel, also calculate the wind gust
        if (channel_index == EQRWS_CHANNELS::WIND_SPEED) {
            wind_gust = caluclatePeakValue(samples);
        }
    }

    // Upload the data
    Uploader uploader (stationConfig);
    uploader.performUpload(nextUploadTime,
            channel_averages.at(EQRWS_CHANNELS::TEMP_C),
            channel_averages.at(EQRWS_CHANNELS::WIND_SPEED),
            channel_averages.at(EQRWS_CHANNELS::WIND_DIR),
            wind_gust,
            channel_averages.at(EQRWS_CHANNELS::PRESSURE),
            channel_averages.at(EQRWS_CHANNELS::HUMIDITY));
}


float Datalogger::caluclateAverage(const std::vector<Sample_t>& samples)
{
    /*
     * Returns a vector of the average of each sample in the provided buffer.
     */

    float ave = 0.0;

    if (samples.size() == 0) {
        return ave;
    }

    for (const Sample_t& sample : samples) {
        ave += sample.data;
    }

    ave /= samples.size();
    return ave;
}


float Datalogger::caluclatePeakValue(const SampleBuffer_t& samples)
{
    /*
     * Returns the peak absolute value of all the samples in the provided buffer.
     */

    float max_val = 0.0;

    for (const Sample_t& sample : samples) {
        max_val = std::max(max_val, std::abs(sample.data));
    }

    return max_val;
}

