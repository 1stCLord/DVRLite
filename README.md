# DVRLite
A lightweight DVR for recording from IP Cameras when they send an onvif event. 
This software is licensed for all non commercial use, what constitutes commerical use under this license is at the sole discretion of the license holder, me.

A DVR loosly based on Zoneminder for recording from cameras that support onvif events. It is designed for maximum performance by offloading motion detection to the camera.
This way it doesn't need to decode the video stream and writes it directly to disk, allowing it to easily manage many cameras at once or run on modest hardware.

Use a web based UI to control the application, there's currently no authentication so using it on the open internet is not supported. 
Via the web interface you can add/remove cameras, control the settings and view recorded video, there's currently no support for live video.

**Roadmap:**
* implement date selectors on video list ✔
* implement log filter ✔
* implement multiple source view
* implement event message filter ✔
* implement quota reaping
* implement live snapshots ✔
* implement live video
* implement edit source ✔
* maybe implement watchdog (doesn't seem to be needed)
* multiple themes ✔
* shutdown/restart ✔
* c++20 timezone support (once it's been added to the supported implementations)

**not definites**
* add a more sophisticated post process zoned motion detection
* add a notification system
* authentication system

![alt text](https://github.com/TheSombreroKid/DVRLite/blob/master/dvrlite.png?raw=true)

## Installation:
__Windows:__
There currently isn't any service support for windows, simply download the release extract it to a directory and run the application. 
To run at startup you can add a shortcut to the startup items by hitting start+r and typing shell:startup.

__Linux:__
Either build from source as described below and cmake will automatically install or:
* download the release
* extract the DVRLite binary to /usr/local/bin 
* and the web folder to /usr/local/share/DVRLite/
* and the DVRLite.service file to /usr/local/systemd/system/
* enable the service with _systemctl enable DVRLite_

## Build from source:
The project uses cmake and uses jsoncpp, ffmpeg, oatpp and openssl.
On windows I recommend you use vcpkg and install the dependancies and then open the cmake project in visual studio.
On linux install the dependancies using the system package manager and run the following commands to automatically install as a SystemD service:
cmake .
sudo make install

## Setup
Once the application is running, the web service will default to port 8000.
You can access the web ui by pointing your browser at localhost:8000.
From here you can access the Settings->Config menu in the top right which you can use to set the record path.
![alt text](https://github.com/TheSombreroKid/DVRLite/blob/master/config.png?raw=true)
You can then add an onvif camera by selecting the Add button beside the settings dropdown.
![alt text](https://github.com/TheSombreroKid/DVRLite/blob/master/add.png?raw=true)
This menu lets you configure a camera:
* Name: give the camera a name.
* Address: the onvif address of the camera, for a standard onvif camera the ip address should be all that's needed and the onvif address will be constructed from that, for non conforming cameras you may need to provide the full onvif address.
* Custom Video Address (optional): Some cameras provide higher quality video feeds on different paths from the ones provided in their onvif interface (like reolink), setting them here will force the app to use these feeds instead.
* Username: This is the username needed to log onto the camera.
* Password: This is the password needed to log onto the camera.
* Sources to trigger: This is the sources you want to record when a motion detection event is sent from this camera, self means record footage from the camera you are currently setting up.
* Event Filter (optional): If your camera provides details in it's motion event message you can use this to filter based on the contents of the message, if this is set only motion events with this text will trigger recording.
* Record duration (seconds): The number of seconds a recording will continue after a motion detection event is registered.
* Disk quota (mb): Not currently supported, eventually this will automatically reap the oldest recordings when the disk usage exceeds the specified quota, 0 means no reaping.
* Record Audio: Do you want to record audio from the camera.
* Record Video: Do you want to record video from the camera.