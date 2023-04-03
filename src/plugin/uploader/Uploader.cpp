/*
 * Uploader.cpp
 *
 *  Created on: 31/03/2023
 *      Author: Tim Bullen
 */

#include <errno.h>      // Error integers
#include <cstring>      // Contains strerror() function
#include <sstream>
#include <iomanip>

#include "Uploader.h"
#include "Log.h"

Uploader::Uploader(const StationConfiguration& stationConfig)
    : stationConfig(stationConfig)
{

}


void Uploader::checkCurl()
{
    // Perform a simple query of the curl version. If curl is not present, throw an exception.
    if (system("curl -V") != 0) {
        throw std::runtime_error("Unable to verify the existence of curl on the system. Please install curl in order to run this program.");
    }
}


void Uploader::performUpload(time_t timestamp,
                            float temperature,
                            float wind_speed,
                            float wind_dir,
                            float wind_gust,
                            float pressure,
                            float humidity) const
{
    std::ostringstream sstr;
    sstr << "Uploading data to Windy for timestamp " << std::to_string(timestamp) << std::fixed << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Temperature: "       << std::setw(10) << std::right << std::setprecision(1) << temperature << "°C" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Wind Speed: "        << std::setw(10) << std::right << std::setprecision(1) << wind_speed << " m/s" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Wind Gust Speed: "   << std::setw(10) << std::right << std::setprecision(1) << wind_gust << " m/s" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Wind Direction: "    << std::setw(10) << std::right << (int)wind_dir << "°" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Humidity: "          << std::setw(10) << std::right << std::setprecision(1) << humidity << "%" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Pressure: "          << std::setw(10) << std::right << std::setprecision(1) << pressure << " hPa" << std::endl;
    Log::detailed(sstr.str());

    // Create the URL string for the data upload
    std::ostringstream url;
    url << std::fixed;  // Set fixed point expression only
    url << "https://stations.windy.com/pws/update/" << stationConfig.getConfig().windyAPIKey;

    // Set the station number
    url << "?" << "station=" << 0;  // TODO: make this configurable for users with multiple stations

    // Add the timestamp and the data values to the URL query string
    url << "&" << "ts=" << timestamp;
    url << "&" << "winddir=" << static_cast<int>(wind_dir);
    url << "&" << "wind=" << std::setprecision(1) << wind_speed;
    url << "&" << "gust=" << std::setprecision(1) << wind_gust;
    url << "&" << "temp=" << std::setprecision(1) << temperature;
    url << "&" << "rh=" << std::setprecision(1) << humidity;
    url << "&" << "pressure=" << std::setprecision(1) << pressure;

    // Add the URL in quotes to escape special characters. 2>&1 combines stderr into stdout
    std::string cmd = "curl \"" + url.str() + "\" 2>&1";

    // Create a pipe to the system call so we can read the output
    std::array<char, 200> buffer;
    std::string result = "";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);

    if (!pipe) {
        Log::error("Error executing HTTPS upload. Error: " + std::string(strerror(errno)));
    }
    else {
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }

    if (result != "") {
        Log::detailed("Received response from HTTPS upload: " + result);
    }
}

