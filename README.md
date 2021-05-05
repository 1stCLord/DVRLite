# DVRLite
A lightweight DVR for recording from IP Cameras when they send an onvif event. 
This software is licensed for all non commercial use, what constitutes commerical use under this license is at the sole discretion of the license holder, me.

A DVR loosly based on Zoneminder for recording from cameras that support onvif events. It is designed for maximum performance by offloading motion detection to the camera.
This way it doesn't need to decode the video stream and writes it directly to disk, allowing it to easily manage many cameras at once or run on modest hardware.

Use a web based UI to control the application, there's currently no authentication so using it on the open internet is not supported. 
Via the web interface you can add/remove cameras, control the settings and view recorded video, there's currently no support for live video.

Roadmap:
-  implement date selectors on video list ✔
-  implement log filter ✔
-  implement multiple source view
-  implement event message filter ✔
-  implement quota reaping
-  implement live snapshots ✔
-  implement live video
-  implement edit source ✔
-  maybe implement watchdog (doesn't seem to be needed)
-  multiple themes ✔
-  shutdown/restart ✔
-  c++20 timezone support (once it's been added to the supported implementations)

not definites
-  add a more sophisticated post process zoned motion detection
-  add a notification system

![alt text](https://github.com/TheSombreroKid/DVRLite/blob/master/dvrlite.png?raw=true)
