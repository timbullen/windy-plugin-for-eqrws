/*
 * Main.cpp
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#include <csignal>     // SIGPIPE signals
#include <cstdint>
#include <atomic>
#include <string>
#include <iostream>
#include <cstring>
#include <thread>

#include "version.h"
#include "Log.h"
#include "StationConfiguration.h"
#include "WindyPlugin.h"


/**
 * Local Variables
 */

const uint32_t MAIN_LOOP_POLL_PERIOD_ms =       100;
std::atomic<bool> exitApplication =             false;


/**
 * Private Function Declaration
 */

static void sighandler(int signum);


/**
 * Private Function Implementation
 */

void sighandler(int signum)
{
    Log::error("Caught signal: " + std::to_string(signum) + ". Terminating application.");
    exitApplication = true;
}


/**
 * Main
 */

int main(int argc, char **argv)
{
    std::string config_path;

    if (argc > 0)
    {
        for (int i = 0 ; i < argc ; i++)
        {
            if ((strcmp(argv[i], "--version")) == 0
                    || (strcmp(argv[i], "-v") == 0))
            {
                std::cout << "Windy Plugin for EQRWS" << std::endl;
                std::cout << "Version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << std::endl;
                exit(EXIT_SUCCESS);
            }

            if ((strcmp(argv[i], "--help")) == 0
                    || (strcmp(argv[i], "-h") == 0))
            {
                std::cout << "Usage: EQRWS_windy_plugin [options] -c <path/to/configuration>" << std::endl;
                std::cout << " -v, --version          Prints the version number and exits." << std::endl;
                std::cout << " -h, --help             Prints this message and exits." << std::endl;
                std::cout << " -d, --debug <level>    Sets the logging level verbosity." << std::endl;
                std::cout << " -c, --config <path>    Sets the path to the configuration file to use." << std::endl;
                exit(EXIT_SUCCESS);
            }

            if ((strcmp(argv[i], "--debug")) == 0
                    || (strcmp(argv[i], "-d") == 0))
            {
                if (argc > (i + 1))
                {
                    try {
                        int verbose = std::stoi(argv[i+1]);

                        if (verbose >= 0 && verbose < MAX_LOG_LEVEL)
                        {
                            Log::setLogLevel((LogVerbosityLevel_t) verbose);
                        }
                        else {
                            throw std::runtime_error("");
                        }
                    }
                    catch (...) {
                        std::cout << "Please specify the print output verbosity value as a command line parameter. eg -d 1" << std::endl;
                        std::cout << "  0 - No std output." << std::endl;
                        std::cout << "  1 - Print errors only." << std::endl;
                        std::cout << "  2 - Print errors and basic activity messages." << std::endl;
                        std::cout << "  3 - Print errors and detailed activity messages." << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            }

            if ((strcmp(argv[i], "--config")) == 0
                    || (strcmp(argv[i], "-c") == 0))
            {
                if (argc > (i + 1))
                {
                    config_path = std::string(argv[i+1]);
                }
            }
        }
    }

    // Fall-through if the config filepath wasn't provided in the command line args
    if (config_path == "") {
        std::cout << "Please provide the path to the configuration file as a command line parameter. eg --config /etc/EQRWS_Windy_plugin.ini" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Catch SIGTERM signals to perform a graceful shutdown of the system.
    signal(SIGTERM, sighandler);

    // Load the instrument configuration
    StationConfiguration stationConfig;

    try {
    	stationConfig.loadConfig(config_path);
    }
    catch (const std::exception& e) {
    	std::cout << "Failed to load a valid configuration from the file at path '" << config_path << "'." << std::endl;
    	std::cout << "Error: " << e.what() << std::endl;
    	exit(EXIT_FAILURE);
    }

    WindyPlugin plugin(stationConfig);

    plugin.start();

    while (!exitApplication)
    {
    	plugin.run();

        // Sleep here to avoid consuming 100% CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_POLL_PERIOD_ms));
    }

    plugin.close();

    Log::activity("Application finished.");

    exit(EXIT_SUCCESS);
}

