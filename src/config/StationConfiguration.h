/*
 * StationConfiguration.h
 *
 * A configuration handler class that parses the configuration at startup and holds an
 * internal structure containing all configuration values.
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#ifndef STATIONCONFIGURATION_H_
#define STATIONCONFIGURATION_H_

#include <string>
#include <vector>
#include <array>

#include "PluginDefs.h"


typedef struct SeedLinkConfigutation {
    std::string             networkID;
    std::string             stationID;
    std::string             location;
    uint32_t                resolution_microunits;
    std::string             band;
    std::string             source;
    std::array<std::string, EQRWS_CHANNELS::NUMBER_OF_CHANNELS> subsources;
} SeedLinkConfiguration_t;


typedef struct WindyConfiguration {
    std::string             stationId;
    std::string             stationPassword;
} WindyConfiguration_t;


typedef struct Configuration {
    // Windy Config
    WindyConfiguration_t    windy;

    // Datalogging Config
    uint32_t                uploadPeriod_s;
    uint32_t                windGustDuration_s;

	// EQRWS Instrument Config
	std::string             IPAddress;
	SeedLinkConfiguration_t seedlink;
} Configuration_t;


class StationConfiguration {
public:
	StationConfiguration();

	/*
	 * Loads the instrument configuration from the configuration file at the given filepath.
	 *
	 * NOTE: Throws exceptions for any errors that are encountered when parsing the file.
	 */
	void loadConfig(const std::string& config_filepath);

	/*
	 * Returns the configuration structure for the instrument.
	 *
	 * NOTE: Throws an exception if a valid configuration has not yet been loaded.
	 */
	const Configuration_t& getConfig() const;

private:
	void parseFileContents(std::ifstream& file_stream);
	static std::string getKeyValue(const std::vector<std::string>& lines, const std::string& key);
	static std::string trimWhitespaces(const std::string& str);

	bool loadOK = false;
	Configuration_t config;
};

#endif /* STATIONCONFIGURATION_H_ */
