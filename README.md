Overview
---------

A small user-space daemon to watch the battery charge of devices under /sys/class/power_supply/. 
It sends a notification when the first time charge falls to 20%, and again when it falls to 10%. 

Requirements
-------------

libnotify

Building
---------

mkdir build
cd build
cmake ..
make

Usage
------

`battery-watch path`

... where path is the full path to the device's capacity.

Example:

`battery-watch '/sys/class/power_supply/sony_controller_battery_a4:15:66:68:65:f4/capacity'`

TODO
-----

+ Show device name in alert
+ Monitor multiple devices
+ Monitoring devices with no capacity like laptop batteries
