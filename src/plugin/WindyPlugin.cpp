/*
 * WindyPlugin.cpp
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#include "WindyPlugin.h"

WindyPlugin::WindyPlugin(const StationConfiguration& stationConfig)
	: datalogger(stationConfig),
	  stream(stationConfig, datalogger),
	  stationConfig(stationConfig)
{

}


void WindyPlugin::start()
{
    // Start streaming data from the SeedLink server
	stream.connect();
}


void WindyPlugin::run()
{
    // Check the datalogger periodically
    datalogger.pollDatalogger();
}


void WindyPlugin::close()
{
	stream.close();
}
