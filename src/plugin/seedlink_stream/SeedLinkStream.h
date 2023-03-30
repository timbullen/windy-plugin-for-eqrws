/*
 * SeedLinkStream.h
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#ifndef SEEDLINKSTREAM_H_
#define SEEDLINKSTREAM_H_

#include "StationConfiguration.h"

class SeedLinkStream {
public:
	SeedLinkStream(const StationConfiguration& stationConfig);

private:
	const StationConfiguration& stationConfig;
};

#endif /* SEEDLINKSTREAM_H_ */
