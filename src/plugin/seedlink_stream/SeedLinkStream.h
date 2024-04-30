/*
 * SeedLinkStream.h
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#ifndef SEEDLINKSTREAM_H_
#define SEEDLINKSTREAM_H_

#include <libslink/libslink.h>      // The client library interface
#include <thread>
#include <atomic>

#include "StationConfiguration.h"
#include "Datalogger.h"


class SeedLinkStream {
public:
	SeedLinkStream(const StationConfiguration& stationConfig, Datalogger& datalogger);

	/*
	 * Opens a connection to the SeedLink server. This will spawn a sub-thread that will
	 * listen for any received packets from the SeedLink server. When data packets are received,
	 * the contents will be unpacked and then forwarded to the Datalogger class.
	 *
	 * NOTE: If any critical errors are encountered in the initialisation of the stream,
	 * then an exception will be thrown.
	 */
	void connect();

	/*
	 * Instructs the sub-thread to exit and then cleans up by rejoining the thread, and freeing
	 * any reserved memory.
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
