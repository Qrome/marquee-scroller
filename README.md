/* The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

# Marquee Scroller (Clock, Weather, News, and More)

## Features Displays the following every 60 seconds:
* Accurate Clock refresh off Internet Time Servers
* Local Weather and conditions (refreshed every 10 - 30 minutes)
* News Headlines from all the major sources
* Configured through Web Interface
* Display 3D print progress from your OctoPrint Server
* Option to display random goofy advice
* Video: https://youtu.be/DsThufRpoiQ

## Required Parts:
* Wemos D1 Mini: https://amzn.to/2qLyKJd
* Dot Matrix Module: https://amzn.to/2HtnQlD

## Wiring for the Wemos D1 Mini to the Dot Matrix Display
CLK -> D5 (SCK)  
CS  -> D6  
DIN -> D7 (MOSI)  
VCC -> 5V+  
GND -> GND-  

## 3D Printed Case by David Payne:  
https://www.thingiverse.com/thing:2867294

## Compiling and Loading to Wemos D1
It is recommended to use Arduino IDE.  You will need to configure Arduino IDE to work with the Wemos board and USB port and installed the required USB drivers etc.  
* USB CH340G drivers:  https://wiki.wemos.cc/downloads
* Enter http://arduino.esp8266.com/stable/package_esp8266... into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.
* Open Boards Manager from Tools > Board menu and install esp8266 platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).
* Select Board:  "WeMos D1 R2 & mini"

Packages the following packages and libraries are used (download and install):  
<ESP8266WiFi.h>  
<ArduinoJson.h>  --> https://github.com/bblanchon/ArduinoJson  
<ESP8266WebServer.h>  
<WiFiManager.h> --> https://github.com/tzapu/WiFiManager  
"FS.h"  
<SPI.h>  
<Adafruit_GFX.h>  --> https://github.com/adafruit/Adafruit-GFX-Library  
<Max72xxPanel.h> --> https://github.com/markruys/arduino-Max72xxPanel  

## Initial Configuration
You will need to update the **Settings.h** file with your API Keys for the following:  
* Open Weather API Key: http://openweathermap.com/
* News API Key: https://newsapi.org/sources
* GeoNames Key (optional as you can use mine or create your own) http://www.geonames.org/
* Your OctoPrint API Key (optional if you use the OctoPrint status)

## Web Interface
The Marquee Scroller uses the **WiFiManager** so when it can't find the last network it was connected to 
it will become a **AP Hotspot** -- connect to it with your phone and you can then enter your WiFi connection information.

After connected to your WiFi network it will display the IP addressed assigned to it and that can be 
used to open a browser to the Web Interface.  Most everything can be configured there.

The Clock will display the time of the City selected for the weather.

## Contributors
David Payne  
Nathan Glause  
Daniel Eichhorn -- Author of the TimeClient class  
