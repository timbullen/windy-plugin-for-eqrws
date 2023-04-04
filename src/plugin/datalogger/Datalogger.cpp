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
#include "UtilityFunctions.h"


// How long to wait past the completion of the data window to allow data to be filtered through from the SeedLink server.
const uint32_t UPLOAD_PERIOD_BUFFER_s =                 20;

// The sample rate of the EQRWS - Always 1 Hz, not configurable
const uint32_t EQRWS_SAMPLE_RATE_Hz =                   1;

const uint32_t EQRWS_SAMPLES_PER_HOUR =                 (60 * 60 * EQRWS_SAMPLE_RATE_Hz);

// The amount of time to store in the rain accumulation buffer, must be at least an hour in order to provide hourly accumulation stats
const uint32_t NUM_RAIN_ACC_BUFFER_SAMPLES =            EQRWS_SAMPLES_PER_HOUR + (300 * EQRWS_SAMPLE_RATE_Hz);


Datalogger::Datalogger(const StationConfiguration& stationConfig)
    : stationConfig(stationConfig),
      rainAccumulationBuffer(NUM_RAIN_ACC_BUFFER_SAMPLES)
{
    if (NUM_RAIN_ACC_BUFFER_SAMPLES < (EQRWS_SAMPLES_PER_HOUR + (UPLOAD_PERIOD_BUFFER_s * EQRWS_SAMPLE_RATE_Hz))) {
        throw std::runtime_error("Rain accumulation buffer does not contain enough samples to cover an hour.");
    }

    // Validate the existence of curl before continuing
    Uploader::checkCurl();

    // Set the first upload time to be the next even interval of the upload period
    const uint32_t upload_period_s = stationConfig.getConfig().uploadPeriod_s;
    const std::time_t now = std::time(nullptr);
    nextUploadTime = ((now / upload_period_s) * upload_period_s) + upload_period_s;

    Log::detailed("Set first data upload time to: " + UtilityFunctions::formatTimestampToStr(nextUploadTime));
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

    // Update the rain accumulation buffer
    if (channel_index == EQRWS_CHANNELS::RAIN_ACC) {
        updateRainAccumulation(sample);
    }

    // Check for warnings
    if (channel_index == EQRWS_CHANNELS::PRESSURE) {
        if (sample.data < 800.0 || sample.data > 1200.0) {
            Log::error("Warning! Received abnormal atmospheric pressure value of " + std::to_string(sample.data)
                    + ". Are you sure you have configured the correct SeedLink resolution?");
        }
    }
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
    return (now >= static_cast<time_t>(nextUploadTime + UPLOAD_PERIOD_BUFFER_s));
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
                if (sample.timestamp >= static_cast<time_t>(nextUploadTime - stationConfig.getConfig().uploadPeriod_s)) {
                    samples.push_back(sample);
                }
                else {
                    Log::error("Rejecting sample with timestamp " + UtilityFunctions::formatTimestampToStr(sample.timestamp) + " outside data upload window.");
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

    // Get the rain accumulation for the past hour
    const std::time_t an_hour_ago = nextUploadTime - (60 * 60);
    float rain_mm = getTotalRainAccumulation(an_hour_ago, nextUploadTime);

    // Upload the data
    Uploader uploader (stationConfig);
    uploader.performUpload(nextUploadTime,
            channel_averages.at(EQRWS_CHANNELS::TEMP_C),
            channel_averages.at(EQRWS_CHANNELS::WIND_SPEED),
            channel_averages.at(EQRWS_CHANNELS::WIND_DIR),
            wind_gust,
            channel_averages.at(EQRWS_CHANNELS::PRESSURE),
            channel_averages.at(EQRWS_CHANNELS::HUMIDITY),
            rain_mm);
}


void Datalogger::updateRainAccumulation(const Sample_t& sample)
{
    /*
     * Here we do some work to convert the rain data from accumulated rain since midnight UTC
     * to total precipitation in the past hour.
     */

    // Here we calculate a diff of this sample from the last, i.e the change in rain accumulation
    // over the past sample.
    if (rainAccumulationBuffer.size() == 0) {
        // First sample, save as reference for next time and exit
        prevRainSample = sample;
        rainAccumulationBuffer.add(Sample(sample.timestamp, 0.0));
        return;
    }

    if (sample.data < prevRainSample.data) {
        // The rain counter is reset to zero every 24H, ignore the sample when this happens
    }
    else {
        // Calculate the increase in rain since the previous sample
        Sample_t diff (sample.timestamp, sample.data - prevRainSample.data);

        // Save the result in the buffer
        rainAccumulationBuffer.add(diff);
    }

    prevRainSample = sample;
}


float Datalogger::getTotalRainAccumulation(const std::time_t start, const std::time_t end) const
{
    /*
     * Returns the total rain accumulation from all buffered samples that are within the
     * start and end boundaries.
     */

    // Tally up all the rain accumulation samples for the time window
    float total = 0.0;
    for (uint32_t i = 0; i < rainAccumulationBuffer.size(); i++) {
        const Sample_t& diff = rainAccumulationBuffer.at(i);

        // Only inclusive of end time boundary since sample is rain since the previous sample,
        if (diff.timestamp > start && diff.timestamp <= end) {
            total += diff.data;
        }
    }

    return total;
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

