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

## Features include:
* Accurate Clock refresh off Internet Time Servers
* Local Weather and conditions (refreshed every 10 - 30 minutes)
* News Headlines from all the major sources
* Configured through Web Interface
* Display 3D print progress from your OctoPrint Server
* Option to display Bitcoin current value
* Basic Authorization around Configuration web interface
* Support for OTA (loading firmware over WiFi)
* Update firmware through web interface
* Configurable scroll speed
* Configurable scrolling frequency
* Configurable number of LED panels
* Options of different types of Clock Displays (display seconds or temperature) -- only on 8 or more display panels
* Video: https://youtu.be/DsThufRpoiQ
* Build Video by Chris Riley: https://youtu.be/KqBiqJT9_lE

## Required Parts:
* Wemos D1 Mini: https://amzn.to/2qLyKJd
* Dot Matrix Module: https://amzn.to/2HtnQlD  

Note: Using the links provided here help to support these types of projects. Thank you for the support.  

## Wiring for the Wemos D1 Mini to the Dot Matrix Display
CLK -> D5 (SCK)  
CS  -> D6  
DIN -> D7 (MOSI)  
VCC -> 5V+  
GND -> GND-  

![Marquee Scroller Wiring](/images/marquee_scroller_pins.png)  


Added Pushbutton to turn display on / off and a Locally attached DHT 11/22 Temp sensor

![Marquee Parts](/images/PushButton-Temp.png)

## 3D Printed Case by David Payne:  
Original Single Panel version: https://www.thingiverse.com/thing:2867294  
Double Wide LED version: https://www.thingiverse.com/thing:2989552  

## Upgrading from version 2.5 or Higher
Version 2.5 introduced the ability to upgrade pre-compiled firmware from a binary file.  In version 2.6 and on you should find binary files that can be uploaded to your marque scrolling clock via the web interface.  From the main menu in the web interface select "Firmware Update" and follow the prompts.
* **marquee.ino.d1_mini_2.6.bin** - compiled for Wemos D1 Mini and standard 4x1 LED (default)
* **marquee.ino.d1_mini_wide_2.6.bin** - compiled for Wemos D1 Mini and double wide 8x1 LED display

## Compiling and Loading to Wemos D1
It is recommended to use Arduino IDE.  You will need to configure Arduino IDE to work with the Wemos board and USB port and installed the required USB drivers etc.  
* USB CH340G drivers:  https://wiki.wemos.cc/downloads
* Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.  This will add support for the Wemos D1 Mini to Arduino IDE.
* Open Boards Manager from Tools > Board menu and install esp8266 platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).
* Select Board:  "WeMos D1 R2 & mini" or "LOLIN(WEMOS) D1 R2 & mini"
* Set 1M SPIFFS -- this project uses SPIFFS for saving and reading configuration settings.

## Loading Supporting Library Files in Arduino
Use the Arduino guide for details on how to installing and manage libraries https://www.arduino.cc/en/Guide/Libraries  
**Packages** -- the following packages and libraries are used (download and install):  
<ESP8266WiFi.h>  
<ESP8266WebServer.h>  
<WiFiManager.h> --> https://github.com/tzapu/WiFiManager  
"FS.h"  
<SPI.h>  
<Adafruit_GFX.h>  --> https://github.com/adafruit/Adafruit-GFX-Library  
<Max72xxPanel.h> --> https://github.com/markruys/arduino-Max72xxPanel  
<JsonStreamingParser.h> --> https://github.com/squix78/json-streaming-parser  

Note ArduinoJson (version 5.13.1) is now included as a library file in version 2.7 and later.

## Initial Configuration
Starting with version 2.0 editing the **Settings.h** file is optional.  All API Keys are now managed in the Web Interface except for the GeoNames Key. It is not required to edit the Settings.h file before loading and running the code.  
* Open Weather Map API Key: http://openweathermap.org/
* News API Key: https://newsapi.org/
* GeoNames Key (optional as you can use mine or create your own) http://www.geonames.org/ -- this key is only managed in the Settings.h file.
* Your OctoPrint API Key (optional if you use the OctoPrint status)
* Version 2.0 supports Chained 4x1 LED displayes -- configure in the Settigns.h file.  

NOTE: The settings in the Settings.h are the default settings for the first loading. After loading you will manage changes to the settings via the Web Interface. If you want to change settings again in the settings.h, you will need to erase the file system on the Wemos or use the “Reset Settings” option in the Web Interface.  

## Web Interface
The Marquee Scroller uses the **WiFiManager** so when it can't find the last network it was connected to 
it will become a **AP Hotspot** -- connect to it with your phone and you can then enter your WiFi connection information.

After connected to your WiFi network it will display the IP addressed assigned to it and that can be 
used to open a browser to the Web Interface.  You will be able to manage your API Keys through the web interface.  
The default user / password for the configuration page is: admin / password  

The Clock will display the time of the City selected for the weather.  

<p align="center">
  <img src="/images/2018-04-19%2006.58.05.png" width="200"/>
  <img src="/images/2018-04-19%2006.58.15.png" width="200"/>
  <img src="/images/2018-04-19%2006.58.32.png" width="200"/>
  <img src="/images/2018-04-19%2006.58.58.png" width="200"/>
</p>

## Donation or Tip
Please do not feel obligated, but donations and tips are warmly welcomed.  I have added the donation button at the request of a few people that wanted to contribute and show appreciation.  Thank you, and enjoy the application and project.  

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=A82AT6FLN2MPY)

## Contributors
David Payne  
Nathan Glaus  
Daniel Eichhorn -- Author of the TimeClient class  
yanvigdev  
nashiko-s  

Contributing to this software is warmly welcomed. You can do this basically by forking from master, committing modifications and then making a pulling requests against the latest DEV branch to be reviewed (follow the links above for operating guide). Detailed comments are encouraged. Adding change log and your contact into file header is encouraged. Thanks for your contribution.

When considering making a code contribution, please keep in mind the following goals for the project:
* User should not be required to edit the Settings.h file to compile and run.  This means the feature should be simple enough to manage through the web interface.
* Changes should always support the recommended hardware (links above).

![Marquee Scroller](/images/5d7f02ccbf01125cabbf246f97f2ead1_preview_featured.jpg)  
![Marquee Parts](/images/1ffa0c835554d280258c13be5513c4fe_preview_featured.jpg)

