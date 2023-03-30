/*
 * WindyPlugin.h
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#ifndef WINDYPLUGIN_H_
#define WINDYPLUGIN_H_

#include "StationConfiguration.h"
#include "SeedLinkStream.h"

class WindyPlugin {
public:
	WindyPlugin(const StationConfiguration& stationConfig);

private:
	SeedLinkStream stream;
	const StationConfiguration& stationConfig;
};

#endif /* WINDYPLUGIN_H_ */
