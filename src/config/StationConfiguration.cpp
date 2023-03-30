/*
 * StationConfiguration.cpp
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#include <fstream>

#include "StationConfiguration.h"
#include "Log.h"

StationConfiguration::StationConfiguration()
{
}


void StationConfiguration::loadConfig(const std::string& config_filepath)
{
    // Read the config file from disk
    std::ifstream in_file(config_filepath);

    if (in_file.fail()) {
        throw std::runtime_error("Unable to open file for overwriting: " + config_filepath);
    }

    parseFileContents(in_file);

    // Finished
    Log::activity("Loaded the instrument configuration.");
    loadOK = true;
}


const Configuration_t& StationConfiguration::getConfig() const
{
	if (!loadOK) {
		throw std::runtime_error("getConfig() called before a valid configuration has been loaded.");
	}
	return config;
}


/*
 * Private methods
 */

void StationConfiguration::parseFileContents(std::ifstream& file_stream)
{
	/*
	 * Parses the configuration file and writes the configuration values to the
	 * configuration struct. Throws exceptions for any errors that are encountered.
	 */

    // Read through the file line by line and identify the relevant lines
	std::string line;
    std::vector<std::string> lines;

    while (std::getline(file_stream, line)) {
    	lines.push_back(trimWhitespaces(line));
    }

    file_stream.close();

    // Extract all required key/value pairs
    config.stationName = getKeyValue(lines, "stationName");
    config.windyAPIKey = getKeyValue(lines, "windyAPIKey");

    std::string lat = getKeyValue(lines, "latitude");
    try {
    	config.latitude = std::stod(lat);
    }
    catch (...) {
    	throw std::runtime_error("Invalid format provided for latitude value: " + lat + ". Express location coordinates in floating point format, eg. -43.530629");
    }

    std::string longitude = getKeyValue(lines, "longitude");
    try {
    	config.longitude = std::stod(longitude);
    }
    catch (...) {
    	throw std::runtime_error("Invalid format provided for longitude value: " + longitude + ". Express location coordinates in floating point format, eg. -43.530629");
    }

    std::string elevation = getKeyValue(lines, "elevation");
    try {
    	config.elevation = std::stod(elevation);
    }
    catch (...) {
    	throw std::runtime_error("Invalid format provided for elevation value: " + elevation);
    }

    std::string trans_height = getKeyValue(lines, "transducer_height");
    try {
    	config.transducerHeight = std::stod(trans_height);
    }
    catch (...) {
    	throw std::runtime_error("Invalid format provided for transducer height value: " + trans_height);
    }

    config.IPAddress = getKeyValue(lines, "EQRWS_address");
}


std::string StationConfiguration::getKeyValue(const std::vector<std::string>& lines, const std::string& key)
{
	/*
	 * Retrieves the value for the key provided from the array of file lines.
	 */

	for (const auto& line : lines) {
		if (line.find(key) != std::string::npos) {
			return trimWhitespaces(line.substr(line.rfind('=') + 1));
		}
	}

	throw std::runtime_error("Unable to find configuration value for key: " + key);
}


std::string StationConfiguration::trimWhitespaces(const std::string& str)
{
    /*
     * Trim the leading and trailing whitespaces of the string.
     */

    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');

    if (first > str.size()) {   // String contains no non-space characters
        return "";
    }
    return str.substr(first, (last - first + 1));
}

