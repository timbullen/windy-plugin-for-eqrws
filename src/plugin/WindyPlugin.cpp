/*
 * WindyPlugin.cpp
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#include "WindyPlugin.h"

WindyPlugin::WindyPlugin(const StationConfiguration& stationConfig)
	: stream(stationConfig),
	  stationConfig(stationConfig)
{

}

