/*
 * SeedLinkStream.cpp
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#include <iomanip>    // time string formatting
#include <sstream>

#include "SeedLinkStream.h"
#include "Log.h"
#include "UtilityFunctions.h"


const bool DEBUG_OUTPUT = 						false;
const uint32_t SEEDLINK_PORT = 					18000;      // Always 18000, not configurable on the EQRWS instruments


SeedLinkStream::SeedLinkStream(const StationConfiguration& stationConfig, Datalogger& datalogger)
	: stationConfig(stationConfig),
	  datalogger(datalogger)
{

}


void SeedLinkStream::connect()
{
    if (listenThread) {
        throw std::runtime_error("SeedLink stream is already running.");
    }

    openStream();

    listenThread = std::unique_ptr<std::thread>(new std::thread([this](){collectData();}));
}


void SeedLinkStream::close()
{
    if (listenThread) {
        // Tell thread to exit and join thread
        exitThread = true;

        listenThread->join();
    }

    // Free memory
	closeStream();
}


/*
 * Private Methods
 */

void SeedLinkStream::openStream()
{
	/*
	 * Creates the objects needed to read from the SeedLink stream using the libslink library.
	 */

    const Configuration_t& config = stationConfig.getConfig();

    if (DEBUG_OUTPUT) {
        sl_loginit (2, NULL, NULL, NULL, NULL);     // 2 =  highest verbosity level
    }

    // Allocate and initialize a new connection description
    this->sl_conn = sl_newslcd();

    // Set the connection address
    sprintf(seedlink_addr, "%s:%i", stationConfig.getConfig().IPAddress.c_str(), SEEDLINK_PORT);
    sl_conn->sladdr = seedlink_addr;

    // Start from the current system time
    tm start_datetime;
    std::time_t now = std::time(NULL);	// Returns system time secondsSinceEpoch
    gmtime_r(&now, &start_datetime);	// Creates broken-format in UTC

    strftime(begin_time, 30, "%Y,%m,%d,%H,%M,%S", &start_datetime);
    sl_conn->begin_time = begin_time;

    // Leave end time undefined
    tm end_datetime;
    std::time_t then = now + 10;
    gmtime_r(&then, &end_datetime);	// Creates broken-format in UTC
    strftime(end_time, 30, "%Y,%m,%d,%H,%M,%S", &end_datetime);
    sl_conn->end_time = nullptr;

    sl_conn->netto = 60;         // timeout in seconds if we haven't received anything
    sl_conn->netdly = 10;        // reconnection timer duration

    // Configure the desired streams to extract. - All channel streams from the sensor
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << config.seedlink.location;		// Format stream location with leading zeroes
    std::string stream_loc = ss.str();

    std::string stream_selectors = "";
    for (uint8_t channel_idx = 0; channel_idx < config.seedlink.subsources.size(); channel_idx++)
    {
        stream_selectors += stream_loc
                + config.seedlink.band
                + config.seedlink.source
                + config.seedlink.subsources[channel_idx] + ".D ";
    }

    if (sl_addstream(sl_conn,
                     config.seedlink.networkID.c_str(),
                     config.seedlink.stationID.c_str(),
                     stream_selectors.c_str(), -1, 0) != 0)
    {
        throw std::runtime_error("Error adding streams to SeedLink stream connection request");
    }


    Log::activity("Requesting SeedLink stream starting from " + UtilityFunctions::formatTimestampToStr(now) + " UTC");

    Log::activity("Initialised SeedLink stream connection to " + std::string(sl_conn->sladdr));
}


void SeedLinkStream::collectData()
{
    Log::detailed("Listening for packets from SeedLink server...");

    // Loop with the connection manager
    SLpacket* slpack;

    while (!exitThread && sl_collect(sl_conn, &slpack)) {
        int ptype  = sl_packettype(slpack);
        int seqnum = sl_sequence(slpack);

        packetHandler((char*)&slpack->msrecord, ptype, seqnum, SLRECSIZE);
    }

    Log::detailed("Finished collecting packets.");
}


void SeedLinkStream::closeStream()
{
	/*
	 * Closes the stream, resets the parameters and releases memory
	 */

    // Make sure everything is shut down and save the state file
    if (sl_conn->link != -1) {
        sl_disconnect (sl_conn);
    }

    // Free the connection description memory
    sl_conn->sladdr = NULL;
    sl_conn->begin_time = NULL;
    sl_conn->end_time = NULL;
    sl_freeslcd(sl_conn);

    Log::activity("Closed the SeedLink stream connection.");
}


void SeedLinkStream::packetHandler(char *msrecord, int packet_type, int seqnum, int packet_size)
{
    /*
     * Handles packets received from the SeedLink server and performs actions depending
     * on what type of packet they are.
     * Data packets are unpacked, and then forwarded on.
     */

    // The following is dependent on the packet type values in libslink.h
    std::vector<std::string> type = { "Data", "Detection", "Calibration", "Timing",
                                        "Message", "General", "Request", "Info",
                                        "Info (terminated)", "KeepAlive" };

    // Build a current local time string
    double dtime   = sl_dtime ();                               // Epoch time
    double secfrac = (double) ((double)dtime - (int)dtime);     // Fractional part of epoch time
    time_t itime   = (time_t) dtime;                            // Integer part of epoch time
    tm timep;
    gmtime_r(&itime, &timep);
    char timestamp[20];

    snprintf (timestamp, 20, "%04d:%02d:%02d %02d:%02d:%02d.%01.0f",
                timep.tm_year + 1900, timep.tm_mon + 1, timep.tm_mday,
                timep.tm_hour, timep.tm_min, timep.tm_sec, secfrac);

    // Process waveform data
    if (packet_type == SLDATA)
    {
        sl_log (0, 1, "%s, seq %d, Received %s blockette:\n",
                timestamp, seqnum, type.at(packet_type).c_str());

        // Unpack the received data record
        const int parse_blockettes = 1;
        const int unpack_data = 1;
        sl_msr_parse(sl_conn->log, msrecord, &msr, parse_blockettes, unpack_data);

        if (DEBUG_OUTPUT) {
            sl_msr_print (sl_conn->log, msr, 1);
        }

        Log::detailed("Data packet received from SeedLink server with timestamp: " + std::string(timestamp));
        processDataPacket(msr, itime);
    }
    else if (packet_type == SLKEEP)
    {
        sl_log (0, 2, "Keep alive packet received\n");
    }
    else
    {
        sl_log (0, 1, "%s, seq %d, Received %s blockette\n",
                timestamp, seqnum, type.at(packet_type).c_str());
    }
}


void SeedLinkStream::processDataPacket(SLMSrecord * const msr, const time_t start_time)
{
    /*
     * Unpacks the samples from the data packet and then forwards them on to the datalogger
     * class.
     */

    const Configuration_t& config = stationConfig.getConfig();

    // Extract some relevant metadata out of the miniseed record
    const char channel_subsource = msr->fsdh.channel[2];
    const uint32_t sample_rate = msr->fsdh.samprate_fact * msr->fsdh.samprate_mult;
    const uint32_t period_ms = 1000 / sample_rate;

    if (msr->numsamples < 0) {
        Log::error("Received MiniSEED record contains no samples");
        return;
    }

    for (int32_t i = 0; i < msr->numsamples; i++) {
        int32_t data_int = msr->datasamples[i];
        float data_float = static_cast<float>(data_int) / (1e6 / config.seedlink.resolution_microunits);

        // Add a timestamp (will be truncated to nearest second but that doesn't matter)
        const time_t timestamp = start_time + ((i * period_ms) / 1000);

        // Combine timestamp and data value into a sample object
        const Sample sample (timestamp, data_float);

        // Match the channel SID string to the channel index
        for (uint8_t i = 0; i < config.seedlink.subsources.size(); i++) {
            // TODO: use a std::map here instead
            if (config.seedlink.subsources.at(i).c_str()[0] == channel_subsource) {
                datalogger.addSample(i, sample);
            }
        }
    }
}

