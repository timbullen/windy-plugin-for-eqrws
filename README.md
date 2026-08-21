# Windy Plugin for EQRWS

A plugin application to feed the live data from a [Canterbury Seismic](https://canterburyseismic.com/) EQR-WS Weather Station to the [Windy](https://www.windy.com/) weather forecasting service.

## How It Works

This application reads the weather data from the SeedLink server of an EQR-WS weather station. It then performs some basic filtering and processing on the data and then uploads a summary packet at user-defined intervals to the Windy community servers, using the [Windy API](https://community.windy.com/topic/8168/report-your-weather-station-data-to-windy) for personal weather stations. This plugin application is designed to run quietly and permanently in the background.

If the connection to your EQRWS weather station is broken, no data updates will be sent to Windy during this time. Once the connection is reestablished, the updates will continue but will not cover the disconnection period.

## Dependencies

This application requires a system that has [curl](https://github.com/curl/curl) installed, which comes pre-installed on most operating systems. Curl is used to perform the HTTPS requests to upload data to the Windy servers.

## Before you start

To use this application you will need to first create a user profile account on the [Windy Stations webpage](https://stations.windy.com/). You must first register your station and then Windy will autogenerate a station ID and password to use.

## Cloning and Compilation

Clone this repository and its submodules with the following command:

```shell
git clone --recurse-submodules https://github.com/timbullen/windy-plugin-for-eqrws.git
```

The plugin must link the `libslink` library in order to read from the SeedLink servers. We compile this with the following:

```shell
cd windy-plugin-for-eqrws/
cd lib/libslink/
make
```

Then compile the plugin application using the following:

```shell
cd -
make -j4
```

The resultant binary application is found under `bin/target`.

## Configuring

The application reads its configuration from the `.ini` configuration file at startup. An example file `configuration.ini` is provided in the root directory of this repository.

| Configuration Field                 | Description                                                                                                                                                                                                                                                           |
|-------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `dataUploadPeriodSecs`              | The frequency of the data uploads to the Windy servers, expressed in seconds. The Windy servers will reject updates that are more frequent than every 5 minutes.                                                                                                      |
| `windGustDurationSecs`              | The duration over which a wind gust is measured. Default is 3 seconds, meaning the current wind gust value is the rolling average of the previous 3 seconds of wind speeds.                                                                                           |
| `windyStationId`                    | The ID of the station that Windy autogenerates for you once you have registered your station.                                                                                                                                                                         |
| `windyStationPassword`              | The password for authenticating the station that Windy autogenerates for you once you have registered your station.                                                                                                                                                   |
| `address`                           | The IP address of your EQRWS instrument on your network. It is recommended that you configure your EQRWS to use a static IP address if it is not using one.                                                                                                           |
| `seedlinkNetworkID`                 | The SeedLink 'Network ID' value for the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                                                            |
| `seedlinkStationID`                 | The SeedLink 'Station ID' value for the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                                                            |
| `seedlinkLocation`                  | The SeedLink stream location for the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                                                               |
| `seedlinkResolutionMicrounits`      | The resolution of the data from the SeedLink server on the EQRWS. Expressed in microunits. Must be one of either: '1' for 1 microunit, '10' for 10 microunits, or '1000' for 1 milliunit. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS. |
| `seedlinkBand`                      | The SeedLink 'Band' value for the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                                                                  |
| `seedlinkSource`                    | The SeedLink 'Source' value for the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                                                                |
| `seedlinkSubsourceWindSpeed`        | The SeedLink 'Subsource' or 'Channel ID' value for the wind speed channel of the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                   |
| `seedlinkSubsourceWindDir`          | The SeedLink 'Subsource' or 'Channel ID' value for the wind direction channel of the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                               |
| `seedlinkSubsourceTemperature`      | The SeedLink 'Subsource' or 'Channel ID' value for the temperature channel of the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                  |
| `seedlinkSubsourcePressure`         | The SeedLink 'Subsource' or 'Channel ID' value for the pressure channel of the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                     |
| `seedlinkSubsourceHumidity`         | The SeedLink 'Subsource' or 'Channel ID' value for the humidity channel of the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                                     |
| `seedlinkSubsourceRainIntensity`    | The SeedLink 'Subsource' or 'Channel ID' value for the rain intensity channel of the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                               |
| `seedlinkSubsourceRainAccumulation` | The SeedLink 'Subsource' or 'Channel ID' value for the rain accumulation channel of the EQRWS. You can see this when viewing the 'SeedLink Settings' webpage of the EQRWS.                                                                                            |

## Installation

The following will install the plugin using systemd to always be running in the background.

```shell
./install-systemd.sh
```

You can then view the status of the service with the following.

```shell
systemctl status windy-plugin
```

And stop and start the process.

```shell
systemctl stop windy-plugin
systemctl start windy-plugin
```

## Usage

The plugin application can be run from the command line with the following

```shell
EQRWS_windy_plugin --config /path/to/configuration
```

The level of verbosity of the output can be adjusted with the following

```shell
EQRWS_windy_plugin --config /path/to/configuration --debug 2
```

Where the levels are

| Level | Description |
| ----- | ----- |
| 0 | No messages of any kind are printed to the console. |
| 1 | Only error messages are printed to the console. |
| 2 | General activity messages are printed to the console. |
| 3 | Detailed activity messages are printed to the console. |

## Contributing

If you have an improvement or have found a bug please feel free to open an issue or add the change and create a pull request. If you file a bug please make sure to include as much information about your environment (compiler version, etc.) as possible to help reproduce the issue.
