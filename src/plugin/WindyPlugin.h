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
#include "Datalogger.h"

class WindyPlugin {
public:
	WindyPlugin(const StationConfiguration& stationConfig);

	/**
	 * TODO
	 */
	void start();

	/**
	 * TODO
	 */
	void run();

	/**
	 * TODO
	 */
	void close();

private:
	Datalogger datalogger;
	SeedLinkStream stream;

	const StationConfiguration& stationConfig;
};

#endif /* WINDYPLUGIN_H_ */
