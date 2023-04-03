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
	 * Initialises the plugin and its subsystems.
	 */
	void start();

	/**
	 * Run the plugin. This should be polled inside the main loop.
	 */
	void run();

	/**
	 * Exits all subprocesses and cleanly closes the plugin.
	 */
	void close();

private:
	Datalogger datalogger;
	SeedLinkStream stream;
};

#endif /* WINDYPLUGIN_H_ */
