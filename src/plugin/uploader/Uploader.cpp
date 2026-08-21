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

const std::string WINDY_API_BASE_URL = "https://stations.windy.com/api/v2/";


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
                            float humidity,
                            float rain) const
{
    std::ostringstream sstr;
    sstr << "Uploading data to Windy for timestamp " << std::to_string(timestamp) << std::fixed << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Temperature: "       << std::setw(10) << std::right << std::setprecision(1) << temperature << "°C" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Wind Speed: "        << std::setw(10) << std::right << std::setprecision(1) << wind_speed << " m/s" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Wind Gust Speed: "   << std::setw(10) << std::right << std::setprecision(1) << wind_gust << " m/s" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Wind Direction: "    << std::setw(10) << std::right << (int)wind_dir << "°" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Humidity: "          << std::setw(10) << std::right << std::setprecision(1) << humidity << "%" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Pressure: "          << std::setw(10) << std::right << std::setprecision(1) << pressure << " hPa" << std::endl;
    sstr << "\t" << std::setw(20) << std::left << "Rain in past hour: " << std::setw(10) << std::right << std::setprecision(2) << rain << " mm" << std::endl;
    Log::detailed(sstr.str());

    const std::string url = constructUrl(
            timestamp,
            temperature,
            wind_speed,
            wind_dir,
            wind_gust,
            pressure,
            humidity,
            rain
    );

    performHttpRequest(url);
}


/**
 * Private Methods
 */

std::string Uploader::constructUrl(
        time_t timestamp,
        float temperature,
        float wind_speed,
        float wind_dir,
        float wind_gust,
        float pressure,
        float humidity,
        float rain) const
{
    /*
     * Generates the URL string for the HTTP GET upload request for Windy API v2
     */

    auto config = stationConfig.getConfig();

    std::ostringstream url;
    url << std::fixed;  // Set fixed point expression only
    url << WINDY_API_BASE_URL << "observation/update?";  // station measurement update endpoint

    // Set Windy station ID and auth
    url << "id=" << config.windy.stationId;
    url << "&" << "PASSWORD=" << config.windy.stationPassword;

    // Add the timestamp and the data values to the URL query string
    url << "&" << "ts=" << timestamp;
    url << "&" << "winddir=" << static_cast<int>(wind_dir);
    url << "&" << "wind=" << std::setprecision(1) << wind_speed;
    url << "&" << "gust=" << std::setprecision(1) << wind_gust;
    url << "&" << "temp=" << std::setprecision(1) << temperature;
    url << "&" << "humidity=" << std::setprecision(1) << humidity;
    url << "&" << "pressure=" << static_cast<int>(pressure * 100);  // Convert from hPa to Pa
    url << "&" << "precip=" << std::setprecision(2) << rain;

    return url.str();
}


void Uploader::performHttpRequest(const std::string& url)
{
    /*
     * Makes the HTTP GET request to the windy servers and logs the output.
     */

    // Add the URL in quotes to escape special characters. 2>&1 combines stderr into stdout
    std::string cmd = "curl \"" + url + "\" 2>&1";

    // Create a pipe to the system call so we can read the output
    std::array<char, 200> buffer;
    std::string result = "";

    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        Log::error("Error executing HTTPS upload. Error: " + std::string(strerror(errno)));
        return;
    }
    else {
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
    }

    if (pclose(pipe) != EXIT_SUCCESS) {
        Log::error("Attempted HTTPS upload to Windy returned non-zero exit code.");

        if (result != "") {
            Log::error("Output from HTTPS upload: " + result);
        }
    }
    else if (result != "") {
        Log::detailed("Output from HTTPS upload: " + result);
    }
}
