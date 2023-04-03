/*
 * SeedLinkStream.h
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#ifndef SEEDLINKSTREAM_H_
#define SEEDLINKSTREAM_H_

#include <seedlink/libs/3rd-party/libslink/libslink.h>      // The client library interface
#include <thread>
#include <atomic>

#include "StationConfiguration.h"
#include "Datalogger.h"

class SeedLinkStream {
public:
	SeedLinkStream(const StationConfiguration& stationConfig, Datalogger& datalogger);

	/*
	 * Opens a connection to the SeedLink server.
	 */
	void connect();

	/*
	 * TODO
	 */
	void close();

private:
	const StationConfiguration& stationConfig;
	Datalogger& datalogger;

	void openStream();
	void collectData();
	void closeStream();
	void packetHandler(char *msrecord, int packet_type, int seqnum, int packet_size);
	void processDataPacket(SLMSrecord * const msr, const time_t start_time);

	std::unique_ptr<std::thread> listenThread;
	std::atomic<bool> exitThread = false;

	char seedlink_addr[30];
	char begin_time[30];
	char end_time[30];
	SLCD* sl_conn = nullptr;              // Struct holding the connection parameters
	SLMSrecord * msr = nullptr;           // MiniSEED record struct
};

#endif /* SEEDLINKSTREAM_H_ */
