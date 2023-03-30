/*
 * StationConfiguration.h
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#ifndef STATIONCONFIGURATION_H_
#define STATIONCONFIGURATION_H_

#include <string>
#include <vector>


typedef struct Configuration {
	std::string				stationName;
	std::string				windyAPIKey;
	double					latitude;
	double					longitude;
	float					elevation;
	float					transducerHeight;
	std::string				IPAddress;
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
	std::string getKeyValue(const std::vector<std::string>& lines, const std::string& key);
	static std::string trimWhitespaces(const std::string& str);

	bool loadOK = false;
	Configuration_t config;
};

#endif /* STATIONCONFIGURATION_H_ */
