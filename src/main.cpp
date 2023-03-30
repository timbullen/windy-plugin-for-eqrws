/*
 * Main.cpp
 *
 *  Created on: 30/03/2023
 *      Author: Tim Bullen
 */

#include <signal.h>     // SIGPIPE signals
#include <unistd.h>
#include <atomic>
#include <string>
#include <iostream>
#include <cstring>

#include "version.h"
#include "Log.h"
#include "StationConfiguration.h"


/**
 * Thread Instances
 */




/**
 * Local Variables
 */

std::atomic<bool> exitApplication;

// TODO: get this from the command line
const std::string CONFIG_FILEPATH = "/media/sf_shared/windy-plugin-for-eqrws/configuration.ini";


/**
 * Private Function Declaration
 */

static void sighandler(int signum);


/**
 * Public Function Implementation
 */



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
    if (argc > 0)
    {
        for (int i = 0 ; i < argc ; i++)
        {
            if ((strcmp(argv[i], "--version")) == 0
                    || (strcmp(argv[i], "-v") == 0))
            {
                std::cout << "Windy Plugin for EQRWS" << '\n';
                std::cout << "Version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << '\n';
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
                        std::cout << "Please specify the print output verbosity value as a command line parameter. eg -d 1" << '\n';
                        std::cout << "  0 - No std output." << '\n';
                        std::cout << "  1 - Print errors only." << '\n';
                        std::cout << "  2 - Print errors and basic activity messages." << '\n';
                        std::cout << "  3 - Print errors and detailed activity messages." << '\n';
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }

    if (getuid() != 0) {
        std::cout << "This application must be run as root." << std::endl;
        exit(EXIT_FAILURE);
    }

    exitApplication = false;

    // Catch SIGTERM signals to perform a graceful shutdown of the system.
    signal(SIGTERM, sighandler);

    // Load the instrument configuration
    StationConfiguration stationConfig;

    try {
    	stationConfig.loadConfig(CONFIG_FILEPATH);
    }
    catch (const std::exception& e) {
    	std::cout << "Failed to load a valid configuration from the file at path '" << CONFIG_FILEPATH << "'." << std::endl;
    	std::cout << "Error: " << e.what() << std::endl;
    	exit(EXIT_FAILURE);
    }

    while (!exitApplication)
    {
        // Sleep here to avoid consuming 100% CPU
        usleep(1000);
    }

    Log::activity("Application finished.");

    exit(EXIT_SUCCESS);
}

