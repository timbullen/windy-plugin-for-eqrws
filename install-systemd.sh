#!/bin/sh
# Installs the systemd service on the system

TARGET_NAME=EQRWS_windy_plugin
TARGET=bin/target/$TARGET_NAME
BIN_DIR=/usr/local/bin
CONFIG_DIR=/etc/$TARGET_NAME
SYSTEMD_DIR=/etc/systemd/system/

if [ ! -f $TARGET ]; then
	echo "Plugin binary file $TARGET must be built first before installation can occur. Use the 'make' command"
	exit 1
fi

cp $TARGET $BIN_DIR/
if [ $? -ne 0 ]; then
	echo "Unable to copy binary file to $BIN_DIR"
	exit 1
fi

chmod 777 $BIN_DIR/$TARGET_NAME

mkdir -p $CONFIG_DIR
cp configuration.ini $CONFIG_DIR/
if [ $? -ne 0 ]; then
	echo "Unable to copy configuration file to $CONFIG_DIR"
	exit 1
fi

cp windy-plugin.service $SYSTEMD_DIR/
if [ $? -ne 0 ]; then
	echo "Unable to copy systemd service file to $SYSTEMD_DIR"
	exit 1
file

chmod 666 $SYSTEMD_DIR/windy-plugin.service

# Refresh the systemd cache
systemctl daemon-reload

# Make plugin start automatically at boot
systemctl enable windy-plugin
if [ $? -ne 0 ]; then
	echo "Unable to enable the plugin systemd service"
	exit 1
file

# Start running the plugin now
systemctl start windy-plugin
if [ $? -ne 0 ]; then
	echo "Unable to start running the plugin systemd service"
	exit 1
file

exit 0