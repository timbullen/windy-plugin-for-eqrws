/*
 * StationConfiguration.cpp
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#include <fstream>

#include "StationConfiguration.h"
#include "Log.h"


const int MIN_DATA_UPLOAD_PERIOD_s              = 60;
const int MAX_DATA_UPLOAD_PERIOD_s              = 6 * 60 * 60;

const int SEEDLINK_NETWORK_ID_LEN               = 2;
const int SEEDLINK_STATION_ID_LEN               = 4;
const int SEEDLINK_BAND_LEN                     = 1;
const int SEEDLINK_SOURCE_LEN                   = 1;
const int SEEDLINK_CHANNEL_ID_LEN               = 1;

const int MIN_SEEDLINK_LOCATION                 = 0;
const int MAX_SEEDLINK_LOCATION                 = 99;


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
    Log::activity("Successfully loaded the instrument configuration.");
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
    std::string key, value;


    key = "dataUploadPeriodSecs";
    try {
        value = getKeyValue(lines, key);
        int num = std::stoi(value);
        if (num < MIN_DATA_UPLOAD_PERIOD_s || num > MAX_DATA_UPLOAD_PERIOD_s) {
            throw std::runtime_error("Value must be between " + std::to_string(MIN_DATA_UPLOAD_PERIOD_s) + " and " + std::to_string(MAX_DATA_UPLOAD_PERIOD_s) + " seconds.");
        }
        config.uploadPeriod_s = num;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "stationNumber";
    try {
        value = getKeyValue(lines, key);
        config.stationNumber = std::stoi(value);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "windyAPIKey";
    try {
        config.windyAPIKey = getKeyValue(lines, key);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "address";
    try {
        config.IPAddress = getKeyValue(lines, key);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "seedlinkNetworkID";
    try {
        value = getKeyValue(lines, key);
        if (value.size() != SEEDLINK_NETWORK_ID_LEN) {
            throw std::runtime_error("Value must be " + std::to_string(SEEDLINK_NETWORK_ID_LEN) + " character(s) long.");
        }
        config.seedlink.networkID = value;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "seedlinkStationID";
    try {
        value = getKeyValue(lines, key);
        if (value.size() != SEEDLINK_STATION_ID_LEN) {
            throw std::runtime_error("Value must be " + std::to_string(SEEDLINK_STATION_ID_LEN) + " character(s) long.");
        }
        config.seedlink.stationID = value;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "seedlinkLocation";
    try {
        value = getKeyValue(lines, key);
        int num = std::stoi(value);
        if (num < MIN_SEEDLINK_LOCATION || num > MAX_SEEDLINK_LOCATION) {
            throw std::runtime_error("Value must be between " + std::to_string(MIN_SEEDLINK_LOCATION) + " and " + std::to_string(MAX_SEEDLINK_LOCATION) + ".");
        }
        config.seedlink.location = num;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "seedlinkResolutionMicrounits";
    try {
        value = getKeyValue(lines, key);
        int num = std::stoi(value);
        if (num != 1 && num != 10 && num != 1000) {
            throw std::runtime_error("Value must be one of: 1, 10 or 1000");
        }
        config.seedlink.resolution_microunits = num;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "seedlinkBand";
    try {
        value = getKeyValue(lines, key);
        if (value.size() != SEEDLINK_BAND_LEN) {
            throw std::runtime_error("Value must be " + std::to_string(SEEDLINK_BAND_LEN) + " character(s) long.");
        }
        config.seedlink.band = value;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    key = "seedlinkSource";
    try {
        value = getKeyValue(lines, key);
        if (value.size() != SEEDLINK_SOURCE_LEN) {
            throw std::runtime_error("Value must be " + std::to_string(SEEDLINK_SOURCE_LEN) + " character(s) long.");
        }
        config.seedlink.source = value;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
    }


    const std::array<std::string, EQRWS_CHANNELS::NUMBER_OF_CHANNELS> channel_keys ({
        "seedlinkSubsourceWindSpeed",
        "seedlinkSubsourceWindDir",
        "seedlinkSubsourceTemperature",
        "seedlinkSubsourcePressure",
        "seedlinkSubsourceHumidity",
        "seedlinkSubsourceRainIntensity",
        "seedlinkSubsourceRainAccumulation"
    });

    for (uint8_t channel_index = 0; channel_index < EQRWS_CHANNELS::NUMBER_OF_CHANNELS; channel_index++) {
        key = channel_keys.at(channel_index);
        try {
            value = getKeyValue(lines, key);
            if (value.size() != SEEDLINK_CHANNEL_ID_LEN) {
                throw std::runtime_error("Value must be " + std::to_string(SEEDLINK_CHANNEL_ID_LEN) + " character(s) long.");
            }
            config.seedlink.subsources.at(channel_index) = value;
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unable to parse value for key: '" + key + "'. Value: " + value + ". Error: " + std::string(e.what()));
        }
    }
}


std::string StationConfiguration::getKeyValue(const std::vector<std::string>& lines, const std::string& key)
{
	/*
	 * Retrieves the value for the key provided from the array of file lines.
	 */

	for (const auto& line : lines) {
		if (line.find(key) != std::string::npos) {
			std::string result = trimWhitespaces(line.substr(line.rfind('=') + 1));
			if (result == "") {
			    throw std::runtime_error("Empty value was provided for key: " + key);
			}
			return result;
		}
	}

	throw std::runtime_error("Unable to find key: " + key);
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

