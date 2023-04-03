# Windy Plugin for EQRWS

A plugin application to feed the data from a [Canterbury Seismic Instruments Ltd](https://csi.net.nz/) EQRWS Weather Station to the Windy weather forecasting service.

## Description

This application reads the weather data from the SeedLink server of an EQRWS weather station by Canterbury Seismic Instruments Ltd. It then performs some basic filtering on the data and then uploads a summary packet every minute to the Windy community servers. This plugin application is designed to run quietly and efficiently in the background.

## Cloning and Compilation

Clone this repository and its submodules with the following command:

	git clone --recurse-submodules https://gitlab.com/tbullen/windy-plugin-for-eqrws.git

TODO: Add steps to compile the libslink library

Then compile the plugin application using the following:

	cd windy-plugin-for-eqrws/
	make -j4

The resultant application is found under `bin/target`.

## Configuring

The application reads its configuration from the `.ini` configuration file at startup. An example file `configuration.ini` is provided in the root directory of this repository.

## Installation

TODO: Add steps for installing using systemd

## Running

The plugin application can be run from the command line with the following

	EQRWS_windy_plugin --config /path/to/configuration



## Usage
Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

## Contributing
State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment
TODO

## License
TODO

## Project status
If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.
