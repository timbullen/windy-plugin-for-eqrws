#!/bin/sh
# Installs the systemd service on the system

CONFIG_DIR=/etc/EQRWS_windy_plugin
SYSTEMD_DIR=/etc/systemd/system/

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