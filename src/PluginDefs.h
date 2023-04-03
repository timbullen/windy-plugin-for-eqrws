/*
 * PluginDefs.h
 *
 *  Created on: 31/03/2023
 *      Author: Tim Bullen
 */

#ifndef SRC_PLUGINDEFS_H_
#define SRC_PLUGINDEFS_H_

#include <ctime>
#include <cstdint>
#include <array>

/*
 * Enum of channel indices for the 7-channel EQRWS
 */
namespace EQRWS_CHANNELS {
typedef enum Channels {
    WIND_SPEED = 0,
    WIND_DIR,
    TEMP_C,
    PRESSURE,
    HUMIDITY,
    RAIN_INT,
    RAIN_ACC,
    NUMBER_OF_CHANNELS,
} Channels_t;
}


/*
 * Fixed length array containing a data value for each channel
 */
typedef std::array<float, EQRWS_CHANNELS::NUMBER_OF_CHANNELS> DataArray_t;


/*
 * Sample object combining a data value with a timestamp.
 */
typedef struct Sample {
    Sample() : timestamp(0), data(0.0) {}

    Sample(time_t t, float v) : timestamp(t), data(v) {}

    std::time_t     timestamp;
    float           data;
} Sample_t;


#endif /* SRC_PLUGINDEFS_H_ */
